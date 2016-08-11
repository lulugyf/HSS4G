package test;

import javax.servlet.http.HttpServlet;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * Created by guanyf on 2016/8/10.
 */
public class MyBaseServlet extends HttpServlet {

    protected String readBody(InputStream pin) throws IOException {
        BufferedReader bf = new BufferedReader(new InputStreamReader(pin));
        StringBuilder sb = new StringBuilder();
        while(true) {
            String line = bf.readLine();
            if(line == null) break;
            sb.append(line);
        }
        return sb.toString();
    }

    protected void sleep(long milis) {
        try {
            Thread.sleep(milis);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
