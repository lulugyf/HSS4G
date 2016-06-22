package com.sitech.crmpd.inter.client.jmx.func;

import com.sitech.crmpd.inter.common.Constants;
import org.slf4j.Logger;

import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by guanyf on 2016/6/21.
 * 用于解析返回结果的code和desc
 * 使用以下配置:
 return.desc.pattern: "<ResultDesc>(.+)<\/ResultDesc>;<faultstring>(.+)</faultstring>"
 return.code.pattern: "<ResultCode>(.+)<\/ResultCode>"
 start.tag: <ResultCode>
 end.tag: </ResultCode>
 */
public class ResultCodeParse {
    private String resultDesc = null;
    private ResultCodeMap resultmap = null;
    private Pattern resultDescPattern = Pattern.compile("<ResultDesc>(.+)<\\/ResultDesc>");
    private Logger LOGGER;
    private String startTag, endTag;
    private Pattern codePattern;
    private List<Pattern> descPatterns;


    public String getDesc() { return resultDesc; }
    public ResultCodeParse(Properties properties, ResultCodeMap resultmap, Logger log) {
        this.resultmap = resultmap;
        LOGGER = log;

        startTag = properties.getProperty(Constants.START_TAG_KEY, "<ResultCode>");
        endTag = properties.getProperty(Constants.END_TAG_KEY, "</ResultCode>");
        if(properties.containsKey("return.code.pattern")) {
            codePattern = Pattern.compile(properties.getProperty("return.code.pattern"));
        }

        //获取desc描述的pattern串
        if(properties.containsKey("return.desc.pattern")){
            String s = properties.getProperty("return.desc.pattern");
            String[] x = s.split(";");
            descPatterns = new ArrayList<Pattern>(x.length);
            for(String ps: x){
                descPatterns.add(Pattern.compile(ps));
            }
        }
    }
    private String match(Pattern pattern, String data) {
        Matcher matcher = pattern.matcher(data);
        String m = null;
        if(matcher != null && matcher.find()){
            m = matcher.group(1);
        }
        return m;
    }

    private void matchDesc(String data) {
        if(descPatterns == null)
            return;
        for(Pattern p: descPatterns){
            String s = match(p, data);
            if(s != null){
                resultDesc = s;
                break;
            }
        }
    }

    public int parseCode(String data) {
        if(data == null)
            return 7713;
        if(data.length() == 4)
            return Integer.parseInt(data);

        matchDesc(data);

        if(resultmap != null) {
            int rcode = resultmap.mapcode(data);
            if (rcode != -1) {
                LOGGER.debug("found map-code: {}", rcode);
                return rcode;
            }
        }
        if(codePattern != null ) {
            String resultCode = match(codePattern, data);
            try {
                return Integer.parseInt(resultCode) % 10000;
            } catch (final NumberFormatException e) {
                return 7712;
            }
        }

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
        return 7711;

    }
}
