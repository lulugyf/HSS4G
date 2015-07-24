package com.sitech.crmpd.inter.client;

import java.io.FileInputStream;
import java.io.IOException;
import java.text.MessageFormat;
import java.text.ParseException;
import java.util.List;
import java.util.Properties;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.TimeUnit;

import org.apache.http.Consts;
import org.apache.http.client.config.RequestConfig;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.config.SocketConfig;
import org.apache.http.entity.ContentType;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.impl.conn.PoolingHttpClientConnectionManager;
import org.apache.http.util.EntityUtils;
import org.beetl.core.Configuration;
import org.beetl.core.GroupTemplate;
import org.beetl.core.Template;
import org.beetl.core.resource.FileResourceLoader;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.sitech.crmpd.inter.common.CmdDataAck;
import com.sitech.crmpd.inter.common.Constants;

/**
 * @author heihuwudi@gmail.com</br> Created By: 2015年2月11日 下午6:28:00
 */
public class SoapCaller {

	/** name="{@link com.sitech.crmpd.inter.client.SoapCaller}" */
	private static final Logger LOGGER = LoggerFactory.getLogger(SoapCaller.class);
	private static final ContentType TEXT_XML = ContentType.create("text/xml", Consts.UTF_8);
	private static final String DATA = System.getProperty("http.keep.test.data.key",
			"http.keep.test.data");
	private static final String PERIOD = System.getProperty("http.keep.test.period.key",
			"http.keep.test.period");
	private Properties properties;
	private GroupTemplate groupTemplate;
	private volatile CloseableHttpClient httpClient;
	private String startTag, endTag;
	private int timeout, retryCount;
	private Properties testProperties;

	private ParamTran ptran;
	private  List<String> IPHEX_TO_INT;

	SoapCaller(Properties properties) throws IOException {
		this.properties = properties;
		String orderPath = properties.getProperty(Constants.ORDER_PATH);
		if (orderPath.charAt(0) != '/') {
			orderPath = System.getenv("ETCDIR") + "/" + orderPath;
		}
		groupTemplate = new GroupTemplate(new FileResourceLoader(orderPath),
				Configuration.defaultConfiguration());
		startTag = properties.getProperty(Constants.START_TAG_KEY, "<ResultCode>");
		endTag = properties.getProperty(Constants.END_TAG_KEY, "</ResultCode>");
		try {
			timeout = Integer.parseInt(properties.getProperty(Constants.REMOTE_TIMEOUT), 60000);
		} catch (final NumberFormatException e) {
			timeout = 60000;
		}
		try {
			retryCount = Integer.parseInt(properties.getProperty(Constants.RETRY_COUNT), 1);
		} catch (final NumberFormatException e) {
			retryCount = 1;
		}
		final boolean httpKeepTest = Boolean.parseBoolean(properties
				.getProperty(Constants.HTTP_KEEP_TEST));
		if (httpKeepTest) {
			testProperties = new Properties();
			try {
				testProperties.load(new FileInputStream(properties
						.getProperty(Constants.HTTP_KEEP_CFG)));
				final MessageFormat format = new MessageFormat("{0}|{1}|{2}|{3}|{4}|{5}");
				final Object[] objects = format.parse(testProperties.getProperty(DATA));
				final String[] params = new String[objects.length];
				for (int i = 0; i < params.length; i++) {
					params[i] = (String) objects[i];
				}
				final String data = createData(params[0], params[1], params[2], params[3],
						params[4], params[5]);
				final long period = TimeUnit.SECONDS.toMillis(Integer.parseInt(testProperties
						.getProperty(PERIOD, "60")));
				new Timer(true).schedule(new TimerTask() {

					@Override
					public void run() {
						retry(data, 0, false);
					}
				}, period, period);
			} catch (final ParseException e) {}

		}
	}

	public void setTran(ParamTran t){
		ptran = t;
	}

	private synchronized CloseableHttpClient getHttpClient() {
		if (httpClient == null) {
			final SocketConfig config = SocketConfig.custom().setSoTimeout(timeout).build();

			final PoolingHttpClientConnectionManager cm = new PoolingHttpClientConnectionManager();
			final int max = 100;
			cm.setMaxTotal(max);
			cm.setDefaultMaxPerRoute(max);
			cm.setDefaultSocketConfig(config);
			httpClient = HttpClients
					.custom()
					.setConnectionManager(cm)
					.setDefaultSocketConfig(config)
					.setDefaultRequestConfig(
							RequestConfig.custom().setConnectTimeout(timeout)
							.setConnectionRequestTimeout(timeout).setSocketTimeout(timeout)
							.build()).build();
		}
		return httpClient;
	}

