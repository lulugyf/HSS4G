package com.sitech.crmpd.inter.client.jmx;

import java.lang.management.ManagementFactory;
import java.util.HashMap;
import java.util.Map.Entry;

import javax.management.MBeanServer;
import javax.management.ObjectName;

/**
 * 
java -javaagent:agent.jar=port=7777,host=localhost,agentContext=/j4p -cp target/offon-soap-0.0.1-SNAPSHOT-jar-with-dependencies.jar com.sitech.crmpd.inter.client.jmx.Control

curl http://127.0.0.1:7777/j4p/list

curl http://127.0.0.1:7777/j4p/exec/offon:name=PortControl/list

curl http://127.0.0.1:7777/j4p/exec/offon:name=PortControl/start/haa/2

curl http://127.0.0.1:7777/j4p/exec/offon:name=PortControl/stop/haa/2

 * @author guanyf
 *
 */

public class Control implements ControlMBean{
	
	private HashMap<String, HLRPort> ports = new HashMap<String, HLRPort>();

	@Override
	public String list() {
		StringBuffer sb = new StringBuffer();
		sb.append("=============\n");
		for(Entry<String, HLRPort> e: ports.entrySet()){
			sb.append(e.getKey()).append(' ').append(e.getValue().isRunning()).append('\n');
		}
		sb.append("-------------\n");
		return sb.toString();
	}

	@Override
	public boolean stop(String hlrcode, String hlrport) {
		String s = String.format("%s.%s", hlrcode, hlrport);
		if(ports.containsKey(s)){
			HLRPort p = ports.remove(s);
			p.stopit();
			return true;
		}
		return false;
	}

	@Override
	public boolean start(String hlrcode, String hlrport) {
		String s = String.format("%s.%s", hlrcode, hlrport);
		if(ports.containsKey(s)){
			HLRPort p = ports.get(s);
			if(p.isAlive()) //已经存在且是活动的，则返回
				return false;
			else
				ports.remove(s); //否则删除， 重启启动一个
		}
		HLRPort p = new HLRPort(hlrcode, hlrport);
		ports.put(s, p);
		p.start();
		return true;
	}
	
	
	
	public static void main(String[] args) throws Exception {   
        MBeanServer server = ManagementFactory.getPlatformMBeanServer();
        
        ObjectName helloName = new ObjectName("offon:name=PortControl");
        server.registerMBean(new Control(), helloName);
 
        System.out.println("start.....!");
        while(true)
        	Thread.sleep(1000000);
    }

}
