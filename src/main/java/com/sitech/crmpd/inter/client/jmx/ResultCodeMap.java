package com.sitech.crmpd.inter.client.jmx;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import java.io.InterruptedIOException;
import java.util.LinkedList;
import java.util.List;

/**
 * Created by guanyf on 2016/5/31.
 */
public class ResultCodeMap {
    private static class CodeItem {
        String val;
        int code;
        CodeItem(String c, String v) {
            try {
                code = Integer.parseInt(c);
            }catch(Exception ex){ex.printStackTrace();}
            val = v;
        }
    }
    private List<CodeItem> l;
    public ResultCodeMap(String fname) {
        l = new LinkedList<CodeItem>();
        try{
            BufferedReader bf = new BufferedReader(new FileReader(fname));
            while(true) {
                String line = bf.readLine();
                if (line == null ) break;
                line = line.trim();
                if (line.length() == 0)
                    continue;
                if(line.charAt(0) == '#')
                    continue;
                int p = line.indexOf(' ');
                if( p <= 0) continue;
                String c = line.substring(0, p).trim();
                String v = line.substring(p+1).trim();
                l.add(new CodeItem(c, v));
            }
        } catch(IOException ex){

        }
    }
    public int mapcode(String data) {
        for(CodeItem ci: l) {
            if(data.indexOf(ci.val) >= 0)
                return ci.code;
        }
        return -1;
    }
}
