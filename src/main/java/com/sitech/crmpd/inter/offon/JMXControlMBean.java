package com.sitech.crmpd.inter.offon;

import com.sitech.crmpd.inter.offon.exec.BasePort;
import com.sitech.crmpd.inter.offon.exec.ConfigureManager;
import com.sitech.crmpd.inter.offon.utils.LoggerUtil;
import org.slf4j.Logger;

import java.util.HashMap;
import java.util.Map;

public interface JMXControlMBean {
	public String list();
	public String stop(String hlrcode, String hlrport);
	public String start(String hlrcode, String hlrport);
	public String restart(String hlrcode, String hlrport);
	public String cm();
}

class JMXControl implements JMXControlMBean {
	private ConfigureManager cm = null;

	private HashMap<String, BasePort> ports = new HashMap<String, BasePort>();
	private String basePath;

	public JMXControl(ConfigureManager cm, String basePath) {
		this.cm = cm;
		this.basePath = basePath;
	}
	@Override
	public String list() {
		StringBuffer sb = new StringBuffer();
		sb.append("=============\n");
		for(Map.Entry<String, BasePort> e: ports.entrySet()){
			sb.append(e.getKey()).append(' ').append(e.getValue().isRunning()).append('\n');
		}
		sb.append("-------------\n");
		return sb.toString();
	}

	@Override
	public String stop(String hlrcode, String hlrport) {
		String portname = hlrcode + "." + hlrport;
		BasePort port = null;
		String ret = "Worker not running.";
		if(ports.containsKey(portname)) {
			port = ports.get(portname);
			if (port.isRunning()) {
				ret = "Worker stopped";
				port.stop();
			}
		}
		return ret;
	}

	static class Worker extends Thread{
		private BasePort port;
		private Logger log;
		Worker(BasePort port, Logger log){
			this.port = port;
			this.log = log;
		}
		public void run() {
			port.for_ever(log);
		}
	}

	@Override
	public String start(String hlrcode, String hlrport) {
		String portname = hlrcode + "." + hlrport;
		BasePort port = null;
		String ret;
		if(ports.containsKey(portname)) {
			port = ports.get(portname);
			if(port.isRunning()) {
				return "Already running!";
			}

			ret = "Worker stopped, restart it.";
		}else{
			ret = "Start Worker finished.";
		}
		port = cm.getPort(portname);
		if(port == null){
			return "Config for "+portname + " not found!";
		}
		ports.put(portname, port);
		Logger log = LoggerUtil.getLogger(basePath, portname, "SOAP");
		Worker worker = new Worker(port, log);
		worker.setDaemon(true);
		worker.start();
		return ret;
	}

	@Override
	public String restart(String hlrcode, String hlrport) {
		stop(hlrcode, hlrport);
		return start(hlrcode, hlrport);
	}

	@Override
	public String cm() {
		return null;
	}
}
