package com.sitech.crmpd.inter.offon.utils;

import com.sitech.crmpd.inter.offon.mgr.CmdDataAck;
import org.slf4j.Logger;
import org.yaml.snakeyaml.Yaml;

import java.io.File;
import java.io.FileInputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * Created by guanyf on 7/31/2016.
 * 指令配置处理模块， 包括指令文件的解析保存， 以及指令内容的生成（变量替换）， 一个配置文件一个实例
 *
 * 有两个附带的转换要求 ( 在 OrderPare 中实现）：
 * 1. apn 名称到 template_id 的转换
 * 2. ip 从 hex -> int 的转换
 */
public class OrderConfigure {
    private String config_file_path;
    private long config_mod_time = 0L;

    private String header;
    private String footer;

    private Map<String, String> orders;
    private QueryParse queryparse; //查询依赖指令的 变量替换 配置保存
    private final Formater fmt = new Formater();

    /**
     *
     * @param config_file_path  指令文件路径
     * @param log               日志对象
     */
    public OrderConfigure(String config_file_path, Logger log) {
        this.config_file_path = config_file_path;
        loadConf(log);
    }

    public void loadConf(Logger log) {
        File f = new File(config_file_path);
        if(f.lastModified() <= config_mod_time)
            return;

        Map m = null;
        try{
            m =(Map)new Yaml().load(new FileInputStream(f));
        }catch(Throwable e){
            log.error("order file {} load failed", config_file_path, e);
            return;
        }

        header = (String)m.get("header");
        footer = (String)m.get("footer");
        HashMap<String, String> orders = new HashMap<String, String>();
        QueryParse queryparse = new QueryParse(log);

        for(Object o: m.keySet()){
            String key = (String)o;
            if(key.startsWith("OP")){ //指令配置
                String value = String.valueOf(m.get(key));
                String ordercode = key.substring(2);
                orders.put(ordercode, value);
            }else if(key.startsWith("QP")){ //查询参数配置
                log.debug("add qp: {}", key);
                queryparse.addQP(key, m.get(key));
            }
        }
        this.orders = orders;
        this.queryparse = queryparse;

        config_mod_time = f.lastModified();
        log.info("order Config {} loaded, orders={} qorders={}",
                new Object[]{config_file_path, orders.size(), queryparse.size() });
    }

    /**
     * 返回全部的指令代码
     * @return
     */
    public Set<String> ordercodes() {
        return orders.keySet();
    }

    final private void specialDeal(String orderbody, Map<String, String> vars) {
        if (orderbody.indexOf("${strutil.subStringTo(imsi,2,5)}") >= 0) {
            String t = "strutil.subStringTo(imsi,2,5)";
            String s = vars.get("imsi");
            if(s == null)
                vars.put(t, "");
            else if(s.length() < 5)
                vars.put(t, s);
            else
                vars.put(t, s.substring(2, 5));
        }
        if (orderbody.indexOf("${strutil.subStringTo(ssInfo2,2,5)}") >= 0) {
            String t = "strutil.subStringTo(ssInfo2,2,5)";
            String s = vars.get("ssInfo2");
            if(s == null)
                vars.put(t, "");
            else if(s.length() < 5)
                vars.put(t, s);
            else
                vars.put(t, s.substring(2, 5));
        }
    }
    /**
     * 第一次的指令报文生成
     * @param cmd
     * @param task
     */
    public void genOrder(CmdDataAck cmd, OrderTask task, Map<String, String> vars) {
        String ordercode = cmd.ordercode;
        String queryorder = queryparse.queryOrder(ordercode);
        if(queryorder != null) { //查询依赖指令
            ordercode = queryorder;
            task.cmd = cmd.clone();
            task.isQuery = true;
            //log.info("query order {} for {}", queryorder, cmd.ordercode);
        }else{
            task.isQuery = false;
        }

        String orderbody = orders.get(ordercode);
        if(orderbody == null) {
            task.request_str = null; //无指令配置
            return;
        }

        cmd.varsPut(vars);
        specialDeal(orderbody, vars);
        task.request_str = fmt.format(header + orderbody + footer, vars); //替换指令中的变量
        task.fromAck(cmd); //保存指令参数
    }

    /**
     * 对于查询依赖指令， 查询指令返回后生成最终的指令报文
     * @param task
     */
    public void genOrder(OrderTask task, Map<String, String> vars) {
        String ordercode = task.ordercode;
        String query_result = task.response_str;

        queryparse.parseQueryResult(task.cmd, query_result, vars);
        String orderbody = orders.get(ordercode);
        task.request_str = fmt.format(header + orderbody + footer, vars);
        task.isQuery = false;
        task.cmd = null;
    }
}
