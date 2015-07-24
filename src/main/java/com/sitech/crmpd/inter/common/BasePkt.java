package com.sitech.crmpd.inter.common;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class BasePkt {
	private static Logger logger = LoggerFactory.getLogger(BasePkt.class);
	protected static int header_len = 32;
	private static byte[] tag1 = "31200000".getBytes();
	private static byte[] msg_mark = "`CD'".getBytes();
	private static char pseudo[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
		'C', 'D', 'E', 'F' };

	// 983 6 -> 000983
	public static int int2ba(int num, int len, byte[] buf, int offset) {
		int k;
		for (int i = 0; i < len; i++) {
			k = num % (int) Math.pow(10, len - i) / (int) Math.pow(10, len - i - 1) + '0';
			buf[offset + i] = (byte) k;
		}
		return 0;
	}

	public static int ba2int(byte[] buf, int offset, int len) {
		int x = 0;
		for (int i = 0; i < len; i++) {
			x += (buf[offset + i] - '0') * Math.pow(10, len - i - 1);
		}
		return x;
	}

	public int gen_header(byte[] buf, String msgtype, int msglen, int transid) {
		System.arraycopy(msg_mark, 0, buf, 0, 4);
		int2ba(msglen + header_len, 4, buf, 4);
		System.arraycopy(msgtype.getBytes(), 0, buf, 8, 8);
		int2ba(transid, 8, buf, 16);
		System.arraycopy(tag1, 0, buf, 24, 8);
		return header_len;
	}

	protected int str2ba(String str, int len, byte[] buf, int offset) {
		int l = 0;
		if (str != null) {
			final byte[] b = str.getBytes();
			l = Math.min(len, b.length);
			System.arraycopy(b, 0, buf, offset, l);
			offset += l;
		}
		if (len > l) {
			for (int i = 0; i < len - l; i++) {
				buf[offset++] = 0;
			}
		}
		return len;
	}

	public static int read_pkt(InputStream pin, byte[] buf) throws IOException {
		int msglen;
		int ret;
		msglen = pin.read(buf, 0, header_len);
		if (msglen != header_len) {
			return -1;
		}
		msglen = ba2int(buf, 4, 4);
		if (msglen > buf.length) {
			return -3;
		}
		ret = pin.read(buf, header_len, msglen - header_len);
		if (ret != msglen - header_len) {
			return -2;
		}
		logger.debug("read from manager: [{}]", new String(buf, header_len, ret));
		return msglen;
	}

	public static int write_pkt(OutputStream pout, byte[] buf) throws IOException {
		final int msglen = ba2int(buf, 4, 4);
		if (msglen > buf.length) {
			System.out.println("invalid pkt, len:" + msglen);
			return -1;
		}
		logger.trace("write [{}]", new String(buf), msglen);
		pout.write(buf, 0, msglen);
		pout.flush();
		return msglen;
	}

	public static String bytes2HexStr(byte in[]) {
		return bytes2HexStr(in, 0, in.length);
	}

	public static String bytes2HexStr(byte in[], int start, int len) {

		byte ch = 0x00;
		int i = 0;

		if (in == null || in.length <= 0) {
			return null;
		}
		if (start >= in.length) {
			return null;
		}
		if (start + len > in.length) {
			len = in.length - start;
		}

		final StringBuffer out = new StringBuffer(len * 2);
		i = start;
		while (i < len + start) {
			ch = (byte) (in[i] & 0xF0); // Strip off high nibble
			ch = (byte) (ch >>> 4); // shift the bits down
			ch = (byte) (ch & 0x0F); // must do this is high order bit is on!

			out.append(pseudo[ch]); // convert the nibble to a String
			// Character
			ch = (byte) (in[i] & 0x0F); // Strip off low nibble
			out.append(pseudo[ch]); // convert the nibble to a String
			// Character
			i++;
		}
		final String rslt = out.toString();
		out.delete(0, len * 2);
		return rslt;
	}

	public static byte[] hexStr2Bytes(String str) {
		final int len = str.length();
		if (len % 2 > 0) {
			return null;
		}
		final byte[] ret = new byte[len / 2];
		for (int i = 0; i < len / 2; i++) {
			ret[i] = (byte) Integer.parseInt(str.substring(i * 2, i * 2 + 2), 16);
		}
		return ret;
	}

}
