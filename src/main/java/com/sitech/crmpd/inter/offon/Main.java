package com.sitech.crmpd.inter.offon;

import com.sitech.crmpd.inter.offon.exec.BasePort;
import com.sitech.crmpd.inter.offon.exec.ConfigureManager;
import com.sitech.crmpd.inter.offon.utils.LoggerUtil;
import org.slf4j.Logger;

import javax.management.MBeanServer;
import javax.management.ObjectName;
import java.io.File;
import java.lang.management.ManagementFactory;

/**
 * Created by guanyf on 7/31/2016.
 */
public class Main {

    public static void main(String[] args) throws Exception{
        ConfigureManager cm = null;
        Logger log = null;
        String confFile = System.getProperty("CONF");
        if(confFile == null){
            System.out.printf("Please set -DCONF=? to main config file\n");
            return;
        }
        File fpath = new File(confFile);
        if(!fpath.exists()){
            System.out.printf("port conf file[CONF] %s not found!\n", fpath.getAbsolutePath());
            return;
        }

        String poolcfg = System.getProperty("HTTPPOOLCONF");
        if(poolcfg == null){
            System.out.printf("Please set -DHTTPPOOLCONF=? to pool config file\n");
            return;
        }
        File _f = new File(poolcfg);
        if(!_f.exists()){
            System.out.printf("pool conf file[HTTPPOOLCONF] %s not found!\n", _f.getAbsolutePath());
            return;
        }

        String basePath = fpath.getParent();
        if(args.length == 0) {
            cm = new ConfigureManager(fpath.getAbsolutePath(), basePath);
            new Main(cm, basePath).startControl();

        }else if(args.length == 1) {
            String hlrport = args[0];
            cm = new ConfigureManager(fpath.getAbsolutePath(), basePath, hlrport, null);
            BasePort port = cm.getPort(hlrport);
            port.for_ever(LoggerUtil.getConsoleLogger());
        }else if(args.length == 2) {
            String hlrport = args[0];
            String cmdfile = args[1];
            cm = new ConfigureManager(fpath.getAbsolutePath(), basePath, hlrport, cmdfile);
            BasePort port = cm.getPort(hlrport);
            port.for_ever(LoggerUtil.getConsoleLogger());
        }
    }
    private ConfigureManager cm;
    private String basePath;
    public Main(ConfigureManager cm, String basePath) {
        this.basePath = basePath;
        this.cm = cm;
    }

    private void startControl() throws Exception {
        MBeanServer server = ManagementFactory.getPlatformMBeanServer();

        ObjectName helloName = new ObjectName("offon:name=PortControl");
        server.registerMBean(new JMXControl(cm, basePath), helloName);

        System.out.println("start.....!");
        while(true)
            Thread.sleep(1000000);
    }
}
