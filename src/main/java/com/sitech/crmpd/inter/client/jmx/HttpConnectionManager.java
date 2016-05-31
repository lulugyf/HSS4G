package com.sitech.crmpd.inter.client.jmx;

import org.apache.http.Consts;
import org.apache.http.StatusLine;
import org.apache.http.client.config.RequestConfig;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.config.SocketConfig;
import org.apache.http.conn.routing.HttpRoute;
import org.apache.http.entity.ContentType;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClientBuilder;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.impl.conn.PoolingHttpClientConnectionManager;
import org.apache.http.pool.PoolStats;
import org.apache.http.util.EntityUtils;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

/**
 * Created by guanyf on 2016/5/31.
 */
public class HttpConnectionManager {
    private static int timeout1 = 60000;
    private static HttpClientBuilder builder = null;
    private static PoolingHttpClientConnectionManager cm;
    ContentType TEXT_XML = ContentType.create("text/xml", Consts.UTF_8);
    static {
        final SocketConfig config = SocketConfig.custom().setSoTimeout(timeout1).build();

        cm = new PoolingHttpClientConnectionManager(20, TimeUnit.SECONDS);
        int max = 100;
        int maxtotal = 200;
        String s = System.getProperty("MAXURLCONN");
        if (s != null ){
            try{
                max = Integer.parseInt(s);
            }catch(Exception ex){ex.printStackTrace();}
        }
        s = System.getProperty("MAXURLCONNTOTAL");
        if (s != null ){
            try{
                maxtotal = Integer.parseInt(s);
            }catch(Exception ex){ex.printStackTrace();}
        }

        cm.setMaxTotal(maxtotal);
        cm.setDefaultMaxPerRoute(max);
        cm.setDefaultSocketConfig(config);
        builder = HttpClients
                .custom()
                .setConnectionManager(cm)
                .setDefaultSocketConfig(config)
                .setDefaultRequestConfig(
                        RequestConfig.custom().setConnectTimeout(timeout1)
                                .setConnectionRequestTimeout(timeout1).setSocketTimeout(timeout1)
                                .build());
//        builder.setConnectionTimeToLive(20, TimeUnit.SECONDS);
    }

    public synchronized static CloseableHttpClient getHttpClient() {
        return builder.build();
    }

    public void test() {
        String url = "http://172.21.3.187:8181/list";
        String data = "hello";
        CloseableHttpResponse httpResponse = null;
        CloseableHttpClient httpClient = null;
        HttpPost httpPost = null;
        try {
            httpPost = new HttpPost(url);
            httpPost.setHeader("SOAPAction", "");
            httpPost.setEntity(new StringEntity(data, TEXT_XML));

            httpClient = getHttpClient();
            httpResponse = httpClient.execute(httpPost);
            StatusLine sl = httpResponse.getStatusLine();
            if(sl.getStatusCode() == 500){

            }else if(sl.getStatusCode() != 200){
                System.out.printf("order execute HTTP failed %s %s %s\n", new Object[]{sl.getStatusCode(), sl.getReasonPhrase(), url});
                return ;
            }
            data = EntityUtils.toString(httpResponse.getEntity(), "UTF-8");

//             System.out.printf("receive = [%s]\n", data);
        } catch (final IOException e) {
            if (httpPost != null) {
                httpPost.abort();
            }
            if (httpClient != null) {
                try {
                    httpClient.close();
                } catch (final IOException ee) {}
            }
        } finally {
            if (httpResponse != null) {
                try {
                    httpResponse.close();
                } catch (final IOException e) {}
            }
        }
    }

    public static String cmStats() {
        StringBuffer sb = new StringBuffer();
        for(HttpRoute hr : cm.getRoutes()) {
            sb.append("\n==="+hr.toString() + "==\n");
            PoolStats ps = cm.getStats(hr);
            sb.append(ps.toString());
        }
        return sb.toString();
    }

    public static void main(String[] args) throws Exception{
        final HttpConnectionManager c = new HttpConnectionManager();
        for(int i=0; i< 15; i++ ) {
            new Thread() {
                public void run(){
                    for(int i=0; i<100; i++){
                        c.test();
                        try {
                            Thread.sleep(100);
                        }catch (InterruptedException e){}
                    }
                }
            }.start();

        }

        while(true) {
            try {
                Thread.sleep(1000);
            }catch (InterruptedException e){}

            c.test();
            System.out.println(c.cmStats());
        }
    }
}
