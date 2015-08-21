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

public abstract class BaseComm {

//	private static Logger logger = LoggerFactory.getLogger(Communication.class);
	protected Logger logger;
	protected Properties properties;
	protected HttpSoapCaller caller;
	
	private boolean running;
	
	private List<HttpSoapCaller> callers = null; //多端口执行指令
	private Map<String, Integer> order2caller = null;  //保存ordercode 对应哪个一个caller
	private Map<String, List<SubOrder>> groupOrders = null; //多端口指令组合

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
	
	/**
	8001:
      - s8002 0
      - s8003 1
      - s8004 2
    8002: s8005 0
	 * @param m
	 */
	public void setOrderGroups(Map m){
		groupOrders = new HashMap<String, List<SubOrder>>();
		for(Object o: m.keySet()){
			if(! (o instanceof String))
				continue;
			String k = (String)o;
			o = m.get(o);
			List<SubOrder> s = new ArrayList<SubOrder>();
			if(o instanceof String){
				String v = (String)o;
				String[] vs = v.trim().split("\\ +");
				if(vs.length < 2)
					continue;
				s.add(new SubOrder(vs[0], Integer.parseInt(vs[1])));
			}else if(o instanceof List){
				for(Object ol: (List)o){
					if(ol instanceof String){
						String v = (String)ol;
						String[] vs = v.trim().split("\\ +");
						if(vs.length < 2)
							continue;
						s.add(new SubOrder(vs[0], Integer.parseInt(vs[1])));						
					}
				}
			}
			groupOrders.put(k, s);
		}

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
		String last_reply = null;
		HttpSoapCaller c = null;
		while (running) {
			ret = getOrder(req, ack);
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
				if(groupOrders != null && groupOrders.containsKey(ack.ordercode)){ //执行接口组合指令
					logger.debug("groups---");
					String org_order = ack.ordercode;
					int retn = -1;
					for(SubOrder so:  groupOrders.get(ack.ordercode)){
						c = callers.get(so.caller_index);
						c.setLastReplay(last_reply);
						ack.ordercode = so.ordercode;
						retn = c.apply(ack, req);
						if(retn != 0)
							break;
						last_reply = c.getLastReply();
					}
					ack.ordercode = org_order;
					req.ordercode = org_order;
					req.retn = retn;
				}else{
					logger.debug("--default caller");
					if(callers != null){
						Integer i = order2caller.get(ack.ordercode);
						if(i != null )
							c = callers.get(i);
						else
							c = caller;
					}else{
						c = caller;
					}
					c.setLastReplay(last_reply);
					c.apply(ack, req);
					last_reply = c.getLastReply();
				}
				t2 = System.currentTimeMillis();
				logger.info("order_time:{} tm:{}(ms) retn:{} desc:{}", new Object[] { ack.ordercode,
						t2 - t1, req.retn, req.info});
			}
		}
		close();
		running = false;
		logger.info("worker thread ended!!");
	}
	
	
	
	static class SubOrder{
		public String ordercode;
		public int caller_index;
		public SubOrder(String oc, int ci){
			ordercode = oc;
			caller_index = ci;
		}
	}

}
