package com.sitech.crmpd.inter.offon.exec;

import com.google.common.base.Splitter;
import com.sitech.crmpd.inter.common.*;
import com.sitech.crmpd.inter.offon.exec.ConfigureManager;
import com.sitech.crmpd.inter.offon.mgr.BasePkt;
import com.sitech.crmpd.inter.offon.mgr.CmdDataAck;
import com.sitech.crmpd.inter.offon.mgr.CmdDataReq;
import com.sitech.crmpd.inter.offon.mgr.LoginAck;
import com.sitech.crmpd.inter.offon.mgr.LoginReq;
import com.sitech.crmpd.inter.offon.utils.*;
import org.slf4j.Logger;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.sitech.crmpd.inter.offon.mgr.*;

/**
 * Created by guanyf on 7/31/2016.
 *
 * 一个执行端口的配置信息
hei.1:
  manager.ip: 127.0.0.1
  manager.port: 22109
  manager.user: hlrhei
  manager.password: hlrhei
  term.type: 1
  remote.url: http://10.112.141.121:8080/LTEProxy/LTESOAPEndPoint
  order.path: /offon2/gyf/test/orders/ERI_mianyang.yaml
  return.desc.pattern: "<ResultDesc>(.+)</ResultDesc>;<faultstring>(.+)</faultstring>"
  return.code.pattern: "<ResultCode>(.+)</ResultCode>"
  result.map: resultmap.txt
  APN_TPL_ID:
  IPHEX_TO_INT:
  [execute.max]: 1      可以并行执行的指令数， 默认为1
  PROPERTIES: HLRSN=2,USER=boss,PWD=boss
  caller.1:
    remote.url: http://10.109.231.85:8080/spg
    order.path: orders/enum.yaml
    return.desc.pattern: "<ResultDesc>(.+)</ResultDesc>;<faultstring>(.+)</faultstring>"
    return.code.pattern: "<ResultCode>(.+)</ResultCode>"
    result.map: resultmap.txt
    PROPERTIES: HLRSN=2,USER=boss,PWD=boss

 *
 */
public abstract class BasePort {
    protected String portname;


    // ordercode -> NetworkEntity, 以支持多种网元执行指令
    protected Map<String, NetworkEntity> orders = new HashMap<String, NetworkEntity>();
    protected OrderPrepare orderPrepare;
    protected ConfigureManager cm;
    protected String basePath;
    protected Map conf;
    private int maxExec = 1; //可以并行执行的指令数， 默认为1

    protected BasePort(String basePath, String portname, Map conf, ConfigureManager cm) {
        this.cm = cm;
        this.portname = portname;
        this.conf = conf;
        this.basePath = basePath;
    }

    public void initOrderConf(Logger log) {
        String ip_hex2int = null;
        if(conf.containsKey("IPHEX_TO_INT"))
            ip_hex2int = (String)conf.get("IPHEX_TO_INT");
        String apnorders = null;
        if(conf.containsKey("APN_TPL_ID"))
            apnorders = (String)conf.get("APN_TPL_ID");
        orderPrepare = new OrderPrepare(apnorders, ip_hex2int);

        NetworkEntity c = genNE("__caller.0", basePath, conf, log);
        for(String ordercode: c.ordercfg.ordercodes()){
            orders.put(ordercode, c);
        }
        for(Object o: conf.keySet()) {
            String key = (String)o;
            Object v = conf.get(o);
            if(key.startsWith("caller.") && v instanceof  Map){
                c = genNE(key, basePath, (Map)v, log);
                for(String ordercode: c.ordercfg.ordercodes()){
                    orders.put(ordercode, c);
                }
            }
        }
        if(conf.containsKey("execute.max")) {
            maxExec = (Integer)conf.get("execute.max");
        }
    }

