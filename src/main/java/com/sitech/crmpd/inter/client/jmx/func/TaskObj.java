package com.sitech.crmpd.inter.client.jmx.func;

import com.sitech.crmpd.inter.client.jmx.HttpSoapCaller;
import com.sitech.crmpd.inter.common.CmdDataAck;
import com.sitech.crmpd.inter.common.CmdDataReq;
import org.slf4j.Logger;

/**
 * Created by guanyf on 2016/7/6.
 */
public class TaskObj {
    public String stream_id;
    public String phone_no;
    public String ordercode;
    public String url;
    public String request_str;
    public String response_str;
    public Logger logger;
    public long t1;
    public boolean isQuery = false; //是否查询指令
    public CmdDataAck cmd; // 对于查询指令， 需要保存原指令
    public HttpSoapCaller c;

    public void toReq(CmdDataReq req) {
        req.stream_id = stream_id;
        req.ordercode = ordercode;
        req.phone_no = phone_no;
    }

    public void fromAck(CmdDataAck ack) {
        ordercode = ack.ordercode;
        stream_id = ack.stream_id;
        phone_no = ack.phone_no;
        t1 = System.currentTimeMillis();
    }
}
