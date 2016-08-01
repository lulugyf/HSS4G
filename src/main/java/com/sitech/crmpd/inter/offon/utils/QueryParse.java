package com.sitech.crmpd.inter.offon.utils;

import com.sitech.crmpd.inter.offon.mgr.CmdDataAck;
import org.beetl.core.Configuration;
import org.beetl.core.GroupTemplate;
import org.beetl.core.Template;
import org.beetl.core.resource.StringTemplateResourceLoader;
import org.slf4j.Logger;

import java.io.IOException;
import java.text.Normalizer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by guanyf on 2016/6/20.
 */
public class QueryParse {
    private Logger log;
    private Formater fmt = new Formater();

    public QueryParse(Logger log) {
        this.log = log;
    }

    // 查询结果中提取参数实现
    static class QueryOrder{
        String ordercode;
        String queryorder; //要提前执行的查询指令代码
        List<String[]> vars = new ArrayList<String[]>();
    }
    private Map<String, QueryOrder> qorders = new HashMap<String, QueryOrder>(); //查询参数对应需要解析的正则表达式列表

    @SuppressWarnings("unchecked")
    public void addQP(String key, Object value) {
        String ordercode = key.substring(2);
        QueryOrder qo = new QueryOrder();
        qo.ordercode = ordercode;
        if(value instanceof List){
            for(String s: (List<String>)value){
                int p = s.indexOf('=');
                String varname = s.substring(0,p);
                String varvalue = s.substring(p+1);
                if("query_order".equals(varname)){
                    qo.queryorder = varvalue;
                }else{
                    qo.vars.add(new String[]{varname, varvalue});
                }
                log.debug("load query order {} = {}", ordercode, s);
            }
        }else{
            log.error("invalid query order config: {} {}", key, value);
        }
        qorders.put(ordercode, qo);
    }
    public int size() { return qorders.size() ; }

    public String queryOrder(String ordercode) {
        if(qorders.containsKey(ordercode))
            return qorders.get(ordercode).queryorder;
        return null;
    }

    /**
     * parse query_result into vars
     * @param cmd  原始指令参数
     * @param query_result  查询指令的结果
     * @param vars 用于保存解析结果
     */
    public void parseQueryResult(CmdDataAck cmd, String query_result, Map<String, String> vars) {
        // 从查询指令的结果中解析参数出来， 传入到out_tpl中
        QueryOrder qo = qorders.get(cmd.ordercode);
        cmd.varsPut(vars);
        String vname, vvalue;
        for(String[] v: qo.vars){
            vname = v[0];
            vvalue = v[1];
            vvalue = fmt.format(vvalue, vars);
            Pattern p = Pattern.compile(vvalue);
            Matcher matcher = p.matcher(query_result);

            if (matcher != null && matcher.find())
                vars.put(vname, matcher.group(1));
            else {
                vars.put(vname, "");
                log.warn("query param {} not found", vname);
            }
        }
    }

}
