package com.sitech.crmpd.inter.client.jmx.func;

import com.sitech.crmpd.inter.client.jmx.HttpSoapCaller;
import com.sitech.crmpd.inter.common.CmdDataAck;
import com.sitech.crmpd.inter.common.CmdDataReq;
import org.slf4j.Logger;

/**
 * Created by guanyf on 2016/7/6.
 */
public class TaskObj {
    public String ordercode;
    public String url;
    public String request_str;
    public String response_str;
    public Logger logger;
    public long t1;
    public HttpSoapCaller c;
}
