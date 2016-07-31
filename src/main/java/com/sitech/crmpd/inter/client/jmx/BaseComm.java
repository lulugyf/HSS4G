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
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.TimeUnit;

import com.sitech.crmpd.inter.client.jmx.func.HttpExecutor;
import com.sitech.crmpd.inter.client.jmx.func.TaskObj;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.sitech.crmpd.inter.common.BasePkt;
import com.sitech.crmpd.inter.common.CmdDataAck;
import com.sitech.crmpd.inter.common.CmdDataReq;
import com.sitech.crmpd.inter.common.Constants;
import com.sitech.crmpd.inter.common.LoginAck;
import com.sitech.crmpd.inter.common.LoginReq;

/* 与manager通讯的部分 */

public abstract class BaseComm {

//	private static Logger logger = LoggerFactory.getLogger(Communication.class);
	protected Logger logger;
	protected Properties properties;
	protected HttpSoapCaller caller;
	
	private boolean running;
	
	private List<HttpSoapCaller> callers = null; //多端口执行指令
	private Map<String, Integer> order2caller = null;  //保存ordercode 对应哪个caller

	public void setProps(Properties properties){ this.properties = properties; }
	public void setCaller(HttpSoapCaller c){ this.caller = c; }
	public void setLogger(Logger logger){ this.logger = logger; }
	
	/**
	 * 添加指令端口， 原有构造函数中的端口会放入第一个元素
	 * @param c
	 */
	public void addCaller(HttpSoapCaller c){
		if(callers == null){
			callers = new ArrayList<HttpSoapCaller>();
			callers.add(caller);
		}
		callers.add(c);
	}

	protected abstract boolean connect() ;
	
	protected abstract int getOrder(CmdDataReq req, CmdDataAck ack) ;

	public abstract void close() ;

	
	public void stop() {
		running = false;
	}

	private void initOrder2Caller(){
		if(callers == null)
			return;
		order2caller = new HashMap<String, Integer>();
		for(int i=callers.size(); i>0; i--){
			int ii = i-1;
			for(String ordercode: callers.get(ii).getAllOrderCode()){
				order2caller.put(ordercode, ii);
			}
		}
	}
	public void for_ever() {
		if (!connect()) {
			logger.warn("can not connect to manager");
			return;
		}
		logger.info("manager port connected.");
		initOrder2Caller();
		final CmdDataReq req = new CmdDataReq();
		final CmdDataAck ack = new CmdDataAck();
		final byte[] buf = new byte[2048];
		req.type = CmdDataReq.ONELY_GET;
		int ret;
		long t1, t2;
		running = true;

		HttpSoapCaller c = null;
		while (running) {
			ret = getOrder(req, ack);
			if (ret <= 0) {
				logger.warn("getOrder failed[{}], break", ret);
				break;
			}
			if (ack.retn == 3001) {
				logger.debug("no data, sleep 5s");
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


				if(callers != null){
					Integer i = order2caller.get(ack.ordercode);
					if(i != null )
						c = callers.get(i);
					else
						c = caller;
				}else{
					c = caller;
				}
				c.apply(ack, req);

				t2 = System.currentTimeMillis();
				logger.info("order_time:{} tm:{}(ms) retn:{} desc:{}", new Object[] { ack.ordercode,
						t2 - t1, req.retn, req.info});
			}
		}
		close();
		running = false;
		logger.info("worker thread ended!!");
	}

	/*public void for_ever(int maxExecutor) {
		if (!connect()) {
			logger.warn("can not connect to manager");
			return;
		}
		logger.info("manager port connected.");
		initOrder2Caller();

		HttpExecutor poolExecutor = HttpExecutor.getInstance();

		ArrayBlockingQueue<TaskObj> replyQ = new ArrayBlockingQueue<TaskObj>(maxExecutor);
		int curExecutor = 0; //当前在执行的指令数量

		final CmdDataReq req = new CmdDataReq();
		final CmdDataAck ack = new CmdDataAck();
		final byte[] buf = new byte[2048];
		req.type = CmdDataReq.ONELY_GET;
		int ret;
		long t1, t2;
		running = true;

		HttpSoapCaller c = null;
		TaskObj obj = null;
		while (running) {
			// 1. try to poll reply from executors thread pool
			obj = null;
			try {
				if(curExecutor >= maxExecutor) {
					obj = replyQ.poll(5, TimeUnit.SECONDS); // poll wait 5 seconds
				}else if(curExecutor > 0){
					obj = replyQ.poll();
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			// 2. try to parse reply str
			if(obj == null){
				if(curExecutor >= maxExecutor)
					continue; // if exeed maxExecutor, don'task get more command
				req.type = CmdDataReq.ONELY_GET;
			}else{
				curExecutor --;
				req.type = CmdDataReq.REPLY_GET;
				obj.toReq(req);
				obj.c.postApply(ack, req, obj.response_str);
				t2 = System.currentTimeMillis();
				logger.info("order_time:{} tm:{}(ms) retn:{} desc:{}", new Object[] { obj.ordercode,
						t2 - obj.t1, req.retn, req.info});
			}
			// 3. try to get order from manager
			ret = getOrder(req, ack);
			if (ret <= 0) {
				logger.error("getOrder failed[{}], break", ret);
				break;
			}
			if (ack.retn == 3001) {
				if(curExecutor == 0) {
					logger.debug("no data, sleep 5s");
					try {
						Thread.sleep(5000L);
					} catch (final Exception ex) {
					}
				}
				continue;
			}
			// 4. got one order, find caller
			curExecutor ++;
			logger.info(
					"order to exec: ordercode[{}]stream_id[{}]phone[{}]imsi[{}]info1[{}]info2[{}]info3[{}]",
					new Object[] { ack.ordercode, ack.stream_id, ack.phone_no, ack.imsi_no,
							ack.ss_info1, ack.ss_info2, ack.ss_info3 });
			if(callers != null){ //多个网元
				Integer i = order2caller.get(ack.ordercode);
				if(i != null )
					c = callers.get(i);
				else
					c = caller;
			}else{
				c = caller;
			}

			// 5. push task to thread pool
			if(obj == null) {
				obj = new TaskObj();
				obj.logger = logger;
			}
			obj.c = c;
			obj.fromAck(ack);
			c.preApply(ack, req, obj); //准备执行的请求报文， 如果有查询依赖，则查询指令在函数中阻塞执行
			if(obj.request_str == null) {
				// order code not found, do not push a task to thread pool
				obj.response_str = "1002";
				try {
					replyQ.put(obj);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				continue;
			}
			obj.url = c.getUrl();

			poolExecutor.execute(obj, replyQ);
		}
		close();
		running = false;
		logger.info("worker thread ended!!");
	} */

}
