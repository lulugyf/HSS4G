package com.sitech.crmpd.inter.offon.exec;

import com.sitech.crmpd.inter.offon.utils.LoggerUtil;
import com.sitech.crmpd.inter.offon.utils.OrderConfigure;
import org.slf4j.Logger;
import org.yaml.snakeyaml.Yaml;

import java.io.File;
import java.io.FileInputStream;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by guanyf on 7/31/2016.
 * 管理 hss.yaml 配置文件
 *
 * 配置文件更新生效的规则： 重启一个port的时候， 对应端口的配置会使用新的配置， 在配置更新以前启动的端口线程仍然使用旧的配置
 */
public class ConfigureManager {
    private Map<String, BasePort> ports;
    private Map<String, OrderConfigure> orderConfs = new HashMap<String, OrderConfigure>();
    private long config_mod_time = 0L;
    private String fpath;
    private Logger log;
    private String basePath;

    private String portname = null;
    private String cmdFile = null;

    /**
     * 创建一个ConfigureManager, 连接 offon_manager 运行的
     *
     * @param fpath       主配置文件路径
     * @param basePath    配置中的其它配置文件路径， 如果没有以 / 开始， 则以此路径为起始
     */
    public ConfigureManager(String fpath, String basePath) {
        this.fpath = fpath;
        log = LoggerUtil.getLogger(System.getenv("LOGDIR"), "log", "portMgr");
        this.basePath = basePath;
        loadConf();
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
        log = LoggerUtil.getConsoleLogger();
        this.basePath = basePath;
        this.portname = portname;
        this.cmdFile = cmdFile;
        loadConf();
    }

    public BasePort getPort(String name) {
        loadConf(); //检查文件修改时间， 以便更新配置

        if(!ports.containsKey(name))
            return null;

        BasePort pc = ports.get(name);
        pc.initOrderConf(log);
        return pc;
    }

    private void loadConf() {
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
                if(this.portname != null && !portname.equals(this.portname)) // 手工执行， 只 load 指定的hlrport
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
                if(portname != null && cmdFile == null) {
                    pc = new ConsolePort(basePath, portname, conf, this);
                }else if(portname != null && cmdFile != null) {
                    pc = new FilePort(basePath, portname, conf, this, cmdFile);
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


}
