package com.sitech.crmpd.inter.common;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.beetl.core.Configuration;
import org.beetl.core.GroupTemplate;
import org.beetl.core.Template;
import org.beetl.core.resource.StringTemplateResourceLoader;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.slf4j.MDC;
import org.yaml.snakeyaml.Yaml;

import ch.qos.logback.classic.LoggerContext;
import ch.qos.logback.classic.encoder.PatternLayoutEncoder;
import ch.qos.logback.classic.util.ContextInitializer;
import ch.qos.logback.core.joran.spi.JoranException;
import ch.qos.logback.core.rolling.RollingFileAppender;
import ch.qos.logback.core.rolling.TimeBasedFileNamingAndTriggeringPolicy;
import ch.qos.logback.core.rolling.TimeBasedFileNamingAndTriggeringPolicyBase;
import ch.qos.logback.core.rolling.TimeBasedRollingPolicy;




public class LogUtil extends Thread{
	private static final Object mutex = new Object();
	

	private String hlrcode;
	private String hlrport;
	
	public LogUtil(String h, String p){
		hlrcode = h;
		hlrport = p;
	}
	public LogUtil() {}
	
//	public static Logger getLogger(String hlrcode, String hlrport) {
//		LoggerContext loggerContext = (LoggerContext) LoggerFactory.getILoggerFactory();
//
//		RollingFileAppender fileAppender = new RollingFileAppender();
//	    fileAppender.setContext(loggerContext);
//	    
//	    TimeBasedRollingPolicy rollingp = new TimeBasedRollingPolicy();
//	    String fname = String.format("%s/SOAP.%%d{yyyyMMdd}.%s.%s",
//	    		System.getProperty("LOGDIR"), hlrcode, hlrport);
////	    rollingp.setParent(fileAppender);
//	    TimeBasedFileNamingAndTriggeringPolicy tp =  TimeBasedFileNamingAndTriggeringPolicy;
//	    rollingp.setTimeBasedFileNamingAndTriggeringPolicy(timeBasedTriggering);
//	    rollingp.setFileNamePattern(fname);
//	    rollingp.start();
//	    fileAppender.setRollingPolicy(rollingp);
////	    fileAppender.setName("logger");
//	    // set the file name
////	    fileAppender.setFile(fname);
//
//	    PatternLayoutEncoder encoder = new PatternLayoutEncoder();
//	    encoder.setContext(loggerContext);
//	    encoder.setPattern("[%5p]%d{HH:mm:ss.SSS} -- %m%n");
//	    encoder.start();
//
//	    fileAppender.setEncoder(encoder);
//	    fileAppender.start();
//
//	    // attach the rolling file appender to the logger of your choice
//	    Logger logbackLogger = loggerContext.getLogger(Communication.class);
//	    ((ch.qos.logback.classic.Logger) logbackLogger).addAppender(fileAppender);
//
//	    return logbackLogger;
//	}
	
	
	private Logger getLogger() {
		Logger l = null;
		synchronized(mutex){
			System.setProperty("hlrcode", hlrcode);
			System.setProperty("hlrport", hlrport);
			LoggerContext loggerContext = (LoggerContext) LoggerFactory.getILoggerFactory();
			loggerContext.reset();
			ContextInitializer ci = new ContextInitializer(loggerContext);
			try {
				ci.autoConfig();
				l = loggerContext.getLogger(LogUtil.class);
			} catch (JoranException e) {
				e.printStackTrace();
			}
		}
		return l;
		
	}
	
	public static Logger getLog(String hlrcode, String hlrport){
		return null;
	}
	
	

