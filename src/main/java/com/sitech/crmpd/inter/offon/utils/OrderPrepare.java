package com.sitech.crmpd.inter.offon.utils;

import com.sitech.crmpd.inter.offon.mgr.CmdDataAck;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

/**
 * 参数值的对应转换， 
 *   用途是行业apn指令里， 原来的指令中apn是直接输入域名的， 现在华为的需要输入模板号（域名由huawei配置成模板）
 * @author guanyf
 *
 *配置文件格式如下：
_orders=3781
_paramIndex=3               序号： phoneNo|imsi|ssInfo1|ssInfo2|ssInfo3  对应 1 | 2 | 3 | 4 | 5, 这里只允许使用 3 4 5
abkrd.sc=1
abncxys.sc=2
bcsw.sc=3
bzkrd.sc=4
bznjxy.sc=5
cdacjt.sc=6
cdairchina.sc=7
cdbocd.sc=8
cdbocd2.sc=9
cdccb.sc=10
cdcgt.sc=11
cdcqwt.sc=12
cdcsdl.sc=13
cdcszm.sc=14
cdczc.sc=15
cddgj.sc=16
cddlgs.sc=17
cddycgt.sc=18
cddyj.sc=19
cdgh.sc=20
cdghatm1.sc=21
cdghatm2.sc=22
cdgjjt.sc=23
cdgs.sc=24
cdhtdy.sc=25
cdhxcbdlte.sc=26
cdjgkj.sc=27
cdjh.sc=28
cdjyj.sc=29
cdkrd.sc=30
cdlkl.sc=31
cdmptx.sc=32
cdnd.sc=33
cdnjnr.sc=34

 */

public class OrderPrepare {
	private Properties apnmap;
	private Properties apnorders = new Properties();
	private int paramIndex;

	private Map<String, Integer> ip_hex2int = new HashMap<String, Integer>();
	
	
	public OrderPrepare(String apn2id_cfg, String iptrans_orders){
		if(apn2id_cfg != null) {
			apnmap = new Properties();
			try {
				apnmap.load(new FileInputStream(apn2id_cfg));
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
			String o = apnmap.getProperty("_orders");
			for (String i : o.split("\\,")) {
				apnorders.setProperty(i, "1");
			}

			o = apnmap.getProperty("_paramIndex");
			paramIndex = Integer.parseInt(o);
		}

		if(iptrans_orders != null && iptrans_orders.length() >= 4) {
			String[] x = iptrans_orders.split("\\,");
			for (String order : x) {
				ip_hex2int.put(order.trim(), 1);
			}
		}
	}


	/**
	 * IP地址从hex到int 的转换
	 * 非常局限的配置方式， ip地址只能在 ss_info2 位置
	 * @param cmd
	 * @return
	 */
	final private boolean IP_HEX_to_INT(CmdDataAck cmd){
		String hexString = cmd.ss_info2;
		if(hexString.indexOf('.') > 0)
			return true; //已经是带.格式的地址， 可能是已经转换了的
		StringBuffer retIP= new StringBuffer();
		if(hexString==null || hexString.length()!=8){//IP地址参数错误
			return false;
		}
		try{
			for(int i=0;i<8;){
				String ips=hexString.substring(i, i+2);
				i+=2;
				retIP.append(Integer.parseInt(ips, 16));
				if(i>=7) ;


				else retIP.append(".");
			}
		}catch(Exception e){
			System.out.println("转换10进制IP错误："+e.getMessage());
			return false;
		}

		//System.out.println(retIP.toString());
		cmd.ss_info2 = retIP.toString();
		return true;
	}

	final public void prepare(CmdDataAck cmd){
		if(ip_hex2int.containsKey(cmd.ordercode))
			IP_HEX_to_INT(cmd);

		if(apnorders.containsKey(cmd.ordercode)){
			apntran(cmd);
		}
	}


	final private boolean apntran(CmdDataAck cmd){
		// 转换域名 到 模板id

		String old = null;
		switch(paramIndex){
		case 3:
			old = cmd.ss_info1;
			break;
		case 4:
			old = cmd.ss_info2;
			break;
		case 5:
			old = cmd.ss_info3;
			break;
		}
		
		if(old == null)
			return false;
		
		old = old.toUpperCase(); ///将传入的域名转为 大写
		System.out.println("转换前："+old);
		
		if(!apnmap.containsKey(old))
			return false;
		switch(paramIndex){
		case 3:
			cmd.ss_info1 = apnmap.getProperty(old);
			break;
		case 4:
			cmd.ss_info2 = apnmap.getProperty(old);
			break;
		case 5:
			cmd.ss_info3 = apnmap.getProperty(old);
			break;
		}
		System.out.println("转换后："+ apnmap.getProperty(old));
		return true;
	}
	
	
	public static void main(String [] str){
		System.out.println("***");
		OrderPrepare pt=new OrderPrepare("f:/1.cfg", null);
		System.out.println("***");
		CmdDataAck ack=new CmdDataAck();
		System.out.println("***");
		ack.imsi_no="15982462591";
		ack.ordercode="3771";
		ack.phone_no="15982462591";
		ack.ss_info1="6";
		ack.ss_info2="A0A0A0A0";
		ack.ss_info3="CMNET";
		pt.prepare(ack);
		System.out.println(ack.ss_info3);
		
	}
}
