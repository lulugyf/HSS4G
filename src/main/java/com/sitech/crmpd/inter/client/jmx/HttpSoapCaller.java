package com.sitech.crmpd.inter.client.jmx;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.text.MessageFormat;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.TimeUnit;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.http.Consts;
import org.apache.http.StatusLine;
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
import org.beetl.core.resource.StringTemplateResourceLoader;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.yaml.snakeyaml.Yaml;

import com.google.common.base.Splitter;
import com.google.common.base.Splitter.MapSplitter;
import com.sitech.crmpd.inter.client.ParamTran;
import com.sitech.crmpd.inter.common.CmdDataAck;
import com.sitech.crmpd.inter.common.CmdDataReq;
import com.sitech.crmpd.inter.common.Constants;

/**
 * @author heihuwudi@gmail.com</br> Created By: 2015年2月11日 下午6:28:00
 * 
 * change log:
 * 1. 20150723 支持以 .yaml 格式配置指令文件， 如果 orderPath 是以.yaml 结尾的文件，则以yaml解析， 
 * 			否则如果是目录， 则仍然以原有方式处理
 *    yaml 文件格式如下（header footer 必须要有）：
header: |
  <soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:hss="http://www.chinamobile.com/HSS/">
   <soapenv:Header>
    <hss:PassWord>hss9860#^(</hss:PassWord>
    <hss:UserName>NJBOSS</hss:UserName>
   </soapenv:Header>
  <soapenv:Body>
  
footer: |
  </soapenv:Body>
  </soapenv:Envelope>

OP0001: |
  <MOD_ODB>
  <HLRSN>${HLRSN}</HLRSN>
  <ISDN>86${phoneNo}</ISDN>
  <ODBOC>BOIC</ODBOC>
  </MOD_ODB>
OP0011: |
  <MOD_ODB>
  <HLRSN>${HLRSN}</HLRSN>
  <ISDN>86${phoneNo}</ISDN>
  <ODBOC>NOBOC</ODBOC>
  </MOD_ODB>
OP8607: |
  <ADD_KI>
  <HLRSN>${HLRSN}</HLRSN>
  <IMSI>${imsi}</IMSI>
  <OPERTYPE>ADD</OPERTYPE>
  <% var kival=strutil.subStringTo(ssInfo1,0,14)+strutil.subStringTo(ssInfo2,0,18); %>
  <KIVALUE>${kival}</KIVALUE>
  <K4SNO>1</K4SNO>
  <ALG>MILENAGE</ALG>
  <OPCVALUE>${ssInfo3}</OPCVALUE>
  <AMFSNO>1</AMFSNO>
  </ADD_KI>
  
    
    
 * 2. 20150723 支持查询指令后取结果中的内容作为参数传到下一条指令中 
 *   order存放形式如果是 .yaml, 则查找 query_param 的key找到文件， 如果是目录， 则在目录中找query_param.yaml
   本配置的内容也可以直接加在 order指令配置文件中
   QP配置中可以有 query_order 项， 用于配置使用哪个指令来执行查询， 如果没有配置 query_order, 则使用最近一次执行指令的结果解析
    文件格式如下, 解析参数的正则表达式可以有多个：
QP3002: 
 - query_order=9101
 - apntplid=<Group><CNTXID>(\d+)</CNTXID><QOSTPLID>119</QOSTPLID>
 - imsino=<Group><IMSI>(\d+)</IMSI>
QP4001:
 - query_order=9101 
 - imsino=<Group><IMSI>(\d+)</IMSI>

     
     两条指令配置如下:  关闭 blackberry 的指令分解 300 -> 9101/3002
OP9101: |
  <LST_SUB>
  <HLRSN>${HLRSN}</HLRSN>
  <ISDN>86${phoneNo}</ISDN>
  <DETAIL>TRUE</DETAIL>
  </LST_SUB>
OP3002: |
  <hss:MOD_GPRS_CONTEXT>
  <hss:HLRSN>13</hss:HLRSN>
  <hss:ISDN>86${phoneNo}</hss:ISDN>
  <hss:PROV>RMVPDPCNTX</hss:PROV>
  <hss:CNTXID>${apntplid}</hss:CNTXID>
  </hss:MOD_GPRS_CONTEXT>

3011|18483590060||||
9101|18483590060||||
3002|18483590060||||


 * 3. 爱立信的ip地址转换（ 前台送来的是hex格式，需要转换成 n.n.n.n 格式）
 *   设置参数 IPHEX_TO_INT: 3781~4011    （表示 3781 4011 这两个order需要转换 ss_info2 中的hex为十进制）
 *   
 */
