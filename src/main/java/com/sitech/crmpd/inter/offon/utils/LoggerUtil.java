package com.sitech.crmpd.inter.offon.utils;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.Logger;
import ch.qos.logback.classic.LoggerContext;
import ch.qos.logback.classic.encoder.PatternLayoutEncoder;
import ch.qos.logback.core.rolling.RollingFileAppender;
import ch.qos.logback.core.rolling.TimeBasedRollingPolicy;
import ch.qos.logback.core.ConsoleAppender;
import org.slf4j.LoggerFactory;

public class LoggerUtil {
	public static Logger getLogger(String path, String hlrterm, String prefix){
        LoggerContext logCtx = (LoggerContext) LoggerFactory.getILoggerFactory();

        if(logCtx.exists(hlrterm) != null)
            return logCtx.getLogger(hlrterm);
//				if(logCtx.isStarted()){
//					logCtx.stop();
//				}

//			    PatternLayoutEncoder logEncoder = new PatternLayoutEncoder();
//			    logEncoder.setContext(logCtx);
//			    logEncoder.setPattern("%-12date{YYYY-MM-dd HH:mm:ss.SSS} %-5level - %msg%n");
//			    logEncoder.start();

//			    ConsoleAppender logConsoleAppender = new ConsoleAppender();
//			    logConsoleAppender.setContext(logCtx);
//			    logConsoleAppender.setName("console");
//			    logConsoleAppender.setEncoder(logEncoder);
//			    logConsoleAppender.start();

        PatternLayoutEncoder logEncoder = new PatternLayoutEncoder();
        logEncoder.setContext(logCtx);
        logEncoder.setPattern("%d{yyyy-MM-dd HH:mm:ss.SSS} %-5level - %msg%n");
        logEncoder.start();

        RollingFileAppender logFileAppender = new RollingFileAppender();
        logFileAppender.setContext(logCtx);
        logFileAppender.setName("logFile");
        logFileAppender.setEncoder(logEncoder);
        logFileAppender.setAppend(true);
        logFileAppender.setFile(path+"/"+prefix+"."+hlrterm);

        TimeBasedRollingPolicy logFilePolicy = new TimeBasedRollingPolicy();
        logFilePolicy.setContext(logCtx);
        logFilePolicy.setParent(logFileAppender);
        logFilePolicy.setFileNamePattern(path+"/"+prefix+".%d{yyyyMMdd}."+hlrterm);
        logFilePolicy.setMaxHistory(7);
        logFilePolicy.start();

        logFileAppender.setRollingPolicy(logFilePolicy);
        logFileAppender.start();

        Logger log = logCtx.getLogger(hlrterm);
        log.setAdditive(false);

        String level = System.getProperty("LOGLEVEL");
        if("INFO".equals(level) || level == null)
            log.setLevel(Level.INFO);
        else if("DEBUG".equals(level))
            log.setLevel(Level.DEBUG);
//			    log.addAppender(logConsoleAppender);
        log.addAppender(logFileAppender);
        return log;
	}

	public static Logger getConsoleLogger() {
		String logger_name = "console";
		LoggerContext logCtx = (LoggerContext) LoggerFactory.getILoggerFactory();
		Logger log = logCtx.exists(logger_name);
		if(log != null)
			return log;

		PatternLayoutEncoder logEncoder = new PatternLayoutEncoder();
		logEncoder.setContext(logCtx);
		logEncoder.setPattern("%d{yyyy-MM-dd HH:mm:ss.SSS} %-5level - %msg%n");
		logEncoder.start();

		ConsoleAppender logConsoleAppender = new ConsoleAppender();
		logConsoleAppender.setContext(logCtx);
		logConsoleAppender.setName("console");
		logConsoleAppender.setEncoder(logEncoder);
		logConsoleAppender.start();

		log = logCtx.getLogger("console");
		log.setAdditive(false);
        String level = System.getProperty("LOGLEVEL");
        if("INFO".equals(level) || level == null)
            log.setLevel(Level.INFO);
        else if("DEBUG".equals(level))
            log.setLevel(Level.DEBUG);
        log.addAppender(logConsoleAppender);
		return log;
	}

	public static Logger getLogger(String path, String hlrterm){
		return getLogger(path, hlrterm, "SOAP");
	}
	
	public static void main(String[] args){
		String dir = "f:/temp";
		Logger log1 = getLogger(dir, "hd5.1");
		Logger log2 = getLogger(dir, "hd5.2");
		
		log1.info("hello 1");
		log2.info("hello 2");
	}
}
