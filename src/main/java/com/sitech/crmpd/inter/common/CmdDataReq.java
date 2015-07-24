package com.sitech.crmpd.inter.common;


public final class CmdDataReq extends BasePkt
{
	public static String req_type = "10000002";  // GETORDER
	public static char ONELY_GET = '1'; 
	public static char REPLY_GET = '2';
	public int retn;
	public static int len_retn = 4;
	public char type;
	public String phone_no;
	public static int len_phone_no = 16;
	public String stream_id;
	public static int len_stream_id = 16;
	public String ordercode;
	public static int len_ordercode = 4;
	public String info;
	public static int len_info = 201;
	private static int body_len = 242;
	
	public int toByteArray(byte[] buf, int transid)
	{
		int offset = 0;
		gen_header(buf, req_type, body_len, transid);
		offset += header_len;
		int2ba(retn, len_retn, buf, offset); offset += 4;
		buf[offset] = (byte)type; offset += 1;
		str2ba(phone_no, len_phone_no, buf, offset); offset += len_phone_no;
		str2ba(stream_id, len_stream_id, buf, offset); offset += len_stream_id;
		str2ba(ordercode, len_ordercode, buf, offset); offset += len_ordercode;
		str2ba(info, len_info, buf, offset); offset += len_info;
		return offset;
	}

}
