package test;

import java.io.IOException;

import org.apache.http.Consts;
import org.apache.http.StatusLine;
import org.apache.http.client.config.RequestConfig;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.config.SocketConfig;
import org.apache.http.entity.ContentType;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClientBuilder;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.impl.conn.PoolingHttpClientConnectionManager;
import org.apache.http.util.EntityUtils;

public class T1 {

	private static int timeout = 10000;
	private static final ContentType TEXT_XML = ContentType.create("text/xml", Consts.UTF_8);
	
	private static HttpClientBuilder builder = null;
	static {
		final SocketConfig config = SocketConfig.custom().setSoTimeout(timeout).build();

		final PoolingHttpClientConnectionManager cm = new PoolingHttpClientConnectionManager();
		final int max = 100;
		cm.setMaxTotal(max);
		cm.setDefaultMaxPerRoute(max);
		cm.setDefaultSocketConfig(config);
		builder = HttpClients
				.custom()
				.setConnectionManager(cm)
				.setDefaultSocketConfig(config)
				.setDefaultRequestConfig(
						RequestConfig.custom().setConnectTimeout(timeout)
						.setConnectionRequestTimeout(timeout).setSocketTimeout(timeout)
						.build());
	}
	
	private synchronized static CloseableHttpClient getHttpClient() {
			return builder.build();
	}
	
	
	private static String retry(String url, String data, int retry, boolean logIt) {
		CloseableHttpResponse httpResponse = null;
		HttpPost httpPost = null;
		try {
			httpPost = new HttpPost(url);
			httpPost.setHeader("SOAPAction", "");
			httpPost.setEntity(new StringEntity(data, TEXT_XML));
			if (logIt) {
				System.out.printf("send = [%s]\n", data);
			}
			httpResponse = getHttpClient().execute(httpPost);
			StatusLine sl = httpResponse.getStatusLine();
			if(sl.getStatusCode() == 500){
				
			}else if(sl.getStatusCode() != 200){
				System.out.printf("order execute HTTP failed {} {} {}\n", new Object[]{sl.getStatusCode(), sl.getReasonPhrase(), url});
				return "7711";
			}
			data = EntityUtils.toString(httpResponse.getEntity(), "UTF-8");

			return data;
		} catch (final IOException e) {
			if (httpPost != null) {
				httpPost.abort();
			}
			
			if (retry > 0) {
				return retry(url, data,  retry - 1, logIt);
			}else{
				e.printStackTrace();
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
	
	class V extends Thread{
		private String url;
		private String data;
		private String tag;
		public V(String url, String data, String tag){
			this.url = url;
			this.data = data;
			this.tag = tag;
		}
		public void run(){
			for(int i=0; i<20; i++){
				System.out.println(tag+":: " +retry(url, data, 0, false));
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
	
	public void test(){
		String url = "http://localhost:9191/test1";
		String data = "hello, this is post body";
		for(int j=10; j<21; j++){
			new V(url, data, String.valueOf(j)).start(); 
		}
	}
	
	public static void main(String[] args) {
		T1 t = new T1();
		t.test();
	}
}
