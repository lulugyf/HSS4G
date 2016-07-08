package com.sitech.crmpd.inter.client.jmx.func;


/**
 * Created by guanyf on 2016/7/6.
 */

import com.sitech.crmpd.inter.client.jmx.HttpConnectionManager;
import org.apache.http.Consts;
import org.apache.http.StatusLine;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.ContentType;
import org.apache.http.entity.StringEntity;
import org.apache.http.util.EntityUtils;
import org.slf4j.Logger;
import org.yaml.snakeyaml.Yaml;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.concurrent.*;

public class HttpExecutor {
    private ThreadPoolExecutor executorPool;

    private static HttpExecutor executor = null;
    public static HttpExecutor getInstance() {
        if(executor != null)
            return executor;
        String conf = System.getProperty("CONF", System.getenv("ETCDIR") + "/hss.yaml");
        Yaml y = new Yaml();
        Map m = null;
        try {
            m = (Map)y.load(new FileInputStream(new File(conf)));
        } catch (FileNotFoundException e) {
            return null;
        }
        Map<String, Integer> props = null;

        if(m.containsKey("threadPool")) {
            try {
                Map m1 = (Map) m.get("threadPool");
                props = new HashMap<String, Integer>();
                for (String k : new String[]{"corePoolSize", "maxPoolSize", "idleTimeSeconds", "poolQueueSize"}) {
                    if (m1.containsKey(k))
                        props.put(k, (Integer) m1.get(k));
                }
            }catch(Exception e){
                props = null;
            }
        }
        executor = new HttpExecutor(props);
        return executor;
    }
    private static int getInt(Map<String, Integer> m, String key, int def){
        if(m == null)
            return def;
        if(m.containsKey(key))
            return m.get(key);
        return def;
    }

    private HttpExecutor(Map<String, Integer> props) {
        int corePoolSize = getInt(props, "corePoolSize", 20); // default 20
        int maxPoolSize = getInt(props, "maxPoolSize", 200);
        int idleTimeSeconds = getInt(props, "idleTimeSeconds", 10); //线程闲置回收时间
        int poolQueueSize = getInt(props, "poolQueueSize", 50); //线程池队列的长度

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
    }

    public void execute(TaskObj obj, ArrayBlockingQueue<TaskObj> reply) {
        executorPool.execute(new WorkerThread(obj, reply));
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
}

class WorkerThread implements Runnable {

    private static final ContentType TEXT_XML = ContentType.create("text/xml", Consts.UTF_8);
    private TaskObj t;
    private ArrayBlockingQueue<TaskObj> reply;
    private Logger LOGGER;

    public WorkerThread(TaskObj s, ArrayBlockingQueue<TaskObj> reply){
        this.t = s;
        this.reply = reply;
        LOGGER = t.logger;
    }

    @Override
    public void run() {
        System.out.println(Thread.currentThread().getName()+" Start. Command = "+t);
        processCommand();
        System.out.println(Thread.currentThread().getName()+" End.");
    }

    private void processCommand() {
        try {
            t.response_str = retry(t.request_str, 3);
        } catch (Exception e) {
            LOGGER.error("retry failed", e);
            e.printStackTrace();
            t.response_str = "7713";
        }

        try{
            reply.put(t);
        }catch(Exception e) {

        }
    }
    private String retry(String data, int retry, boolean logIt) {
        CloseableHttpResponse httpResponse = null;
        HttpPost httpPost = null;
        String url = t.url;
        try {
            httpPost = new HttpPost(url);
            httpPost.setHeader("SOAPAction", "");
            httpPost.setEntity(new StringEntity(data, TEXT_XML));
            if(LOGGER.isDebugEnabled())
                LOGGER.debug("==to: {}", url);
            if (logIt) {
                LOGGER.info("send = [{}]", data);
            }
            httpResponse = HttpConnectionManager.getHttpClient().execute(httpPost);
            StatusLine sl = httpResponse.getStatusLine();
            if(sl.getStatusCode() == 500){
                LOGGER.warn("http response: 500");
            }else if(sl.getStatusCode() != 200){
                LOGGER.error("order execute HTTP failed {} {} {}", new Object[]{sl.getStatusCode(), sl.getReasonPhrase(), url});
                return "7711";
            }
            data = EntityUtils.toString(httpResponse.getEntity(), "UTF-8");

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
            if (httpResponse != null) {
                try {
                    httpResponse.close();
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