public class HttpSoapCaller {

	/** name="{@link com.sitech.crmpd.inter.client.SoapCaller}" */
//	private static final Logger LOGGER = LoggerFactory.getLogger(SoapCaller.class);
	private Logger LOGGER;
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
	
	private Pattern ns2rmv;  //执行结果中需要删除的 namespace
	
	private ParamTran ptran;
	
	private Map<String, Template> orderMap; //以yaml配置的指令，预先load到这里
	private List<String> IPHEX_TO_INT;
	private Map<String, String> addProp;
	
	private String url = null;
	private String last_reply; //最后一次执行指令的返回结果
	
	// 查询结果中提取参数实现
	static class QueryItem{
		String name;
		Pattern pattern;
		QueryItem(String n, Pattern p){ name = n; pattern = p; }
	}
	private Map<String, List<QueryItem>> queryparams = new HashMap<String, List<QueryItem>>(); //查询参数对应需要解析的正则表达式列表
	private Map<String, String> queryorders = new HashMap<String, String>(); //ordercode 对应的查询order
	
	public HttpSoapCaller(Properties properties, Logger log) throws IOException {
		LOGGER = log;
		init(properties);
	}

	HttpSoapCaller(Properties properties) throws IOException {
		LOGGER = LoggerFactory.getLogger(HttpSoapCaller.class);
		init(properties);
	}
	
	/**
	 * 加载查询结果参数解析配置到内存中
	 * @param path
	 */
	@SuppressWarnings("unchecked")
	private void loadQueryParam(String path){
		File f = new File(path);
		if(!f.exists()){
			LOGGER.info("query_param file {} not found ", path);
			return;
		}
		Yaml y = new Yaml();
		Map m = null;
		try {
			m = (Map)y.load(new FileInputStream(path));
		} catch (FileNotFoundException e) {
			LOGGER.error("load query param file failed:"+path, e);
			return;
		}
		for(Object o: m.keySet()){
			String key = (String)o;
			if(key.startsWith("QP"))
				addQP(key, m.get(key));
		}
		LOGGER.info("query_param {} loaded,  size: {}", path, queryparams.size());
	}
	
	@SuppressWarnings("unchecked")
	private void addQP(String key, Object value) {
		key = key.substring(2);
		List<QueryItem> l = new ArrayList<QueryItem>();
		if(value instanceof List){
			for(String s: (List<String>)value){
				int p = s.indexOf('=');
				String varname = s.substring(0,p);
				String varvalue = s.substring(p+1);
				if("query_order".equals(varname)){
					queryorders.put(key, varvalue); //query_order 保存对应的查询指令代码
				}else{
					l.add(new QueryItem(varname, Pattern.compile(varvalue)));
				}
				LOGGER.debug("load {} query {} = {}", new Object[]{key, s.substring(0,p), varvalue});
			}
		}else if(value instanceof String){
//			LOGGER.error("invalid QP config: {}, must be a list", key);
			String s = (String)value;
			int p = s.indexOf('=');
			String varname = s.substring(0,p);
			String varvalue = s.substring(p+1);
			l.add(new QueryItem(varname, Pattern.compile(varvalue)));
		}
		if(l.size() > 0 && queryorders.containsKey(key))
			queryparams.put(key, l);
	}
	
	/**
	 *  获取最后一次指令返回的结果
	 * @return 
	 */
	public String getLastReply(){
		return last_reply;
	}
	public void setLastReplay(String last_reply){
		this.last_reply = last_reply;
	}
	
	private long orderFileChangeTime = 0L; //这个如果>0, 则标识着需要自动检查指令文件是否更新， 更新后自动加载
	                                 //自动加载的配置为增加属性 order_reloadable （值不检测）
	private String orderPath;
	
