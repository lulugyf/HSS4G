package com.sitech.crmpd.inter.common;

public final class LoginReq extends BasePkt {
	public static String cmd_type = "00000001";
	public String username; /* 用户名称 */
	public static int len_username = 8;
	public String userpswd; /* 用户密码 */
	public static int len_userpswd = 8;
	public String hlrcode; /* 交换机代码 */
	public static int len_hlrcode = 4;
	public char hlrport; /* 通信端口 */
	public char termtype; /* 终端类型 */
	/* char reserve[2] 保留 */
	private static int body_len = 24;

	public int toByteArray(byte[] buf, int transid) {
		int offset = 0;
		gen_header(buf, cmd_type, body_len, transid);
		offset += header_len;
		str2ba(username, len_username, buf, offset);
		offset += len_username;
		str2ba(userpswd, len_userpswd, buf, offset);
		offset += len_userpswd;
		str2ba(hlrcode, len_hlrcode, buf, offset);
		offset += len_hlrcode;
		buf[offset++] = (byte) hlrport;
		buf[offset++] = (byte) termtype;
		return offset;
	}
}
