package com.sitech.crmpd.inter.client.jmx;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.sitech.crmpd.inter.common.BasePkt;
import com.sitech.crmpd.inter.common.CmdDataAck;
import com.sitech.crmpd.inter.common.CmdDataReq;
import com.sitech.crmpd.inter.common.Constants;
import com.sitech.crmpd.inter.common.LoginAck;
import com.sitech.crmpd.inter.common.LoginReq;

/* 与manager通讯的部分 */

public class ManagerComm extends BaseComm{

	private static int transid = 1;
	private Socket sock = null;
	private InputStream iStream = null;
	private OutputStream oStream = null;
	private byte[] buf = new byte[2048];
	
	@Override
	public boolean connect() {
		final String mgr_ip = properties.getProperty(Constants.MANAGER_IP);
		final String mgr_port = properties.getProperty(Constants.MANAGER_PORT);
		final String mgr_user = properties.getProperty(Constants.MANAGER_USER);
		final String mgr_pswd = properties.getProperty(Constants.MANAGER_PASSWORD);
		final String term_type = properties.getProperty(Constants.TERM_TYPE);
		final String hlrcode = properties.getProperty(Constants.HLRCODE);
		final String hlrport = properties.getProperty(Constants.HLRPORT);

		return connect(mgr_ip, Integer.parseInt(mgr_port), mgr_user, mgr_pswd, hlrcode, hlrport,
				term_type);
	}

	// 与 offon_manager 建立连接, 并登录
	private boolean connect(String ip, int port, String user, String pwd, String hlrcode,
			String hlrport, String termtype) {
		final LoginReq req = new LoginReq();
		final LoginAck ack = new LoginAck();

		req.hlrcode = hlrcode;
		req.hlrport = hlrport.charAt(0);
		req.termtype = termtype.charAt(0);
		req.username = user;
		req.userpswd = pwd;
		final byte[] buf = new byte[100];
		req.toByteArray(buf, transid++);
		try {
			sock = new Socket(ip, port);
			iStream = sock.getInputStream();
			oStream = sock.getOutputStream();
			BasePkt.write_pkt(oStream, buf);
			BasePkt.read_pkt(iStream, buf);
			ack.fromByteArray(buf);
			logger.info("login return [{}]", ack.retn);
			if (ack.retn == 0) {
				return true;
			}
		} catch (final Exception ex) {
			logger.error("connect(" + ip + ":" + port + ") failed", ex);
		}
		return false;
	}

	@Override
	public int getOrder(CmdDataReq req, CmdDataAck ack) {
		int len = -1;
		try {
			len = req.toByteArray(buf, transid++);
			BasePkt.write_pkt(oStream, buf);
			len = BasePkt.read_pkt(iStream, buf);
			if (len > 0) {
				len = ack.fromByteArray(buf);
			}
		} catch (final IOException ex) {
			logger.error("", ex);
			len = -1;
		}
		return len;
	}

	@Override
	public void close() {
		try {
			if (iStream != null) {
				iStream.close();
			}
			if (oStream != null) {
				oStream.close();
			}
			if (sock != null) {
				sock.close();
			}
		} catch (final Exception ex) {
			logger.error("", ex);
		}
	}
}
