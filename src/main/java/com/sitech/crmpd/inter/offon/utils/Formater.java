package com.sitech.crmpd.inter.offon.utils;

import java.util.Map;

/**
 * Created by guanyf on 7/31/2016.
 */
public final class Formater {
    public final String format(String s, Map<String, String> vars, boolean keep_var) {
        StringBuilder sb = new StringBuilder();
        int p, p1;
        String vname;
        while(true) {
            p = s.indexOf("${");
            if (p < 0){
                sb.append(s); break;
            }
            p += 2;
            p1 = s.indexOf('}', p);
            if(p1 < 0) {
                sb.append(s); break;
            }
            sb.append(s.substring(0, p-2));
            vname = s.substring(p, p1);
            if(vars.containsKey(vname))
                sb.append(vars.get(vname));
            else if(keep_var) {
                sb.append("${").append(vname).append('}');
            }
            s = s.substring(p1+1);
        }
        return sb.toString();
    }
    public final String format(String s, Map<String, String> vars) {
        return format(s, vars, false);
    }
}
