package com.sitech.crmpd.inter.client.jmx;

public interface ControlMBean {
	public String list();
	public boolean stop(String hlrcode, String hlrport);
	public boolean start(String hlrcode, String hlrport);
}
