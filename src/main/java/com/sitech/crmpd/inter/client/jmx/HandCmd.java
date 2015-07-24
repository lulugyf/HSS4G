package com.sitech.crmpd.inter.client.jmx;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.text.MessageFormat;
import java.text.ParseException;
import java.util.Arrays;
import java.util.Map;
import java.util.Properties;
import java.util.Scanner;

import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.yaml.snakeyaml.Yaml;

import com.google.common.base.Splitter;
import com.google.common.base.Splitter.MapSplitter;
import com.sitech.crmpd.inter.client.ParamTran;
import com.sitech.crmpd.inter.common.CmdDataAck;
import com.sitech.crmpd.inter.common.CmdDataReq;

/**
 * @author heihuwudi@gmail.com</br> Created By: 2015年5月16日 下午4:34:15
 */
public class HandCmd {

	private static void usage(String info) {
		System.out.println(info);
		System.out.println("Usage: $0 hss");
		System.exit(0);
	}
	
	private static boolean readCfg(String etcdir, String hlrcode, String hlrport, Properties p){
		Yaml y = new Yaml();
		Map m = null;
		try {
			m = (Map)y.load(new FileInputStream(new File(etcdir, "hss.yaml")));
		} catch (FileNotFoundException e) {
			return false;
		}
		Map m1 = (Map)m.get(String.format("%s.%s", hlrcode, hlrport));
		if(m1 == null)
			return false;
//		Properties p = new Properties();
		for(Object k: m1.keySet()){
			Object v = m1.get(k);
			if(v == null)
				continue;
			p.setProperty(String.valueOf(k), String.valueOf(v));
		}
		return true;
	}

	/**
	 * @param args
	 *            -H${hlrcode} -N${hlrport}
	 * @throws Exception
	 */
	public static void main(String[] args) throws Exception {

		if (args.length < 1) {
			usage("invalid arguments");
			return;
		}

		String etcdir = ".";
		final Properties properties = new Properties();
		String[] hlr = args[0].split("\\.");
		if(!readCfg(etcdir, hlr[0], hlr[1], properties)){
			System.out.println("readCfg failed");
			return;
		}
//		properties.load(new FileInputStream(args[0]+".properties"));
		

		properties.setProperty("ETCDIR", etcdir);
		final HttpSoapCaller caller = new HttpSoapCaller(properties);
		
		final MessageFormat format = new MessageFormat("{0}|{1}|{2}|{3}|{4}|{5}");

		final Scanner scanner = new Scanner(System.in);
		String line = null;
		if (args.length > 1) {
			final BufferedReader bReader = new BufferedReader(new FileReader(""));
			/** name="{@link com.sitech.crmpd.inter.client.HandCmd}" */
			final Logger logger = LoggerFactory.getLogger(HandCmd.class);
			while ((line = bReader.readLine()) != null) {
				if (line.startsWith("#")) {
					continue;
				}
				line = line.trim();
				if (line.length() == 0) {
					continue;
				}
				System.out.println("输入请求参数 = [" + line + "]");
				logger.info("输入请求参数 = [{}]", line);
				try {
					final int code = call(caller, format, line);
					System.out.println("调用结果 = [" + code + "]");
					logger.info("调用结果 = [{}]", code);
					if (code != 0) {
						System.out.println("指令执行不正确，是否中断测试？输入Y/y中断，否则继续");
						line = scanner.nextLine();
						if (line.equalsIgnoreCase("y")) {
							break;
						}
					}
				} catch (final ParseException e) {
					System.out.println("参数个数必须为6");
					logger.warn("参数个数必须为6[{}]", e.getMessage());
				}
			}
			bReader.close();
		} else {
			while (true) {
				System.out.println("请输入请求参数");
				line = scanner.nextLine();
				System.out.println("输入请求参数 = [" + line + "]");
				if (line.equalsIgnoreCase("quit")) {
					break;
				}
				if (StringUtils.isBlank(line)) {
					continue;
				}
				try {
					System.out.println("调用结果 = [" + call(caller, format, line) + "]");
				} catch (final ParseException e) {
					System.out.println("参数个数必须为6");
				}
			}
			scanner.close();
		}
	}

	private static int call(HttpSoapCaller caller, MessageFormat format, String line)
			throws ParseException {
		final Object[] objects = format.parse(line.trim());
		final String[] params = new String[objects.length];
		for (int i = 0; i < params.length; i++) {
			params[i] = (String) objects[i];
		}
		CmdDataReq ret = new CmdDataReq();
		CmdDataAck cmd = new CmdDataAck();
		cmd.ordercode = params[0];
		cmd.phone_no = params[1];
		cmd.imsi_no = params[2];
		cmd.ss_info1 = params[3];
		cmd.ss_info2 = params[4];
		cmd.ss_info3 = params[5];
		return caller.apply(cmd, ret);
//		return caller.apply(params[0], params[1], params[2], params[3], params[4], params[5]);
	}
}
