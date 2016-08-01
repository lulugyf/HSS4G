package com.sitech.crmpd.inter.offon.exec;

import com.sitech.crmpd.inter.offon.utils.LoggerUtil;
import com.sitech.crmpd.inter.offon.utils.OrderConfigure;
import org.slf4j.Logger;
import org.yaml.snakeyaml.Yaml;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Created by guanyf on 7/31/2016.
 * 管理 hss.yaml 配置文件
 *
 * 配置文件更新生效的规则： 重启一个port的时候， 对应端口的配置会使用新的配置， 在配置更新以前启动的端口线程仍然使用旧的配置
 */
public class ConfigureManager {
    private Map<String, BasePort> ports;
    private Map<String, OrderConfigure> orderConfs = new HashMap<String, OrderConfigure>();


    private String poolConf = null; // 连接池配置文件路径
    private long poolConf_mod = 0L; //连接池配置文件更新时间
    private ConcurrentHashMap<String, HttpExecutor> executors = new ConcurrentHashMap<String, HttpExecutor>();
    private Map _default_pool_conf_; //默认配置， 没有在配置文件中的url， 就是用这个配置


    private long config_mod_time = 0L;
    private String fpath;
    private Logger log;
    private String basePath;

    private String hand_port = null;
    private String cmdFile = null;

    private long check_time = 0L;
    private int check_interval = 120 * 1000; //检查时间间隔120s

    /**
     * 创建一个ConfigureManager, 连接 offon_manager 运行的
     *
     * @param fpath       主配置文件路径
     * @param basePath    配置中的其它配置文件路径， 如果没有以 / 开始， 则以此路径为起始
     */
    public ConfigureManager(String fpath, String basePath) {
        this(fpath, basePath, null, null);
        this.fpath = fpath;
        log = LoggerUtil.getLogger(System.getenv("LOGDIR"), "log", "portMgr");
    }

    /**
     * 创建一个ConfigureManager, 手工输入命令或者从文件读入命令
     * @param fpath        主配置文件路径
     * @param basePath     配置中的其它配置文件路径， 如果没有以 / 开始， 则以此路径为起始
     * @param portname     启动哪一个配置
     * @param cmdFile      批量命令文件， 文件内格式为： orderCode|phoneno|imsi|ssInfo1|ssInfo2|ssInfo3, 本配置为null且portname != null, 则从控制台读取命令
     */
    public ConfigureManager(String fpath, String basePath, String portname, String cmdFile) {
        this.fpath = fpath;
        if(portname != null)
            log = LoggerUtil.getConsoleLogger();
        this.basePath = basePath;
        this.hand_port = portname;
        this.cmdFile = cmdFile;

        String pool_cfg = System.getProperty("HTTPPOOLCONF");
        if(!pool_cfg.startsWith("/"))
            pool_cfg = basePath + File.separator + pool_cfg;
        poolConf = new File(pool_cfg).getAbsolutePath();
        loadConf();
    }

    public BasePort getPort(String name) {
        loadConf(); //检查文件修改时间， 以便更新配置

        if(!ports.containsKey(name))
            return null;

        BasePort pc = ports.get(name);
        if(!pc.initOrderConf(log) )
            return null;
        return pc;
    }

    private void loadConf() {
        long tm = System.currentTimeMillis();
        if(tm - check_time > check_interval){
            loadPortConf();
            loadPoolConf();
            check_time = tm;
        }
    }

    private void loadPortConf() {
        File f = new File(fpath);
        if(f.lastModified() <= config_mod_time)
            return;
        Map m = null;
        try{
            m =(Map)new Yaml().load(new FileInputStream(f));
        }catch(Throwable e){
            log.error("order file load failed", e);
            return;
        }

        Map<String, BasePort> ports = new HashMap<String, BasePort>();
        for(Object k: m.keySet()) {
            Object v = m.get(k);
            if(v instanceof String){
                v = m.get(v);
            }
            if(v instanceof Map) {
                String portname = (String)k;
                if(this.hand_port != null && !portname.equals(this.hand_port)) // 手工执行， 只 load 指定的hlrport
                    continue;
                Map conf = (Map)v;
                if(conf.containsKey("copy_from")){
                    try{
                        Map cf = (Map)m.get(conf.get("copy_from"));
                        cf.putAll(conf);
                        conf = cf;
                    }catch(Exception e){}
                }
                BasePort pc;
                if(hand_port != null && cmdFile == null) {
                    pc = new ConsolePort(basePath, hand_port, conf, this);
                }else if(hand_port != null && cmdFile != null) {
                    pc = new FilePort(basePath, hand_port, conf, this, cmdFile);
                }else {
                    pc = new ManagerPort(basePath, portname, conf, this);
                }
                ports.put(portname, pc);
            }else{
                log.error("invalid config {}", k);
            }
        }
        this.ports = ports;

        config_mod_time = f.lastModified();
    }

    public synchronized OrderConfigure getOrderConf(String path) {
        OrderConfigure oc = orderConfs.get(path);
        if(oc == null){
            oc = new OrderConfigure(path, log);
            orderConfs.put(path, oc);
        }else{
            oc.loadConf(log);
        }
        return oc;
    }

    private void loadPoolConf() {

        File f = new File(poolConf);
        if(f.lastModified() <= poolConf_mod)
            return;
        Yaml y = new Yaml();
        Map m = null;
        try {
            m = (Map)y.load(new FileInputStream(f));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return;
        }
        for(Object k: m.keySet()) {
            String key = (String)k;
            if("_default_pool_conf_".equals(key)){
                _default_pool_conf_ = (Map)m.get(key);
                continue;
            }
            if(!key.startsWith("pool."))
                continue;
            Object o = m.get(key);
            if(o instanceof Map) {
                Map m1 = (Map)o;

                HttpExecutor e = new HttpExecutor(m1);

                executors.put(e.url, e);
            }
        }

        poolConf_mod = f.lastModified();
    }

    public synchronized HttpExecutor getExecutor(String url) {
        loadConf();
        if(executors.containsKey(url))
            return executors.get(url);
        if(_default_pool_conf_ != null){
            _default_pool_conf_.put("url", url);
            HttpExecutor e = new HttpExecutor(_default_pool_conf_);
            executors.put(url, e);
            return e;
        }
        return null;
    }

    public void close() {
        for(HttpExecutor ec: executors.values()){
            ec.shutdown();
        }
    }
    public String poolStatus() {
        StringBuilder sb = new StringBuilder();
        for(String k: executors.keySet()) {
            sb.append("==== ").append(k).append(" ====\n");
            sb.append(executors.get(k).status()).append('\n');
        }
        return sb.toString();
    }

}
