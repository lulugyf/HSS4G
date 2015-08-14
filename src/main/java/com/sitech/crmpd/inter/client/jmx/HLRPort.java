package com.sitech.crmpd.inter.client.jmx;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;
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
	private ManagerComm com;
	private Logger logger;

	private static Object mutex = new Object();


	public HLRPort(String hlrcode, String hlrport){
		this.hlrcode = hlrcode;
		this.hlrport = hlrport;
	}
	
	public static boolean readCfg(String etcdir, String hlrcode, String hlrport, Properties p){
		Yaml y = new Yaml();
		Map m = null;
		try {
			m = (Map)y.load(new FileInputStream(new File(etcdir, "hss.yaml")));
		} catch (FileNotFoundException e) {
			return false;
		}
		Object o = m.get(String.format("%s.%s", hlrcode, hlrport));
		if(o == null)
			return false;
		if(o instanceof String){
			//这里允许使用应用， 比如 hf1.2 的配置与 hf1.1 的相同， 则配置为：
			// hf1.2: hf1.1
			o = m.get(o);
			if(o == null)
				return false;
		}
		if(!(o instanceof Map)){
//			logger.error("can not load hlrport config {}.{}", hlrcode, hlrport);
			return false;
		}
		Map m1 = (Map)o;
//		Properties p = new Properties();
		for(Object k: m1.keySet()){
			Object v = m1.get(k);
			if(v == null)
				continue;
			p.setProperty(String.valueOf(k), String.valueOf(v));
		}
		
		p.setProperty(Constants.HLRCODE, hlrcode);
		p.setProperty(Constants.HLRPORT, hlrport);
		return true;
	}

	private boolean readCfg(String etcdir, String hlrcode,
			String hlrport, Properties p, String fileName)
					throws java.io.IOException {
		int cfgReadFlag = 0;
		boolean ret = false;
		final BufferedReader bf = new BufferedReader(new FileReader(new File(etcdir,
				fileName)));
		String line = null;
		while ((line = bf.readLine()) != null) {
			final String[] x = line.split("\\|");
			if (x.length < 14) {
				cfgReadFlag = 0;
				continue;
			}
			if (!(hlrcode.equals(x[0]) && hlrport.equals(x[1]))) {
				cfgReadFlag = 0;
				continue;
			}
			cfgReadFlag = 1;

			p.setProperty(Constants.HLRCODE, x[0]);
			p.setProperty(Constants.HLRPORT, x[1]);
			p.setProperty(Constants.MANAGER_IP, x[2]);
			p.setProperty(Constants.MANAGER_PORT, x[3]);
			p.setProperty(Constants.REMOTE_URL, x[4]);
			p.setProperty(Constants.REMOTE_TIMEOUT, x[5]);
			p.setProperty(Constants.HTTP_KEEP_TEST, x[6]);
			p.setProperty(Constants.HTTP_KEEP_CFG, x[7]);
			p.setProperty(Constants.ORDER_PATH, x[8]);
			p.setProperty(Constants.START_TAG_KEY, x[9]);
			p.setProperty(Constants.END_TAG_KEY, x[10]);
			p.setProperty(Constants.MANAGER_USER, x[11]);
			p.setProperty(Constants.MANAGER_PASSWORD, x[12]);
			p.setProperty(Constants.TERM_TYPE, x[13]);
//			System.setProperty(Constants.HLRCODE, x[0]);
//			System.setProperty(Constants.HLRPORT, x[1]);
//			System.setProperty(Constants.MANAGER_IP, x[2]);
//			System.setProperty(Constants.MANAGER_PORT, x[3]);
//			System.setProperty(Constants.REMOTE_URL, x[4]);
//			System.setProperty(Constants.REMOTE_TIMEOUT, x[5]);
//			System.setProperty(Constants.HTTP_KEEP_TEST, x[6]);
//			System.setProperty(Constants.HTTP_KEEP_CFG, x[7]);
//			System.setProperty(Constants.ORDER_PATH, x[8]);
//			System.setProperty(Constants.START_TAG_KEY, x[9]);
//			System.setProperty(Constants.END_TAG_KEY, x[10]);
//			System.setProperty(Constants.MANAGER_USER, x[11]);
//			System.setProperty(Constants.MANAGER_PASSWORD, x[12]);
//			System.setProperty(Constants.TERM_TYPE, x[13]);
			
			if (x.length > 14) { // APN指令模版号和域名转换配置文件目录参数 $ETCDIR/ 下的文件名
				if (x[14] == null || x[14].length() == 0 || x[14].equals("@")) {} else {
					p.setProperty("__paramTran", x[14]);
				}
			}

			if (x.length > 15) { // APN指令静态IP地址16进制转为10进制（目前只有诺西需要转换）
				/*
				 * 配置格式 如有需要转换的指令 : |3781~3241~3251| 如没有需要转换的指令： |@|
				 */
				final String strarr[] = x[15].split("~");
				IP_HEX_INT = Arrays.asList(strarr);

			} else {
				p.setProperty("IPHEX_TO_INT", "@");
			}

			if (x.length > 16) {// HLRSN编码配置： 在hss.cfg文件配置值
				final MapSplitter splitter = Splitter.on(',').omitEmptyStrings().trimResults()
						.withKeyValueSeparator('=');
				p.putAll(splitter.split(x[16]));
			}
			ret = true;
			break;
		}
		bf.close();
		if (cfgReadFlag == 0) {
			logger.error("未读取到对应启动配置：HLRCODE=" + hlrcode + ", HLRPORT=" + hlrport + ",结束启动");
			ret = false;
		}
//		System.out.println(p);
		return ret;
	}
	
	public boolean isRunning() {
		return running;
	}
	public void stopit(){
		if(com != null)
			com.stop();
	}
	
	public void run() {
		synchronized(mutex){
			MDC.put("hlrterm", String.format("%s.%s", hlrcode, hlrport));
			logger = LoggerFactory.getLogger("com.sitech.crmpd.inter.common."+hlrcode);
		}
		try{

			final String etcdir = System.getenv("ETCDIR");
			final Properties properties = new Properties();
//			try {
//				boolean r = readCfg(etcdir, hlrcode, hlrport, properties,
//						System.getProperty("configuration.location", "hss.cfg"));
//				if(!r){
//					return;
//				}
//			} catch (IOException e) {
//				e.printStackTrace();
//				return;
//			}
			if(!readCfg(etcdir, hlrcode, hlrport, properties)){
				logger.error("readCfg failed");
				return;
			}
			properties.setProperty("ETCDIR", etcdir);
			
	
	
			HttpSoapCaller caller = null;
			try {
				caller = new HttpSoapCaller(properties, logger);
			} catch (IOException e) {
				logger.error("make SoapCaller failed", e);
				return;
			}
	
			// 建立到 offon_manager 的连接
			com = new ManagerComm(properties, caller, logger);
			running = true;
			com.start(); // 进入主循环
		}finally{
			MDC.remove("hlrterm");
		}
	}
}