	/**
	 * @param orderCode
	 * @param phoneNo
	 * @param imsi
	 * @param ssInfo1
	 * @param ssInfo2
	 * @param ssInfo3
	 * @return 指令执行结果
	 */
	//	public int apply(String orderCode, String phoneNo, String imsi, String ssInfo1, String ssInfo2,
	//			String ssInfo3) {
	//		final String data = createData(orderCode, phoneNo, imsi, ssInfo1, ssInfo2, ssInfo3);
	//		return retry(data, retryCount);
	//	}

	public int apply(CmdDataAck cmd) {
		if(ptran != null) {
			ptran.tran(cmd);
		}
		final String data = createData(cmd.ordercode, cmd.phone_no, cmd.imsi_no,
				cmd.ss_info1, cmd.ss_info2, cmd.ss_info3);
		return retry(data, retryCount);
	}

	private int retry(String data, int retry, boolean logIt) {
		CloseableHttpResponse httpResponse = null;
		HttpPost httpPost = null;
		try {
			httpPost = new HttpPost(properties.getProperty(Constants.REMOTE_URL));
			httpPost.setHeader("SOAPAction", "");
			httpPost.setEntity(new StringEntity(data, TEXT_XML));
			if (logIt) {
				LOGGER.info("send = [{}]", data);
			}
			httpResponse = getHttpClient().execute(httpPost);
			data = EntityUtils.toString(httpResponse.getEntity(), "UTF-8");
			if (logIt) {
				LOGGER.info("receive = [{}]", data);
			}
			final int lIndex = data.indexOf(startTag);
			if (lIndex != -1) {
				final int rIndex = data.indexOf(endTag, lIndex);
				if (rIndex != -1) {
					final String resultCode = data.substring(lIndex + startTag.length(), rIndex);
					try {
						return Integer.parseInt(resultCode) % 10000;
					} catch (final NumberFormatException e) {
						if (logIt) {
							LOGGER.error(e.getMessage());
						}
					}
				}
			}
		} catch (final IOException e) {
			if (httpPost != null) {
				httpPost.abort();
			}
			if (httpClient != null) {
				try {
					httpClient.close();
				} catch (final IOException ee) {}
				httpClient = null;
			}
			if (logIt) {
				LOGGER.error("", e);
			}
			if (retry > 0) {
				return retry(data, retry - 1);
			}
		} finally {
			if (httpResponse != null) {
				try {
					httpResponse.close();
				} catch (final IOException e) {}
			}
		}
		return 7710;
	}

	/**
	 * @param data2
	 * @param i
	 * @return
	 */
	private int retry(String data, int retry) {
		return retry(data, retry, true);
	}

	private String createData(String orderCode, String phoneNo, String imsi, String ssInfo1,
			String ssInfo2, String ssInfo3) {
		final Template template = groupTemplate.getTemplate(orderCode);
		template.binding(properties);
		template.binding("phoneNo", phoneNo);
		template.binding("imsi", imsi);
		template.binding("ssInfo1", ssInfo1);

		//如果是需要转换IP的指令，转换IP为16进制

		if(IPHEX_TO_INT!=null && IPHEX_TO_INT.contains(orderCode)){
			final String intIP = IP_HEX_to_INT(ssInfo2);
			ssInfo2=intIP;
		}

		template.binding("ssInfo2", ssInfo2);
		template.binding("ssInfo3", ssInfo3);
		return template.render();
	}

	public void setIPox(List<String> IPHEX_TO_INT){
		this.IPHEX_TO_INT=IPHEX_TO_INT;
	}

	/**
	 * 将16进制的 IP地址转为 10进制
	 * @param hexString
	 * @return
	 */
	private static String IP_HEX_to_INT(String hexString){
		StringBuffer retIP= new StringBuffer();
		if(hexString==null || hexString.length()!=8){//IP地址参数错误
			return " ";
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
			return " ";
		}
		
		System.out.println(retIP.toString());
		return retIP.toString();
	}
}