    private NetworkEntity genNE(String name, String basePath, Map conf, Logger log) {
        String url = String.valueOf(conf.get("remote.url"));
        String order_path = String.valueOf(conf.get("order.path")); //: orders/enum.yaml
        String descPattern = String.valueOf(conf.get("return.desc.pattern")); //: "<ResultDesc>(.+)</ResultDesc>;<faultstring>(.+)</faultstring>"
        String codePattern = String.valueOf(conf.get("return.code.pattern")); //: "<ResultCode>(.+)</ResultCode>"
        String resultMap = String.valueOf(conf.get("result.map")); //: resultmap.txt
        if( !order_path.startsWith("/")){
            order_path = basePath + File.pathSeparator + order_path;
        }
        if(!resultMap.startsWith("/"))
            resultMap = basePath + File.pathSeparator + resultMap;

        OrderConfigure oc = cm.getOrderConf(order_path);
        String prop_str = "";
        if(conf.containsKey("PROPERTIES")){
            Splitter.MapSplitter splitter = Splitter.on(',').omitEmptyStrings().trimResults()
                    .withKeyValueSeparator('=');
            prop_str = (String)conf.get("PROPERTIES");
        }
        return new NetworkEntity(name, url, oc, codePattern, descPattern, resultMap, prop_str);
    }

    private void exec(CmdDataAck cmd, OrderTask task, ArrayBlockingQueue<OrderTask> q) {
        String ordercode = cmd.ordercode;
        if(!orders.containsKey(ordercode)) {
            task.request_str = null;
            q.offer(task);
            return;
        }
        orderPrepare.prepare(cmd);
        NetworkEntity ne = orders.get(ordercode);
        task.c = ne;
        ne.exec(cmd, task, q);
    }
    // 查询后执行
    private void exec(OrderTask task, ArrayBlockingQueue<OrderTask> q) {
        task.c.exec(task, q);
    }
    private void parseReply(OrderTask task, CmdDataReq req) {
        task.c.parseResult(task, req);
    }

    public static class NetworkEntity {
        protected String name; //name of caller
        protected String url;
        protected OrderConfigure ordercfg;
        protected List<Pattern> codePatterns;
        protected List<Pattern> descPatterns;
        protected ResultCodeMap resultCodeMap;
        private Map<String, String> props;
        private HttpExecutor executor;

        protected NetworkEntity(String name, String url, OrderConfigure ordercfg, String codepattern,
                                String descpattern, String resultcodemap, String prop_str) {
            this.name = name;
            this.url = url;
            this.ordercfg = ordercfg;
            codePatterns = new LinkedList<Pattern>();
            String[] x = codepattern.split(";");
            for(String ps: x){
                codePatterns.add(Pattern.compile(ps));
            }
            descPatterns = new LinkedList<Pattern>();
            x = descpattern.split(";");
            for(String ps: x){
                descPatterns.add(Pattern.compile(ps));
            }
            resultCodeMap = new ResultCodeMap(resultcodemap);

            Splitter.MapSplitter splitter = Splitter.on(',').omitEmptyStrings().trimResults()
                    .withKeyValueSeparator('=');
            props = splitter.split(prop_str);

            executor = HttpExecutor.getInstance(url);
        }

        void exec(CmdDataAck cmd, OrderTask task, ArrayBlockingQueue<OrderTask> q){
            ordercfg.genOrder(cmd, task, props);
            if(task.request_str == null){
                q.offer(task);
                return;
            }
            executor.execute(task, q);
        }

        // 查询后执行
        void exec(OrderTask task, ArrayBlockingQueue<OrderTask> q){
            ordercfg.genOrder(task, props);
            if(task.request_str == null){
                q.offer(task);
                return;
            }
            executor.execute(task, q);
        }

        final private String matchList(String data, List<Pattern> patterns) {
            for(Pattern p: patterns){
                Matcher matcher = p.matcher(data);
                String s = null;
                if(matcher != null && matcher.find()){
                    s = matcher.group(1);
                }
                if(s != null){
                    return s;
                }
            }
            return null;
        }

