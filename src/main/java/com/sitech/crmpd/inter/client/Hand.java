package com.sitech.crmpd.inter.client;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.PrintWriter;
import java.text.MessageFormat;
import java.text.ParseException;
import java.util.Arrays;
import java.util.List;
import java.util.Properties;
import java.util.Scanner;

import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.sitech.crmpd.inter.common.CmdDataAck;

/**
 * @author heihuwudi@gmail.com</br> Created By: 2015年5月16日 下午4:34:15
 */
public class Hand {
	public static String HLRSN_v;
	public static List<String> IP_HEX_INT;

	private static void usage(String info) {
		System.out.println(info);
		System.out.println("Usage: $0 hss.properties");
		System.exit(0);
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

		final Properties properties = new Properties();
		properties.load(new FileInputStream(args[0] + ".properties"));

		final SoapCaller caller = new SoapCaller(properties);
		if (properties.containsKey("__paramTran")) {
			final String etcdir = System.getenv("ETCDIR");
			caller.setTran(new ParamTran(etcdir, properties.getProperty("__paramTran")));
		}
		IP_HEX_INT = Arrays.asList(properties.getProperty("IPHEX_TO_INT", "@").split("~"));
		if (IP_HEX_INT != null && IP_HEX_INT.size() > 0) {
			caller.setIPox(FSM.IP_HEX_INT);
		}

		final MessageFormat format = new MessageFormat("{0}|{1}|{2}|{3}|{4}|{5}");

		final Scanner scanner = new Scanner(System.in);
		String line = null;
		if (args.length > 1) {
			final boolean pause = args.length > 2 ? Boolean.parseBoolean(args[2]) : false;
			final BufferedReader bReader = new BufferedReader(new FileReader(args[1]));
			final PrintWriter writer = new PrintWriter(args[1] + ".result", "utf-8");
			/** name="{@link com.sitech.crmpd.inter.client.Hand}" */
			final Logger logger = LoggerFactory.getLogger(Hand.class);
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
				writer.print(line);
				writer.print("\t");
				try {
					final int code = call(caller, format, line);
					System.out.println("调用结果 = [" + code + "]");
					logger.info("调用结果 = [{}]", code);
					writer.println(code);
					writer.flush();
					if (code != 0) {
						System.out.println("指令执行不正确，是否中断测试？输入Y/y中断，否则继续");
						if (pause) {
							line = scanner.nextLine();
							if (line.equalsIgnoreCase("y")) {
								break;
							}
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

	private static int call(SoapCaller caller, MessageFormat format, String line)
			throws ParseException {
		final Object[] objects = format.parse(line.trim());
		final String[] params = new String[objects.length];
		for (int i = 0; i < params.length; i++) {
			params[i] = (String) objects[i];
		}
		final CmdDataAck cmd = new CmdDataAck();
		cmd.ordercode = params[0];
		cmd.phone_no = params[1];
		cmd.imsi_no = params[2];
		cmd.ss_info1 = params[3];
		cmd.ss_info2 = params[4];
		cmd.ss_info3 = params[5];
		return caller.apply(cmd);
		// return caller.apply(params[0], params[1], params[2], params[3], params[4], params[5]);
	}
}