	@Override
	public void run() {
//		log = getLogger();
		//MDC.put("LOGDIR", "f:/temp/log/");
		Logger log = null;
		synchronized(mutex){
			MDC.put("hlrterm", String.format("%s.%s", hlrcode, hlrport));
			log = LoggerFactory.getLogger("com.sitech.crmpd.inter.common."+hlrcode);
		}
		for(int i=0; i<200; i++){

			log.debug("hello {} {}",  i, hlrcode);

			try {
				Thread.sleep(20);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		MDC.remove("hlrterm");
	}
	
	public static void test2() throws Exception{
		System.setProperty("LOGDIR", "f:/temp/log/");
		String h = "abcdefghijklmnop";
		for(int i=0; i<h.length(); i++){
			new LogUtil("ha"+h.charAt(i), String.valueOf(i)).start();
		}
//		Logger l = getLogger("haz", "2");
//		l.debug("hello");
//		l.error("hello", new Exception("failed"));
		
	}
	
	public static void test1() throws Exception{
		Yaml y = new Yaml();
		Map m = (Map)y.load(new FileInputStream("orders/order_hss_hev.yaml"));
//		System.out.println(m.get("OP1212"));
//		System.out.println(m.get("header"));
//		System.out.println(m.get("footer"));
		
		String header = (String)m.get("header");
		String footer = (String)m.get("footer");
		Map<String, Template> m1 = new HashMap<String, Template>();
		
		
		StringTemplateResourceLoader resourceLoader = new StringTemplateResourceLoader();
		Configuration cfg = Configuration.defaultConfiguration();
		GroupTemplate gt = new GroupTemplate(resourceLoader, cfg);
		
		for(Object o: m.keySet()){
			String key = (String)o;
			if(!key.startsWith("OP"))
				continue;
			String value = (String)m.get(key);
			key = key.substring(2);
			Template t = gt.getTemplate(header+value+footer);
			m1.put(key, t);
		}
		
		Template template = m1.get("8607");
		template.binding("HLRSN", "11");
		template.binding("phoneNo", "13508210041");
		template.binding("imsi", "460078823820000");
		template.binding("ssInfo1", "7A680F3FC7A18D");
		template.binding("ssInfo2", "1CAFA39614670744CC");
		template.binding("ssInfo3", "B4D7C9631FE40FB4FA06E34AF9D493C4");
		String str = template.render();
		System.out.println(str);
	}
	
	public static void test3() {
		String line = "<Group><CNTXID>4</CNTXID><QOSTPLID>901</QOSTPLID><IPV4ADD>STATIC</IPV4ADD></Group><Group><CNTXID>5</CNTXID><QOSTPLID>119</QOSTPLID><IPV4ADD>STATIC</IPV4ADD></Group></Group>";
		Pattern pattern = Pattern.compile("<Group><CNTXID>(\\d+)</CNTXID><QOSTPLID>119</QOSTPLID>");
		Matcher matcher = pattern.matcher(line);
		while (matcher.find()) {
		    System.out.println("group 1: " + matcher.group(1));
		}
	}
	
	private void loadQueryParam(String path){
		File f = new File(path);
		if(!f.exists()){
			System.out.println("query_param file: "+path);
			return;
		}
		Yaml y = new Yaml();
		Map m = null;
		try {
			m = (Map)y.load(new FileInputStream(path));
		} catch (FileNotFoundException e) {
//			LOGGER.error("load query param file failed:"+path, e);
			return;
		}
		for(Object o: m.keySet()){
			String key = (String)o;
			if(!key.startsWith("QP"))
				continue;
			Object value = m.get(key);
			key = key.substring(2);
			if(value instanceof String){
				String s = (String)value;
				int p = s.indexOf('=');
			}else if(value instanceof List){
				for(String s: (List<String>)value){
					int p = s.indexOf('=');
					System.out.println(s.substring(0,p));
					System.out.println(s.substring(p+1));
				}
			}
			
		}
	}
	public static void test4() {
		new LogUtil().loadQueryParam("etc/query_param_hf1.yaml");
	}
	
	public static void test5() {
		Yaml y = new Yaml();
		Map m = null;
		try {
			m = (Map)y.load(new FileInputStream("etc/hss.yaml"));
		} catch (FileNotFoundException e) {
			return;
		}
		Map<String, String> m1 = (Map<String, String>)m.get("hf1.1");
		Properties p = new Properties();
		for(Object k: m1.keySet()){
			Object v = m1.get(k);
			if(v == null)
				continue;
			p.setProperty(String.valueOf(k), String.valueOf(v));
		}
		//m1.putAll(p);
//		p.putAll(m1);
		System.out.println(p.get("REMOTE_URL"));
		System.out.println(p.containsKey("IPHEX_TO_INT"));
	}

	
	public static void main(String[] args) throws Exception{
		String[] s = "hf1.1".split("\\.");
		System.out.println(s[1]);
		
//		test5();
	}

}
