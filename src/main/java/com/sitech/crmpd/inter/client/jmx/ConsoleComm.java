package com.sitech.crmpd.inter.client.jmx;

import java.text.MessageFormat;
import java.text.ParseException;
import java.util.Properties;
import java.util.Scanner;

import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.sitech.crmpd.inter.common.CmdDataAck;
import com.sitech.crmpd.inter.common.CmdDataReq;

public class ConsoleComm extends BaseComm {

	private Scanner scanner;
	private final MessageFormat format = new MessageFormat("{0}|{1}|{2}|{3}|{4}|{5}");
	
	@Override
	protected boolean connect() {
		scanner = new Scanner(System.in);
		return true;
	}

	@Override
	protected int getOrder(CmdDataReq req, CmdDataAck ack) {
		String line;
		while (true) {
			System.out.println("请输入请求参数");
			line = scanner.nextLine();
			if (line.equalsIgnoreCase("quit")) {
				stop();
				ack.retn = 3001;
				break;
			}
			if (StringUtils.isBlank(line)) {
				continue;
			}
			Object[] p = null;
			try {
				p = format.parse(line.trim());
			} catch (ParseException e) {
				e.printStackTrace();
			}
			ack.ordercode = (String)p[0];
			ack.phone_no =  (String)p[1];
			ack.imsi_no =  (String)p[2];
			ack.ss_info1 = (String)p[3];
			ack.ss_info2 = (String)p[4];
			ack.ss_info3 = (String)p[5];
			ack.stream_id = "1";
			ack.retn = 2001;
			return 1;
		}
		return 0;
	}

	@Override
	public void close() {
		scanner.close();
	}

}
