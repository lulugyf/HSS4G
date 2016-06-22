package com.sitech.crmpd.inter.client.jmx.func;

import com.sitech.crmpd.inter.common.CmdDataAck;
import org.beetl.core.Configuration;
import org.beetl.core.GroupTemplate;
import org.beetl.core.Template;
import org.beetl.core.resource.StringTemplateResourceLoader;
import org.slf4j.Logger;

import java.io.IOException;
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
    private Logger LOGGER;
    private GroupTemplate gt;

    public QueryParse(Logger log) {
        this.LOGGER = log;
        try {
            StringTemplateResourceLoader resourceLoader = new StringTemplateResourceLoader();
            Configuration cfg = Configuration.defaultConfiguration();
            gt = new GroupTemplate(resourceLoader, cfg);
        }catch(IOException ex){}
    }

    // 查询结果中提取参数实现
    static class QueryItem{
        String name;
        Pattern pattern;
        Template template;
        QueryItem(String n){ name = n; }
    }
    private Map<String, List<QueryItem>> queryparams = new HashMap<String, List<QueryItem>>(); //查询参数对应需要解析的正则表达式列表
    private Map<String, String> queryorders = new HashMap<String, String>(); //ordercode 对应的查询order

    @SuppressWarnings("unchecked")
    public void addQP(String key, Object value) {
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
                    QueryItem qi = new QueryItem(varname);
                    if(varvalue.indexOf('$') >= 0){ //匹配字符串中有变量， 需要在运行期间来处理
                        qi.template = gt.getTemplate(varvalue);
                    }else {
                        qi.pattern = Pattern.compile(varvalue);
                    }

                    l.add(qi);
                }
                //LOGGER.debug("load {} query {} = {}", new Object[]{key, s.substring(0,p), varvalue});
            }
        }else if(value instanceof String){
//			LOGGER.error("invalid QP config: {}, must be a list", key);
            String s = (String)value;
            int p = s.indexOf('=');
            String varname = s.substring(0,p);
            String varvalue = s.substring(p+1);
            QueryItem qi = new QueryItem(varname);
            if(varvalue.indexOf('$') >= 0){ //匹配字符串中有变量， 需要在运行期间来处理
                qi.template = gt.getTemplate(varvalue);
            }else {
                qi.pattern = Pattern.compile(varvalue);
            }
            l.add(qi);
        }
        if(l.size() > 0 && queryorders.containsKey(key))
            queryparams.put(key, l);
    }

    public String queryOrder(String ordercode) {
        return queryorders.get(ordercode);
    }

    public void parseQueryResult(String ordercode, String result, Template out_tpl, CmdDataAck qparam) {
        // 从查询指令的结果中解析参数出来， 传入到out_tpl中
        for(QueryItem q: queryparams.get(ordercode)){
            Matcher matcher = null;
            if(q.pattern != null) {
                matcher = q.pattern.matcher(result);
            }else{
                String s = qparam.render(q.template, null);
                matcher = Pattern.compile(s).matcher(result);
            }
            String v = "";
            if (matcher != null && matcher.find())
                v = matcher.group(1);
            else {
                LOGGER.error("query param {} not found, abort command", q.name);
            }
            out_tpl.binding(q.name, v);
            LOGGER.debug("key[{}] value[{}]", q.name, v);
        }
    }
}
