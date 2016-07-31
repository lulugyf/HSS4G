package com.sitech.crmpd.inter.offon.exec;


/**
 * Created by guanyf on 2016/7/6.
 * 单个URL的执行线程池
 */

import com.sitech.crmpd.inter.offon.utils.OrderTask;
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
import org.slf4j.Logger;
import org.yaml.snakeyaml.Yaml;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.*;

/**
 * http执行线程池
 *
 * 配置文件格式：
 * pool.1:
 *   coreSize: 10
 *   maxSize: 50
 *   idleTimeSeconds: 120
 *   queueSize: 50
 *
 *   url: http://?????
 *   socketTimeoutSeconds: 60
 *   timeToLiveSeconds: 120
 * pool.2:
 *   ...
 */
public class HttpExecutor {
    private ThreadPoolExecutor executorPool;

    private static HttpExecutor executor = null;
    private static ConcurrentHashMap<String, HttpExecutor> executors = new ConcurrentHashMap<String, HttpExecutor>();
    final private ContentType TEXT_XML = ContentType.create("text/xml", Consts.UTF_8);


    public static HttpExecutor getInstance(String url) {
        return executors.get(url);
    }

    static {
        init();
    }
    private static void init() {
        String conf = System.getProperty("SOAPPOOLCONF");
        Yaml y = new Yaml();
        Map m = null;
        try {
            m = (Map)y.load(new FileInputStream(new File(conf)));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return;
        }
        for(Object k: m.keySet()) {
            String key = (String)k;
            if(!key.startsWith("pool."))
                continue;
            Object o = m.get(key);
            if(o instanceof Map) {
                Map m1 = (Map)o;
                int corePoolSize = (Integer)m1.get("coreSize");
                int maxPoolSize = (Integer)m1.get("maxSize");
                int idleTimeSeconds = (Integer)m1.get("idleTimeSeconds");
                int poolQueueSize = (Integer)m1.get("queueSize");
                String url = (String)m1.get("url");

                HttpExecutor e = new HttpExecutor(corePoolSize, maxPoolSize, idleTimeSeconds, poolQueueSize, url);
                int socketTimeoutSeconds = (Integer)m1.get("socketTimeoutSeconds");
                int timeToLiveSeconds = (Integer)m1.get("timeToLiveSeconds");
                e.initHttp(socketTimeoutSeconds, timeToLiveSeconds, maxPoolSize);

                executors.put(url, e);
            }
        }

    }


    private HttpClientBuilder builder = null;
    private PoolingHttpClientConnectionManager cm;

    private void initHttp(int socketTimeoutSeconds, int timeToLiveSeconds, int maxConn) {
        int timeout1 = socketTimeoutSeconds * 1000;
        final SocketConfig config = SocketConfig.custom().setSoTimeout(timeout1).build();
        cm = new PoolingHttpClientConnectionManager(timeToLiveSeconds, TimeUnit.SECONDS);

        cm.setMaxTotal(maxConn);
        cm.setDefaultMaxPerRoute(maxConn);
        cm.setDefaultSocketConfig(config);

        builder = HttpClients
                .custom()
                .setConnectionManager(cm)
                .setDefaultSocketConfig(config)
                .setDefaultRequestConfig(
                        RequestConfig.custom().setConnectTimeout(timeout1)
                                .setConnectionRequestTimeout(timeout1).setSocketTimeout(timeout1)
                                .build());
    }

    protected synchronized CloseableHttpClient getHttpClient() {
        return builder.build();
    }
    protected String url;

    private HttpExecutor(int corePoolSize, int maxPoolSize, int idleTimeSeconds, int poolQueueSize, String url) {


        RejectedExecutionHandlerImpl rejectionHandler = new RejectedExecutionHandlerImpl();
        //Get the ThreadFactory implementation to use
        ThreadFactory threadFactory = Executors.defaultThreadFactory();
        //creating the ThreadPoolExecutor
        executorPool = new ThreadPoolExecutor(corePoolSize, maxPoolSize, idleTimeSeconds, TimeUnit.SECONDS,
                new ArrayBlockingQueue<Runnable>(poolQueueSize), threadFactory, rejectionHandler);
        //start the monitoring thread
        MyMonitorThread monitor = new MyMonitorThread(executorPool, 3);
        Thread monitorThread = new Thread(monitor);
        monitorThread.start();
        this.url = url;
    }

    public void execute(OrderTask task, ArrayBlockingQueue<OrderTask> reply) {
        executorPool.execute(new WorkerThread(task, reply, this));
    }