	/**
	 * 读取指令配置文件
	 * @param init  是否为启动时的加载
	 * @throws IOException
	 */
	private void orderReload(boolean init) throws IOException{
		File yamlfile = new File(orderPath);
		if(!init && !(orderFileChangeTime > 0 && yamlfile.lastModified() > orderFileChangeTime)){
			return;
		}
		LOGGER.warn("loading order config:{}", orderPath);
		Map m = null;
		try{
			m =(Map)new Yaml().load(new FileInputStream(yamlfile));
		}catch(Throwable e){
			LOGGER.error("order file load failed", e);
		}
		orderMap = new HashMap<String, Template>();
		StringTemplateResourceLoader resourceLoader = new StringTemplateResourceLoader();
		Configuration cfg = Configuration.defaultConfiguration();
		GroupTemplate gt = new GroupTemplate(resourceLoader, cfg);
		
		String header = (String)m.get("header");
		String footer = (String)m.get("footer");
		Template t = null;
		for(Object o: m.keySet()){
			String key = (String)o;
			if(key.startsWith("OP")){ //指令配置
				String value = String.valueOf(m.get(key));
				key = key.substring(2);
				if(key.indexOf('.') > 0){
					// 衍生指令:  OP${n}.${m},  被衍生指令为 ${n},  指令后的内容替换被衍生指令的 __var__ 串
					String k = key.substring(0, key.indexOf('.'));
					String v = (String)m.get("OP"+k);
					if(v == null){
						LOGGER.error("regenerated order {} found org failed", key);
						continue;
					}
					v = v.replace("__var__", value);
					t = gt.getTemplate(header + v + footer);
				}else{
					t = gt.getTemplate(header+value+footer);
				}
				orderMap.put(key, t);

			}else if(key.startsWith("QP")){ //查询参数配置
				addQP(key, m.get(key));
			}
		}
		
		if(m.containsKey("query_param")) // 仍然支持由单独的配置文件保存查询参数解析
			loadQueryParam((String)m.get("query_param"));
	}
	
	private void init(Properties properties) throws IOException{
		this.properties = properties;
		url = properties.getProperty(Constants.REMOTE_URL);
		String orderPath = properties.getProperty(Constants.ORDER_PATH);
		if (orderPath.charAt(0) != '/') {
			orderPath = properties.getProperty("ETCDIR") + "/" + orderPath;
		}
		
		File orderFile = new File(orderPath);
		if(!orderFile.exists())
			throw new IOException("orderPath not found");
		if(orderFile.isFile() && orderPath.endsWith(".yaml")){ //loading order as a single yaml file, not a folder
			this.orderPath = orderPath;
//			LOGGER.info("order_reloadable={}", properties.getProperty("order_reloadable"));
			if("true".equalsIgnoreCase(properties.getProperty("order_reloadable"))){
				this.orderFileChangeTime = new File(this.orderPath).lastModified();
				LOGGER.info("---order is reloadable");
			}
			orderReload(true);
		}else if(orderFile.isDirectory()){ // orders in a folder as separated files
			groupTemplate = new GroupTemplate(new FileResourceLoader(orderPath),
				Configuration.defaultConfiguration());
			loadQueryParam(orderPath + "/query_param.yaml");
		}
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
		CmdDataAck cmd = new CmdDataAck();
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
				cmd.ordercode = params[0];
				cmd.phone_no = params[1];
				cmd.imsi_no  = params[2];
				cmd.ss_info1 = params[3];
				cmd.ss_info2 = params[4];
				cmd.ss_info3 = params[5];
				final String data = createData(cmd);
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
		
		
		if(properties.containsKey("APN_TPL_ID"))
			setTran(new ParamTran(properties.getProperty("ETCDIR"), properties.getProperty("APN_TPL_ID")));
		if (properties.containsKey("IPHEX_TO_INT")) {
			setIPox(Arrays.asList(properties.getProperty("IPHEX_TO_INT").split("~")));
		}
		if(properties.containsKey("PROPERTIES")){
			MapSplitter splitter = Splitter.on(',').omitEmptyStrings().trimResults()
					.withKeyValueSeparator('=');
			addProp = splitter.split(properties.getProperty("PROPERTIES"));
		}
		
		if(properties.containsKey("removenamespace")){
			String regstr = "xmlns:(\\w+)=\\\""+properties.getProperty("removenamespace")+"\\\"";
			ns2rmv = Pattern.compile(regstr);
		}

		LOGGER.info("init done");
	}
	