        // 解析结果串, 包括映射错误代码
        void parseResult(OrderTask task, CmdDataReq req) {
            String data = task.response_str;
            String code = matchList(data, codePatterns);
            if(code == null){
                int rcode = resultCodeMap.mapcode(data);
                if(rcode == -1){
                    req.retn = 7711;
                    req.info = "can not parse result code!";
                }else{
                    req.retn = rcode;
                }
            }else{
                req.retn = Integer.parseInt(code);
                if(req.retn != 0) {
                    int rcode = resultCodeMap.mapcode(data);
                    if(rcode != -1)
                        req.retn = rcode;
                }
            }

            req.info = matchList(data, descPatterns);
        }
    }


    protected abstract boolean connect(Logger log);
    protected abstract int getOrder(CmdDataReq req, CmdDataAck ack, Logger log);
    protected abstract void close(Logger log);

    private boolean running = false;
    private ReentrantLock lock = new ReentrantLock();
    public void for_ever(Logger log) {
//        Logger log = LoggerUtil.getLogger(basePath, portname, "SOAP");

        if (!connect(log)) {
            log.warn("can not connect to manager");
            return;
        }

        log.info("manager port connected.");

        ArrayBlockingQueue<OrderTask> replyQ = new ArrayBlockingQueue<OrderTask>(maxExec);
        int curExecutor = 0; //当前在执行的指令数量

        final CmdDataReq req = new CmdDataReq();
        final CmdDataAck ack = new CmdDataAck();
        final byte[] buf = new byte[2048];
        req.type = CmdDataReq.ONELY_GET;
        int ret;
        long t1, t2;
        running = true;

        OrderTask task = null;
        boolean last_no_order = false;
        lock.lock();
        while (running) {
            // 1. try to poll reply from executors thread pool
            task = null;
            try {
                if(curExecutor > 0 && curExecutor >= maxExec) {
                    task = replyQ.poll(5, TimeUnit.SECONDS); // poll wait timeout
                }else if(curExecutor > 0 && !last_no_order){ //有在执行的指令， 并且上次获取指令成功， 则不等待持续获取指令
                    task = replyQ.poll();
                }else if(curExecutor > 0) { //有在执行的指令, 且上次无指令， 则需避免无等待循环
                    task = replyQ.poll(5, TimeUnit.SECONDS);
                }
            } catch (InterruptedException e) {
                log.error("interrupted", e);
                break;
            }
            // 2. try to parse reply str
            if(task == null){
                if(curExecutor >= maxExec)
                    continue; // if exeed maxExecutor, don'task get more command
                req.type = CmdDataReq.ONELY_GET;
            }else{
                if(task.isQuery){
                    exec(task, replyQ);
                    continue; //查询后执行指令， 不经过manager
                }else {
                    curExecutor--;
                    req.type = CmdDataReq.REPLY_GET;
                    task.toReq(req); //要送回应答数据
                    parseReply(task, req);
                    t2 = System.currentTimeMillis();
                    log.info("order_time:{} tm:{}(ms) retn:{} desc:{}", new Object[]{task.ordercode,
                            t2 - task.t1, req.retn, req.info});
                }
            }
            // 3. try to get order from manager
            ret = getOrder(req, ack, log);
            if (ret <= 0) {
                log.error("getOrder failed[{}], break", ret);
                break;
            }
            if (ack.retn == 3001) {
                if(curExecutor == 0) {
                    log.debug("no data, sleep 5s");
                    try {
                        Thread.sleep(5000L);
                    } catch (final Exception ex) {
                    }
                }
                last_no_order = true;
                continue;
            }
            last_no_order = false;

            log.info(
                    "order to exec: ordercode[{}]stream_id[{}]phone[{}]imsi[{}]info1[{}]info2[{}]info3[{}]",
                    new Object[] { ack.ordercode, ack.stream_id, ack.phone_no, ack.imsi_no,
                            ack.ss_info1, ack.ss_info2, ack.ss_info3 });

            // 4. push task to thread pool
            curExecutor ++;
            if(task == null) {
                task = new OrderTask();
                task.logger = log;
            }
            task.fromAck(ack);
            exec(ack, task, replyQ);
        }
        close(log);
        running = false;
        log.info("worker thread {} ended!!", portname);
        lock.unlock();
    }

    public void stop() {
        running = false;
        lock.lock();

        lock.unlock();
    }

    public boolean isRunning() { return running; }
}
