package com.sitech.crmpd.inter.client.jmx;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
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
	private String stateFile;
	
	public Control(String stateFile){
		this.stateFile = stateFile;
		//loadState();
	}
	
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
			saveState();
			return true;
		}
		return false;
	}
	@Override
	public String cm() {
		return HttpConnectionManager.cmStats();
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
		saveState();
		return true;
	}
	
	@Override
	public boolean restart(String hlrcode, String hlrport) {
		if(!stop(hlrcode, hlrport))
			return false;
		return start(hlrcode, hlrport);
	}
	
	//保存当前启动的状态， 以备重启进程的时候直接批量重启
	private void saveState() {
		PrintWriter out = null;
		try{
			out = new PrintWriter(new FileWriter(stateFile));
			for(Entry<String, HLRPort> e: ports.entrySet()){
				out.write(e.getKey());
				out.write('\n');
//				sb.append(e.getKey()).append(' ').append(e.getValue().isRunning()).append('\n');
			}
		}catch(IOException ex){
			
		}finally{
			if(out != null)
				out.close();
		}
	}
	
	/**
	 * 进程启动是重起之前保存的状态
	 */
	private void loadState() {
		BufferedReader bf = null;
		try{
			bf = new BufferedReader(new FileReader(stateFile));
			while(true){
				String line = bf.readLine();
				if(line == null)
					break;
				line = line.trim();
				int p = line.indexOf('.');
				if(p <= 0)
					continue;
				start(line.substring(0, p), line.substring(p+1));
			}
		}catch(IOException ex){
			
		}finally{
			if(bf != null)
				try {
					bf.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
		}
	}
	
	
	public static void main(String[] args) throws Exception {
        MBeanServer server = ManagementFactory.getPlatformMBeanServer();
        
        String stateFile = System.getenv("LOGDIR") + "/SOAP_PROCESS."+System.getProperty("SOAP_PROCESS");
        ObjectName helloName = new ObjectName("offon:name=PortControl");
        server.registerMBean(new Control(stateFile), helloName);
 
        System.out.println("start.....!");
        while(true)
        	Thread.sleep(1000000);
    }

}
