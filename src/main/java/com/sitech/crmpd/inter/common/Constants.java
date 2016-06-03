package com.sitech.crmpd.inter.common;

/**
 * @author Administrator
 *
 */
public class Constants {
	public static final String HLRCODE = System.getProperty("hlrcode.key",
			"hlrcode");
	public static final String HLRPORT = System.getProperty("hlrport.key",
			"hlrport");
	public static final String HTTP_KEEP_TEST = System.getProperty(
			"http.keep.test.key", "http.keep.test");
	public static final String HTTP_KEEP_CFG = System.getProperty(
			"http.keep.cfg.key", "http.keep.cfg");
	public static final String MANAGER_IP = System.getProperty(
			"manager.ip.key", "manager.ip");
	public static final String MANAGER_PORT = System.getProperty(
			"manager.port.key", "manager.port");
	public static final String MANAGER_USER = System.getProperty(
			"manager.user.key", "manager.user");
	public static final String MANAGER_PASSWORD = System.getProperty(
			"manager.password.key", "manager.password");
	public static final String REMOTE_URL = System.getProperty(
			"remote.url.key", "remote.url");
	public static final String REMOTE_TIMEOUT = System.getProperty(
			"remote.timeout.key", "remote.timeout");
	public static final String RETRY_COUNT = System.getProperty(
			"remote.timeout.key", "retry.count");
	public static final String ORDER_PATH = System.getProperty(
			"order.path.key", "order.path");
	public static final String START_TAG_KEY = System.getProperty(
			"start.tag.key", "start.tag");
	public static final String END_TAG_KEY = System.getProperty("end.tag.key",
			"end.tag");
	public static final String TERM_TYPE = System.getProperty("term.type.key",
			"term.type");
	public static final String RESULT_MAP = System.getProperty("result.map.key",
			"result.map");
}
