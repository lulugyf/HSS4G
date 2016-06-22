package com.sitech.crmpd.inter.client.jmx;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.MessageFormat;
import java.text.ParseException;
import java.util.Properties;
import java.util.Scanner;

import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.sitech.crmpd.inter.common.CmdDataAck;
import com.sitech.crmpd.inter.common.CmdDataReq;

public class FileComm extends BaseComm {

	private final MessageFormat format = new MessageFormat("{0}|{1}|{2}|{3}|{4}|{5}");
	private String fname;
	private BufferedReader bReader;
	private int lineno = 0;
	
	public FileComm(String filename){
		fname = filename;
	}
	
	@Override
	protected boolean connect() {
		try {
			bReader = new BufferedReader(new FileReader(fname));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}

	@Override
	protected int getOrder(CmdDataReq req, CmdDataAck ack) {
		
		String line = null;
		while (true) {
			try {
				line = bReader.readLine();
			} catch (IOException e1) {
				e1.printStackTrace();
				break;
			}
			if(line == null)
				break;
			if (line.startsWith("#")) {
				continue;
			}
			line = line.trim();
			if (line.length() == 0) {
				continue;
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
			ack.stream_id = String.valueOf(lineno++);
			ack.retn = 2001;
			return 1;
		}
		return 0;
	}

	@Override
	public void close() {
		try {
			bReader.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