    public static void main(String args[]) throws InterruptedException{
        //RejectedExecutionHandler implementation
        RejectedExecutionHandlerImpl rejectionHandler = new RejectedExecutionHandlerImpl();
        //Get the ThreadFactory implementation to use
        ThreadFactory threadFactory = Executors.defaultThreadFactory();
        //creating the ThreadPoolExecutor
        ThreadPoolExecutor executorPool = new ThreadPoolExecutor(2, 4, 10, TimeUnit.SECONDS, new ArrayBlockingQueue<Runnable>(2), threadFactory, rejectionHandler);
        //start the monitoring thread
        MyMonitorThread monitor = new MyMonitorThread(executorPool, 3);
        Thread monitorThread = new Thread(monitor);
        monitorThread.start();
        //submit work to the thread pool
        for(int i=0; i<10; i++){
            //executorPool.execute(new WorkerThread("cmd"+i));
        }

        Thread.sleep(30000);
        //shut down the pool
        executorPool.shutdown();
        //shut down the monitor thread
        Thread.sleep(5000);
        monitor.shutdown();

    }

    private int getInt(Map<String, String> m, String key, int def){
        if(m == null)
            return def;
        if(m.containsKey(key))
            return Integer.parseInt(m.get(key));
        return def;
    }
}

class WorkerThread implements Runnable {

    private static final ContentType TEXT_XML = ContentType.create("text/xml", Consts.UTF_8);
    protected OrderTask task;
    private ArrayBlockingQueue<OrderTask> reply;
    private Logger LOGGER;
    private HttpExecutor executor;

    public WorkerThread(OrderTask s, ArrayBlockingQueue<OrderTask> reply, HttpExecutor executor){
        this.task = s;
        this.reply = reply;
        this.executor = executor;
        LOGGER = task.logger;
    }

    @Override
    public void run() {
        System.out.println(Thread.currentThread().getName()+" Start. Command = "+ task);
        processCommand();
        System.out.println(Thread.currentThread().getName()+" End.");
    }

    private void processCommand() {
        try {
            task.response_str = retry(task.request_str, 3);
        } catch (Exception e) {
            LOGGER.error("retry failed", e);
            e.printStackTrace();
            task.response_str = "7713";
        }

        try{
            reply.put(task);
        }catch(Exception e) {

        }
    }
    private String retry(String data, int retry, boolean logIt) {
        CloseableHttpResponse http = null;
        HttpPost httpPost = null;
        String url = executor.url;
        try {
            httpPost = new HttpPost(url);
            httpPost.setHeader("SOAPAction", "");
            httpPost.setEntity(new StringEntity(data, TEXT_XML));
            if(LOGGER.isDebugEnabled())
                LOGGER.debug("==to: {}", url);
            if (logIt) {
                LOGGER.info("send = [{}]", data);
            }
            http = executor.getHttpClient().execute(httpPost);
            StatusLine sl = http.getStatusLine();
            if(sl.getStatusCode() == 500){
                LOGGER.warn("http response: 500");
            }else if(sl.getStatusCode() != 200){
                LOGGER.error("order execute HTTP failed {} {} {}", new Object[]{sl.getStatusCode(), sl.getReasonPhrase(), url});
                return "7711";
            }
            data = EntityUtils.toString(http.getEntity(), "UTF-8");

            if (logIt) {
                LOGGER.info("receive = [{}]", data);
            }
            return data;

        } catch (final IOException e) {
            if (httpPost != null) {
                httpPost.abort();
            }

            if (logIt) {
                LOGGER.error("", e);
            }
            if (retry > 0) {
                return retry(data, retry - 1);
            }
        } finally {
            if (http != null) {
                try {
                    http.close();
                } catch (final IOException e) {}
            }
        }
        return "7710";
    }

    /**
     * @param data
     * @param retry
     * @return
     */
    private String retry(String data, int retry) {
        return retry(data, retry, true);
    }


}

class RejectedExecutionHandlerImpl implements RejectedExecutionHandler {

    @Override
    public void rejectedExecution(Runnable r, ThreadPoolExecutor executor) {
        System.out.println(r.toString() + " is rejected");
    }

}

class MyMonitorThread implements Runnable
{
    private ThreadPoolExecutor executor;
    private int seconds;
    private boolean run=true;

    public MyMonitorThread(ThreadPoolExecutor executor, int delay)
    {
        this.executor = executor;
        this.seconds=delay;
    }
    public void shutdown(){
        this.run=false;
    }
    @Override
    public void run()
    {
        while(run){
            System.out.println(
                    String.format("[monitor] [%d/%d] Active: %d, Completed: %d, Task: %d, isShutdown: %s, isTerminated: %s",
                            this.executor.getPoolSize(),
                            this.executor.getCorePoolSize(),
                            this.executor.getActiveCount(),
                            this.executor.getCompletedTaskCount(),
                            this.executor.getTaskCount(),
                            this.executor.isShutdown(),
                            this.executor.isTerminated()));
            try {
                Thread.sleep(seconds*1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

    }
}

