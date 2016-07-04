package com.sitech.crmpd.inter.common;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Properties;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import ch.qos.logback.classic.LoggerContext;
import ch.qos.logback.classic.encoder.PatternLayoutEncoder;
import ch.qos.logback.core.rolling.RollingFileAppender;
import ch.qos.logback.core.rolling.TimeBasedRollingPolicy;

import com.sitech.crmpd.inter.client.SoapCaller;

/* 与manager通讯的部分 */

public class Communication {

//	private static Logger logger = LoggerFactory.getLogger(Communication.class);
	private Logger logger;
	private static int transid = 1;
	private Socket sock = null;
	private InputStream iStream = null;
	private OutputStream oStream = null;
	private Properties properties;
	private SoapCaller caller;
	
	private boolean running;


	public Communication(Properties properties, SoapCaller caller) {
		this.properties = properties;
		this.caller = caller;
		logger = LoggerFactory.getLogger(Communication.class);
	}
	
	public Communication(Properties properties, SoapCaller caller, Logger log) {
		this.properties = properties;
		this.caller = caller;
		this.logger = log;
	}

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

	public int getOrder(byte[] buf, CmdDataReq req, CmdDataAck ack) {
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
	
	public void stop() {
		running = false;
	}

	public void start() {
		if (!connect()) {
			logger.warn("can not connect to manager");
			return;
		}
		final CmdDataReq req = new CmdDataReq();
		final CmdDataAck ack = new CmdDataAck();
		final byte[] buf = new byte[2048];
		req.type = CmdDataReq.ONELY_GET;
		int ret;
		long t1, t2;
		running = true;
		while (running) {
			ret = getOrder(buf, req, ack);
			if (ret <= 0) {
				logger.warn("getOrder failed[{}], break", ret);
				break;
			}
			if (ack.retn == 3001) {
				logger.warn("no data, sleep 5s");
				try {
					Thread.sleep(5000L);
				} catch (final Exception ex) {}
				req.type = CmdDataReq.ONELY_GET;
			} else {
				logger.info(
						"order to exec: ordercode[{}]stream_id[{}]phone[{}]imsi[{}]info1[{}]info2[{}]info3[{}]",
						new Object[] { ack.ordercode, ack.stream_id, ack.phone_no, ack.imsi_no,
								ack.ss_info1, ack.ss_info2, ack.ss_info3 });
				req.type = CmdDataReq.REPLY_GET;
				t1 = System.currentTimeMillis();
//				req.retn = caller.apply(ack.ordercode, ack.phone_no, ack.imsi_no, ack.ss_info1,
//						ack.ss_info2, ack.ss_info3);
				req.retn = caller.apply(ack);
				t2 = System.currentTimeMillis();
				logger.info("order_time:{} tm:{}(ms) retn:{}", new Object[] { ack.ordercode,
						t2 - t1, req.retn });
				req.stream_id = ack.stream_id;
				req.ordercode = ack.ordercode;
				req.phone_no = ack.phone_no;
				req.info = "returned by hss client";
			}
		}
		close();
		running = false;
	}

	public static void main(String[] args) throws Exception {
		Communication c = new Communication(null, null);

		//connect(String ip, int port, String user, String pwd, String hlrcode,
			//String hlrport, String termtype)
		boolean r = c.connect("127.0.0.1", 22109, "hlrhei", "hlrhei", "hei", "1", "1");
		if(!r){
			c.logger.error("failed");
			c.close();
			return;
		}

		final CmdDataReq req = new CmdDataReq();
		final CmdDataAck ack = new CmdDataAck();
		final byte[] buf = new byte[2048];
		req.type = CmdDataReq.ONELY_GET;
		int ret;
		long t1, t2;

		while (true) {
			ret = c.getOrder(buf, req, ack);
			if (ret <= 0) {
				c.logger.warn("getOrder failed[{}], break", ret);
				break;
			}
			if (ack.retn == 3001) {
				c.logger.warn("no data, sleep 5s");
				try {
					Thread.sleep(5000L);
				} catch (final Exception ex) {}
				req.type = CmdDataReq.ONELY_GET;
			} else {
				c.logger.info(
						"order to exec: ordercode[{}]stream_id[{}]phone[{}]imsi[{}]info1[{}]info2[{}]info3[{}]",
						new Object[] { ack.ordercode, ack.stream_id, ack.phone_no, ack.imsi_no,
								ack.ss_info1, ack.ss_info2, ack.ss_info3 });
				req.type = CmdDataReq.REPLY_GET;

				req.retn = 0;
				req.stream_id = ack.stream_id;
				req.ordercode = ack.ordercode;
				req.phone_no = ack.phone_no;
				req.info = "success";
			}
		}
		c.close();




	}

}
