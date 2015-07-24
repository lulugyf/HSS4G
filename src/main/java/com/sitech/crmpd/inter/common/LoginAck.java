package com.sitech.crmpd.inter.common;

public final class LoginAck extends BasePkt {
	public int retn; /* 返回代码 */
	private static int len_retn = 4;
	char encrypt; /* 加密标志 */
	/* char reserve[3]; 保留 */
	private static int len_reserve = 3;
	public String cipher; /* 密钥 */
	private static int len_cipher = 8;

	public int fromByteArray(byte[] buf) {
		int offset = header_len;
		retn = ba2int(buf, offset, len_retn);
		offset += len_retn;
		encrypt = (char) buf[offset++];
		offset += len_reserve;
		cipher = new String(buf, offset, len_cipher);
		offset += len_cipher;
		return offset;
	}
}
