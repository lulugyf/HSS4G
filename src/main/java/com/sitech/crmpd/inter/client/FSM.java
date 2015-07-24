package com.sitech.crmpd.inter.client;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.Arrays;
import java.util.List;
import java.util.Properties;

import com.google.common.base.Splitter;
import com.google.common.base.Splitter.MapSplitter;
import com.sitech.crmpd.inter.common.Communication;
import com.sitech.crmpd.inter.common.Constants;

/**
 * @author heihuwudi@gmail.com</br> Created By: 2015年5月16日 下午4:36:08
 */
public class FSM {
	public static String HLRSN_v;
	public static List<String> IP_HEX_INT;

	private static void usage(String info) {
		System.out.println(info);
		System.out.println("Usage: $0 -H${hlrcode} -N${hlrport}");
		System.exit(0);
	}

	private static boolean readCfg(String etcdir, String hlrcode, String hlrport, Properties p,
			String fileName) throws java.io.IOException {
		int cfgReadFlag = 0;
		boolean ret = false;
		final BufferedReader bf = new BufferedReader(new FileReader(new File(etcdir, fileName)));
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
			System.setProperty(Constants.HLRCODE, x[0]);
			System.setProperty(Constants.HLRPORT, x[1]);
			System.setProperty(Constants.MANAGER_IP, x[2]);
			System.setProperty(Constants.MANAGER_PORT, x[3]);
			System.setProperty(Constants.REMOTE_URL, x[4]);
			System.setProperty(Constants.REMOTE_TIMEOUT, x[5]);
			System.setProperty(Constants.HTTP_KEEP_TEST, x[6]);
			System.setProperty(Constants.HTTP_KEEP_CFG, x[7]);
			System.setProperty(Constants.ORDER_PATH, x[8]);
			System.setProperty(Constants.START_TAG_KEY, x[9]);
			System.setProperty(Constants.END_TAG_KEY, x[10]);
			System.setProperty(Constants.MANAGER_USER, x[11]);
			System.setProperty(Constants.MANAGER_PASSWORD, x[12]);
			System.setProperty(Constants.TERM_TYPE, x[13]);

			if (x.length >= 14) { // APN指令模版号和域名转换配置文件目录参数 $ETCDIR/ 下的文件名
				if (x[14] == null || x[14].length() == 0 || x[14].equals("@")) {} else {
					p.setProperty("__paramTran", x[14]);
				}
			}

			if (x.length >= 15) { // APN指令静态IP地址16进制转为10进制（目前只有诺西需要转换）
				/*
				 * 配置格式 如有需要转换的指令 : |3781~3241~3251| 如没有需要转换的指令： |@|
				 */
				final String strarr[] = x[15].split("~");
				IP_HEX_INT = Arrays.asList(strarr);

			} else {
				p.setProperty("IPHEX_TO_INT", "@");
			}

			if (x.length >= 16) {// HLRSN编码配置： 在hss.cfg文件配置值
				final MapSplitter splitter = Splitter.on(',').omitEmptyStrings().trimResults()
						.withKeyValueSeparator('=');
				p.putAll(splitter.split(x[16]));
			}

			ret = true;
			break;
		}
		bf.close();
		if (cfgReadFlag == 0) {
			System.out.println("未读取到对应启动配置：HLRCODE=" + hlrcode + ", HLRPORT=" + hlrport + ",结束启动");
			System.exit(0);
		}
		System.out.println(p);

		return ret;
	}

	/**
	 * @param args
	 *            -H${hlrcode} -N${hlrport}
	 * @throws Exception
	 */
	public static void main(String[] args) throws Exception {
		if (args.length < 2) {
			usage("invalid arguments");
			return;
		}

		String hlrcode = null;
		String hlrport = null;
		for (int i = 0; i < args.length; i++) {
			final String s = args[i];
			if (s.charAt(0) != '-') {
				continue;
			}
			switch (s.charAt(1)) {
			case 'H':
				hlrcode = s.substring(2);
				break;
			case 'N':
				hlrport = s.substring(2);
				break;
			default:
				usage("unknown argument:" + s);
				break;
			}
		}
		if (hlrcode == null || hlrport == null) {
			usage("must give hlrcode and hlrport");
		}

		System.setProperty(Constants.HLRCODE, hlrcode);
		System.setProperty(Constants.HLRPORT, hlrport);

		System.out.println("开始启动进程:HLRCODE=" + hlrcode + ", HLRPORT=" + hlrport + "...>>>...");

		final String etcdir = System.getenv("ETCDIR");
		final Properties properties = new Properties();
		readCfg(etcdir, hlrcode, hlrport, properties,
				System.getProperty("configuration.location", "hss.cfg"));

		final SoapCaller caller = new SoapCaller(properties);
		if (properties.containsKey("__paramTran")) {
			caller.setTran(new ParamTran(etcdir, properties.getProperty("__paramTran")));
		}
		if (IP_HEX_INT != null && IP_HEX_INT.size() > 0) {
			caller.setIPox(FSM.IP_HEX_INT);
		}

		// 建立到 offon_manager 的连接
		final Communication com = new Communication(properties, caller);
		com.start(); // 进入主循环

		System.out.println("进程启动完毕:HLRCODE=" + hlrcode + ", HLRPORT=" + hlrport + " * | 启动成功");
	}
}
