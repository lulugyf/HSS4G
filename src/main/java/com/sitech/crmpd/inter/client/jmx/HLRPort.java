package com.sitech.crmpd.inter.client.jmx;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.slf4j.MDC;
import org.yaml.snakeyaml.Yaml;

import com.google.common.base.Splitter;
import com.google.common.base.Splitter.MapSplitter;
import com.sitech.crmpd.inter.client.ParamTran;
import com.sitech.crmpd.inter.common.Constants;


/**
 * @author guanyf Created By: 2015年5月16日 下午4:36:08
 */
public class HLRPort extends Thread{
	public List<String> IP_HEX_INT;
	
	private String hlrcode;
	private String hlrport;
	private boolean running;
	private BaseComm com;
	private Logger logger;
	private boolean handmode = false;

	private static Object mutex = new Object();


	public HLRPort(String hlrcode, String hlrport, BaseComm com){
		this.hlrcode = hlrcode;
		this.hlrport = hlrport;
		handmode = true;
		this.com = com;
	}
	public HLRPort(String hlrcode, String hlrport){
		this.hlrcode = hlrcode;
		this.hlrport = hlrport;
		com = new ManagerComm();
	}
	
	public  boolean readCfg(String etcdir, String hlrcode, String hlrport, Properties p){
		Map<String, Properties> callers = new HashMap<String, Properties>();
		return readCfg(etcdir, hlrcode, hlrport, p, callers);
	}
	
	/**
	 * 读取yaml格式的hss配置文件
	 * @param conf  配置文件路径
	 * @param hlrcode
	 * @param hlrport
	 * @param p
	 * @return
	 */
	@SuppressWarnings({ "rawtypes", "unchecked" })
	public  boolean readCfg(String conf, String hlrcode, String hlrport, Properties p,
			Map<String, Properties> callers){
		Yaml y = new Yaml();
		Map m = null;
		try {
			m = (Map)y.load(new FileInputStream(new File(conf)));
		} catch (FileNotFoundException e) {
			logger.error("conf file {} not found", conf);
			return false;
		}
		Object o = m.get(String.format("%s.%s", hlrcode, hlrport));
		if(o == null) {
			logger.error("hlr port {}:{} config not found", hlrcode, hlrport);
			return false;
		}
		if(o instanceof String){
			//这里允许使用应用， 比如 hf1.2 的配置与 hf1.1 的相同， 则配置为：
			// hf1.2: hf1.1
			o = m.get(o);
			if(o == null) {
				logger.error("reference config {} not found", o);
				return false;
			}
		}
		if(!(o instanceof Map)){
			logger.error("can not load hlrport config {}.{}", hlrcode, hlrport);
			return false;
		}
		Map m1 = (Map)o;
		if(m1.containsKey("copy_from")){ //允许从其他配置中复制数据, 本配置项下的数据会覆盖被复制的数据， 这样可以只配置不同的项
			o = m.get(m1.get("copy_from"));
			if(o != null && (o instanceof Map )){
				Map m2 = m1;
				m1 = (Map)o;
				for(Object k: m2.keySet()){ //覆盖原有的项
					m1.put(k, m2.get(k));
				}
			}
		}
//		Properties p = new Properties();
		for(Object kk: m1.keySet()){
			if(!(kk instanceof String))
				continue;
			String k = (String)kk;
			Object v = m1.get(k);
			if(v == null)
				continue;
			if(v instanceof Map && k.startsWith("caller.") && callers != null){
				// 应该是 caller.$n 的配置了
				Properties p1 = new Properties();
				Map m3 = (Map)v;
				for(Object k1: m3.keySet()){
					Object v1 = m3.get(k1);
					if(v1 != null)
						p1.setProperty((String)k1, (String)v1);
				}
				callers.put(k, p1);
			}else{
				p.setProperty(String.valueOf(k), String.valueOf(v));
			}
		}
		
		p.setProperty(Constants.HLRCODE, hlrcode);
		p.setProperty(Constants.HLRPORT, hlrport);
		return true;
	}

	
	public boolean isRunning() {
		return running;
	}
	public void stopit(){
		if(com != null)
			com.stop();
	}
	
	/**
	 * 检查caller配置p1中的项， 如果没有设置， 则使用caller.0 的
	 * @param p
	 * @param p1
	 */
	private void checkCallerProperties(Properties p, Properties p1){
		  /*
		  order_reloadable: true
		  remote.url: http://10.109.230.132:8080/spg
		  remote.timeout: 60000
		  http.keep.test: false
		  http.keep.cfg: /offon/gyf/lte/test/http.keep.properties
		  order.path: /offon/gyf/lte/test/orders/hzx.yaml
		  start.tag: <ResultCode>
		  end.tag: </ResultCode>
		  APN_TPL_ID: 
		  IPHEX_TO_INT:
		  PROPERTIES: */

		for(Object k1: p.keySet()){
			if( !( k1 instanceof  String ))
				continue;
			String k = (String)k1;
			if(!p1.containsKey(k) && p.containsKey(k))
				p1.setProperty(k, p.getProperty(k));
		}
	}
	
	@SuppressWarnings("rawtypes")
	public void run() {
		String etcdir = null;
		if(!handmode){ //运行模式
			etcdir = System.getenv("ETCDIR");
			logger = LoggerUtil.getLogger(System.getenv("LOGDIR"), hlrcode+"."+hlrport);
		}else{ //手工测试模式
			etcdir = ".";
			logger = LoggerFactory.getLogger(HLRPort.class);
			System.setProperty("hlrname", String.format("%s.%s", hlrcode, hlrport));
		}
		String conf = System.getProperty("CONF", etcdir + "/hss.yaml");
		try{

			final Properties properties = new Properties();

			Map<String, Properties> callers = new HashMap<String, Properties>();
			if(!readCfg(conf, hlrcode, hlrport, properties, callers)){
				logger.error("readCfg failed");
				return;
			}
			properties.setProperty("ETCDIR", etcdir);
	
			HttpSoapCaller caller = null;
			try {
				caller = new HttpSoapCaller(properties, logger);
			} catch (Exception e) {
				logger.error("make SoapCaller failed", e);
				return;
			}
			

			com.setProps(properties);
			com.setCaller(caller);
			com.setLogger(logger);
			running = true;
			if(callers.size() > 0){
				// 添加多个 caller
				for(int i=1; i<callers.size()+1; i++){
					String k = "caller."+i;
					if(!callers.containsKey(k))
						continue;
					Properties p1 = callers.get(k);
					checkCallerProperties(properties, p1);
					try {
						HttpSoapCaller c = new HttpSoapCaller(p1, logger);
						com.addCaller(c);
					} catch (Exception e) {
						logger.error("can not inialize caller: {}", k, e);
					}
				}
			}

			com.for_ever(); // 进入主循环
		}finally{
			if(!handmode){
//				MDC.remove("hlrterm");
			}
			running = false;
		}
	}
}
