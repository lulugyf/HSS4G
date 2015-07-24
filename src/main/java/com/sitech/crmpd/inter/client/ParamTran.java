package com.sitech.crmpd.inter.client;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.*;
import java.util.Properties;

import com.sitech.crmpd.inter.common.CmdDataAck;

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

public class ParamTran {
	private Properties p;
	private Properties orders;
	private int paramIndex;
	
	
	public ParamTran(String cfgfile){
		p = new Properties();
		try {
			p.load(new FileInputStream(cfgfile));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		String o = p.getProperty("_orders");
		orders = new Properties();
		for(String i: o.split("\\,")){
			orders.setProperty(i, "1");
		}
		
		o = p.getProperty("_paramIndex");
		paramIndex = Integer.parseInt(o);
		
		
	}
	
	public ParamTran(String parent, String cfgfile){
		p = new Properties();
		try {
			p.load(new FileInputStream(new File(parent, cfgfile)));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		String o = p.getProperty("_orders");
		
		orders = new Properties();
		for(String i: o.split("\\,")){
			orders.setProperty(i, "1");
		}
		
		o = p.getProperty("_paramIndex");
		paramIndex = Integer.parseInt(o);
	
		
		
	}



	public boolean tran(CmdDataAck cmd){
		
		//再转换域名
		if(!orders.containsKey(cmd.ordercode))
			return false;
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
		
		old=old.toUpperCase(); ///将传入的域名转为 大写
		System.out.println("转换前："+old);
		
		if(!p.containsKey(old))
			return false;
		switch(paramIndex){
		case 3:
			cmd.ss_info1 = p.getProperty(old);
			break;
		case 4:
			cmd.ss_info2 = p.getProperty(old);
			break;
		case 5:
			cmd.ss_info3 = p.getProperty(old);
			break;
		}
		System.out.println("转换后："+p.getProperty(old));
		return true;
	}
	
	
	public static void main(String [] str){
		System.out.println("***");
		ParamTran pt=new ParamTran("f:/1.cfg");
		System.out.println("***");
		CmdDataAck ack=new CmdDataAck();
		System.out.println("***");
		ack.imsi_no="15982462591";
		ack.ordercode="3771";
		ack.phone_no="15982462591";
		ack.ss_info1="6";
		ack.ss_info2="A0A0A0A0";
		ack.ss_info3="CMNET";
		pt.tran(ack);
		System.out.println(ack.ss_info3);
		
	}
}
