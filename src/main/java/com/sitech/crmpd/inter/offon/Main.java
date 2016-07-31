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
        System.out.println("Hello world");

        ConfigureManager cm = null;
        Logger log = null;
        String confFile = System.getProperty("CONF");
        File fpath = new File(confFile);
        if(!fpath.exists()){
            System.out.printf("conf file %s not found!\n", fpath.getAbsolutePath());
            return;
        }
        String basePath = fpath.getParent();
        if(args.length == 0) {
            cm = new ConfigureManager(fpath.getAbsolutePath(), basePath);
            new Main(cm).startControl();

        }else if(args.length == 1) {
            String hlrport = args[0];
            cm = new ConfigureManager(fpath.getAbsolutePath(), basePath, hlrport, null);
            BasePort port = cm.getPort(hlrport);
            port.for_ever(1, LoggerUtil.getConsoleLogger());
        }else if(args.length == 2) {
            String hlrport = args[0];
            String cmdfile = args[1];
            cm = new ConfigureManager(fpath.getAbsolutePath(), basePath, hlrport, cmdfile);
            BasePort port = cm.getPort(hlrport);
            port.for_ever(1, LoggerUtil.getConsoleLogger());
        }
    }
    private ConfigureManager cm;
    public Main(ConfigureManager cm) {
        this.cm = cm;
    }

    private void startControl() throws Exception {
        MBeanServer server = ManagementFactory.getPlatformMBeanServer();

        ObjectName helloName = new ObjectName("offon:name=PortControl");
        server.registerMBean(new JMXControl(cm), helloName);

        System.out.println("start.....!");
        while(true)
            Thread.sleep(1000000);
    }
}