	public void setTran(ParamTran t){
		this.ptran = t;
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
	
	private Template findTemplate(String ordercode){
		Template template = null;
		if(orderMap != null){
			if(orderFileChangeTime > 0L){
				try {
					orderReload(false); //检查order文件是否更新， 更新则自动重新加载
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
			template = orderMap.get(ordercode);
		}else{
			template = groupTemplate.getTemplate(ordercode);
		}
		return template;
	}

//	public int apply(String orderCode, String phoneNo, String imsi, String ssInfo1, String ssInfo2,
//			String ssInfo3) {
//		final String data = createData(orderCode, phoneNo, imsi, ssInfo1, ssInfo2, ssInfo3);
//		return retry(data, retryCount);
//	}
	/**
	 * 执行查询指令， 并解析结果到 out_tpl 中
	 * @param ordercode  原指令代码
	 * @param qparam     原指令参数
	 * @param out_tpl       解析结果加到此模板中
	 */
	private void executeQuery(String ordercode, CmdDataAck qparam, Template out_tpl){
		String queryOrder = queryorders.get(ordercode);
		String result = null;
		if(queryOrder != null){ //如果定义了查询指令， 则通过查询指令获得结果
			LOGGER.debug("--query order:{} for org_order:{}", queryOrder, ordercode);
			Template template = findTemplate(queryOrder);
			if(template == null){
				LOGGER.error("--not found query order:{}", queryOrder);
				return;
			}
			if(addProp != null)
				template.binding(addProp);
			template.binding("phoneNo", qparam.phone_no);
			template.binding("imsi",    qparam.imsi_no);
			template.binding("ssInfo1", qparam.ss_info1);
			template.binding("ssInfo2", qparam.ss_info2);
			template.binding("ssInfo3", qparam.ss_info3);
			String cmdstr = template.render();
			result = retry(cmdstr, retryCount);
			if(result == null || result.length() < 20){
				LOGGER.error("query order {} failed: {}", queryOrder, result);
				return;
			}
		}else{ //否则从上次的结果中获取
			result = last_reply;
		}
		
		// 从查询指令的结果中解析参数出来， 传入到out_tpl中
		for(QueryItem q: queryparams.get(ordercode)){
				Matcher matcher = q.pattern.matcher(result);
				String v = null;
				if(matcher != null && matcher.find())
					v = matcher.group(1);
				else{
					LOGGER.error("query param {} not found, abort command", q.name);
					return;
				}
				out_tpl.binding(q.name, v);
				LOGGER.debug("key[{}] value[{}]", q.name, v);
			}
	}
	
	/**
	 * 删除结果串中的某个namespace, 以便做结果匹配， 如  xmlns:m="http://www.chinamobile.com/IMS/VoLTEAS/"
	 *      <m:ResultCode>0</m:ResultCode>  ==>  <ResultCode>0</ResultCode>
	 * @param result
	 * @param ns
	 * @return
	 */
	private String removeNS(String result){ //, String ns){
//		String regstr = "xmlns:(\\w+)=\\\""+ns+"\\\"";
//		Pattern pattern = Pattern.compile(regstr);
		Matcher matcher = ns2rmv.matcher(result);
		String m = null;
		if(matcher != null && matcher.find()){
			m = matcher.group(1);
		}else{
			return result;
		}
		
		String s = result.replaceAll("<"+m+"\\:", "<");
		s = s.replaceAll("</"+m+"\\:", "<");
		return s;
	}
	
	private String ph2domain(String s){
		StringBuffer b = new StringBuffer();
		for(int i=s.length(); i>0; i--)
			b.append(s.charAt(i-1)).append('.');
		return b.toString();
	}

	/**
	 * 生成要执行的指令内容
	 * @param cmd
	 * @return
	 */
	private String createData(CmdDataAck cmd) {
		Template template = findTemplate(cmd.ordercode);
		if(template == null)
			return null;
		if(addProp != null)
			template.binding(addProp);
		
		if(queryparams.containsKey(cmd.ordercode)  ){
			// 需要使用查询指令结果来 获取参数
			executeQuery(cmd.ordercode, cmd, template);
		}
		if(IPHEX_TO_INT!=null && IPHEX_TO_INT.contains(cmd.ordercode)){
			final String intIP = IP_HEX_to_INT(cmd.ss_info2);
			cmd.ss_info2=intIP;
		}
		if(orderMap.containsKey("__enum__")){
			// 对于enum指令， 需要对手机号码做域名转换
			template.binding("phoneNoDomain", ph2domain(cmd.phone_no));
			template.binding("phoneHeadDomain", ph2domain(cmd.phone_no.substring(0, 7)));
		}
		template.binding("phoneNo", cmd.phone_no);
		template.binding("imsi",    cmd.imsi_no);
		template.binding("ssInfo1", cmd.ss_info1);
		template.binding("ssInfo2", cmd.ss_info2);
		template.binding("ssInfo3", cmd.ss_info3);
		return template.render();
	}

	public int apply(CmdDataAck cmd, CmdDataReq ret) {
		if(ptran != null)
			ptran.tran(cmd);
		final String data = createData(cmd);
		int retn = -1;
		if(data == null)
			retn = 1002;
		else{
			String result = retry(data, retryCount);
			retn = parseResultCode(result);
		}
		if(ret != null){
			ret.retn = retn;
			ret.stream_id = cmd.stream_id;
			ret.ordercode = cmd.ordercode;
			ret.phone_no = cmd.phone_no;
			ret.info = String.valueOf(resultDesc);
		}
		return retn;
	}
	
	private String resultDesc = null;
	private Pattern resultDescPattern = Pattern.compile("<ResultDesc>(.+)<\\/ResultDesc>");
	private int parseResultCode(String data) {
		if(data == null)
			return 7713;
		if(data.length() == 4)
			return Integer.parseInt(data);
		final int lIndex = data.indexOf(startTag);
		if (lIndex != -1) {
			final int rIndex = data.indexOf(endTag, lIndex);
			if (rIndex != -1) {
				final String resultCode = data.substring(lIndex + startTag.length(), rIndex);
				try {
					// <ResultDesc> </ResultDesc>
					Matcher matcher = resultDescPattern.matcher(data);
					resultDesc = null;
					if(matcher != null && matcher.find())
						resultDesc = matcher.group(1);
					return Integer.parseInt(resultCode) % 10000;
				} catch (final NumberFormatException e) {
					return 7712;
				}
			}
		}
		return 7710;
	}

	private String retry(String data, int retry, boolean logIt) {
		CloseableHttpResponse httpResponse = null;
		HttpPost httpPost = null;
		try {
			httpPost = new HttpPost(url);
			httpPost.setHeader("SOAPAction", "");
			httpPost.setEntity(new StringEntity(data, TEXT_XML));
			if (logIt) {
				LOGGER.info("send = [{}]", data);
			}
			httpResponse = getHttpClient().execute(httpPost);
			StatusLine sl = httpResponse.getStatusLine();
			if(sl.getStatusCode() != 200){
				LOGGER.error("order execute HTTP failed {} {} {}", new Object[]{sl.getStatusCode(), sl.getReasonPhrase(), url});
				return "7711";
			}
			data = EntityUtils.toString(httpResponse.getEntity(), "UTF-8");
			if(ns2rmv != null){
				data = removeNS(data); //移除结果串中的 ns
			}
			if (logIt) {
				LOGGER.info("receive = [{}]", data);
			}
			last_reply = data;
			return data;
			
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
		return "7710";
	}

	/**
	 * @param data2
	 * @param i
	 * @return
	 */
	private String retry(String data, int retry) {
		return retry(data, retry, true);
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
	
	public Set<String> getAllOrderCode(){
		return orderMap.keySet();
	}
}
