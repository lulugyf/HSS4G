package com.sitech.crmpd.inter.common;

import org.beetl.core.Template;

import java.util.Map;

public class CmdDataAck extends BasePkt {
	//final private static String ack_type = "";
	public int retn; /* 返回代码 */
	final private static int len_retn = 4;
	public String stream_id; /* 操作流水，业务命令的唯一标识 */
	final private static int len_stream_id = 16;
	public String ordercode; /* 指令代码 */
	final private static int len_ordercode = 4;
	public String phone_no; /* 移动电话号码 */
	final private static int len_phone_no = 16;
	public String imsi_no; /* IMSI号码 */
	final private static int len_imsi_no = 21;
	public String ss_info1; /* 补充信息1 */
	final private static int len_ss_info1 = 16;
	public String ss_info2; /* 补充信息2 */
	final private static int len_ss_info2 = 21;
	public String ss_info3; /* 补充信息3 */
	final private static int len_ss_info3 = 121;

	private String getString(byte[] buf, int offset, int len) {
		int pos;
		for (pos = offset; pos < offset + len; pos++) {
			if (buf[pos] == (byte) 0) {
				break;
			}
		}
		return new String(buf, offset, pos - offset);
	}

	public int fromByteArray(byte[] buf) {
		int offset = header_len;
		retn = ba2int(buf, offset, len_retn);
		offset += len_retn;
		stream_id = getString(buf, offset, len_stream_id).trim();
		offset += len_stream_id;
		ordercode = getString(buf, offset, len_ordercode).trim();
		offset += len_ordercode;
		phone_no = getString(buf, offset, len_phone_no).trim();
		offset += len_phone_no;
		imsi_no = getString(buf, offset, len_imsi_no);
		offset += len_imsi_no;
		ss_info1 = getString(buf, offset, len_ss_info1).trim();
		offset += len_ss_info1;
		ss_info2 = getString(buf, offset, len_ss_info2).trim();
		offset += len_ss_info2;
		ss_info3 = getString(buf, offset, len_ss_info3).trim();
		offset += len_ss_info3;
		return offset;
	}

	public String render(Template template, Map<String, String> addProp) {
		if(addProp != null)
			template.binding(addProp);
		template.binding("phoneNo", this.phone_no);
		template.binding("imsi",    this.imsi_no);
		template.binding("ssInfo1", this.ss_info1);
		template.binding("ssInfo2", this.ss_info2);
		template.binding("ssInfo3", this.ss_info3);
		return template.render();
	}

	public void binding(Template template) {
		template.binding("phoneNo", this.phone_no);
		template.binding("imsi",    this.imsi_no);
		template.binding("ssInfo1", this.ss_info1);
		template.binding("ssInfo2", this.ss_info2);
		template.binding("ssInfo3", this.ss_info3);
	}

	public CmdDataAck clone() {
		CmdDataAck c = new CmdDataAck();
		c.ordercode = ordercode;
		c.phone_no = phone_no;
		c.imsi_no = imsi_no;
		c.ss_info1 = ss_info1;
		c.ss_info2 = ss_info2;
		c.ss_info3 = ss_info3;
		return c;
	}
}
