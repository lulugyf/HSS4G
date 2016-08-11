package test;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by guanyf on 2016/8/2.
 >> 'POST /spg HTTP/1.1\r\nContent-Length: 693\r\nAccept-Encoding: gzip,deflate\r\nSoapaction: \r\nHost: 127.0.0.1:9111\r\nUser-Agent: Apache-HttpClient/4.4.1 (Java/1.6.0_31)\r\nConnection: close\r\nContent-Type: text/xml; charset=UTF-8\r\n\r\n\n<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:SOAP-ENC="http://\nschemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchem\na">\n<SOAP-ENV:Header xmlns:m="http://www.chinamobile.com/IMS/VoLTEAS/">\n <m:Authentication >\n  <m:Username>CDBOSS</m:Username>\n  <m:Password>7C20F57649F7A945E61E4D895455A7E5</m:Password>\n </m:Authentication>\n <m:MEName>CDVOLTETAS5BHW</m:MEName>\n <m:MessageID>1</m:MessageID>\n</SOAP-ENV:Header>\n<SOAP-ENV:Body xmlns:v="http://www.chinamobile.com/IMS/VoLTEAS/">\n<v:RPL_CFNR><v:IMPU>tel:+8615700240244</v:IMPU></v:RPL_CFNR>\n</SOAP-ENV:Body></SOAP-ENV:Envelope>'


 << 'HTTP/1.1 200 OK\r\nContent-Length: 688\r\nContent-Type: text/xml\r\n\r\n<?xml version="1.0" encoding="UTF-8"?>\n<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><SOAP-ENV:Header><MessageID xmlns="http://www.chinamobile.com/IMS/VoLTEAS/">1</MessageID></SOAP-ENV:Header><SOAP-ENV:Body><m:RPL_CFNRResponse xmlns:m="http://www.chinamobile.com/IMS/VoLTEAS/"><m:Result><m:ResultCode>10010505</m:ResultCode><m:ResultDesc>\xe7\xbd\x91\xe5\x85\x83\xe5\x91\xbd\xe4\xbb\xa4\xe6\x89\xa7\xe8\xa1\x8c\xe5\xa4\xb1\xe8\xb4\xa5\xef\xbc\x8c\xe8\xaf\xa6\xe7\xbb\x86\xe4\xbf\xa1\xe6\x81\xaf\xef\xbc\x9a\xe2\x80\x9c Result/ResultCode=22025;\n Result/ResultDesc=SPG_TO_ATS-\xe6\xb2\xa1\xe6\x9c\x89MMTelServices\xe6\x95\xb0\xe6\x8d\xae\xe3\x80\x82\n\xe2\x80\x9d\xe3\x80\x82</m:ResultDesc></m:Result></m:RPL_CFNRResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>'



 huawei hss:
 ====================
 2016-08-02 00:00:03.444 INFO  - send = [<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:hss="http:
 //www.chinamobile.com/HSS/">
 <soapenv:Header>
 <hss:UserName>BOSSSOAP</hss:UserName>
 <hss:PassWord>bosssoap</hss:PassWord>
 </soapenv:Header>
 <soapenv:Body>
 <MOD_LCK>
 <HLRSN>25</HLRSN>
 <ISDN>8615108413081</ISDN>
 <EPSLOCK>TRUE</EPSLOCK>
 </MOD_LCK>
 </soapenv:Body>
 </soapenv:Envelope>
 ]
 2016-08-02 00:00:03.462 INFO  - receive = [<?xml version='1.0' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/
 envelope/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><SOAP-ENV:Header><head
 er><sessionID>05mwELqgCZOKBHPr0uFXKzs9AWEYS6j6V7hMwcmV5sh0VrQgOtETipO03S28MxTo</sessionID></header></SOAP-ENV:Header><SOAP-ENV:Body>
 <MOD_LCKResponse><Result><ResultCode>0</ResultCode><ResultDesc>SUCCESS0002:已经存在，不需要更新</ResultDesc></Result></MOD_LCKRespon
 se></SOAP-ENV:Body></SOAP-ENV:Envelope>]

 huawei tas:
 ===================
 2016-08-02 00:01:02.537 INFO  - send = [<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:SOAP-ENC
 ="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001
 /XMLSchema">
 <SOAP-ENV:Header xmlns:m="http://www.chinamobile.com/IMS/VoLTEAS/">
 <m:Authentication >
 <m:Username>CDBOSS</m:Username>
 <m:Password>7C20F57649F7A945E61E4D895455A7E5</m:Password>
 </m:Authentication>
 <m:MEName>CDVOLTETAS5BHW</m:MEName>
 <m:MessageID>1</m:MessageID>
 </SOAP-ENV:Header>
 <SOAP-ENV:Body xmlns:v="http://www.chinamobile.com/IMS/VoLTEAS/">
 <v:SET_OWSBR>
 <v:IMPU>tel:+8615108326837</v:IMPU>
 <v:US>3</v:US>
 </v:SET_OWSBR>
 </SOAP-ENV:Body>
 </SOAP-ENV:Envelope>
 ]
 2016-08-02 00:01:02.775 INFO  - receive = [<?xml version="1.0" encoding="UTF-8"?>
 <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi
 ="http://www.w3.org/2001/XMLSchema-instance"><SOAP-ENV:Header><MessageID xmlns="http://www.chinamobile.com/IMS/VoLTEAS/">1</MessageI
 D></SOAP-ENV:Header><SOAP-ENV:Body><m:SET_OWSBRResponse xmlns:m="http://www.chinamobile.com/IMS/VoLTEAS/"><m:Result><m:ResultCode>0<
 /m:ResultCode><m:ResultDesc>操作成功。</m:ResultDesc></m:Result></m:SET_OWSBRResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>]


 ericsson hss:
 ====================
 2016-08-01 16:00:38.034 INFO  - send = [<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:hss="http:
 //www.chinamobile.com/HSS/">
 <soapenv:Header>
 <hss:PassWord>boss</hss:PassWord>
 <hss:UserName>boss</hss:UserName>
 </soapenv:Header>
 <soapenv:Body>
 <hss:ADD_TPLCSPSSUB><hss:HLRSN>2</hss:HLRSN><hss:IMSI>460028816705929</hss:IMSI><hss:ISDN>8615700201401</hss:ISDN><hss:CSPSTPLID>1</
 hss:CSPSTPLID></hss:ADD_TPLCSPSSUB></soapenv:Body>
 </soapenv:Envelope>
 ]
 2016-08-01 16:00:38.188 INFO  - receive = [<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/"><soap:Header/><soap
 :Body><ADD_TPLCSPSSUBResponse xmlns="http://www.chinamobile.com/HSS/"><Result><ResultCode>0</ResultCode><ResultDesc>SUCCESS0001:Oper
 ation is successful</ResultDesc><ResultData/></Result></ADD_TPLCSPSSUBResponse></soap:Body></soap:Envelope>]


 nokia hss:
 =====================
 [ INFO]00:01:16.911 -- send = [<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:hss="http://www.chi
 namobile.com/HSS/">
 <soapenv:Header>
 <hss:PassWord>hss9860#^(</hss:PassWord>
 <hss:UserName>NJBOSS</hss:UserName>
 </soapenv:Header>
 <soapenv:Body>
 <MOD_PLMNSS>
 <HLRSN>1</HLRSN>
 <ISDN>8615983299759</ISDN>
 <PLMN-SS-E>TRUE</PLMN-SS-E>
 </MOD_PLMNSS>
 </soapenv:Body>
 </soapenv:Envelope>
 ]
 [ INFO]00:01:16.944 -- receive = [<?xml version='1.0' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/
 " xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><SOAP-ENV:Header><header><sessi
 onID>09KOG0vSRPSxCLDZx11SHpYgusAB39b4P8ejCpkdkJ7RsVTZSDoTrHfNYRrYCaXF</sessionID></header></SOAP-ENV:Header><SOAP-ENV:Body><MOD_PLMN
 SSResponse><Result><ResultCode>0</ResultCode><ResultDesc>SUCCESS0001:操作成功</ResultDesc></Result></MOD_PLMNSSResponse></SOAP-ENV:B
 ody></SOAP-ENV:Envelope>]


 */
public class HSSServlet extends MyBaseServlet{

    @Override
    protected void doPost(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
        String content_type = req.getContentType();
        System.out.println("==content-type:"+content_type);
        String body = readBody(req.getInputStream());

        //<soapenv:Body>        <hss:ADD_TPLCSPSSUB>
        Pattern p = Pattern.compile("<soapenv:Body.*>[\\n\\r ]*<hss:([a-zA-Z_]+)>");
        Matcher m = p.matcher(body);
        System.out.println("---body"+body);
        String op = null;
        if(m != null && m.find()){
            op = m.group(1);
        }
        if(op == null){
            throw new ServletException("Invalid request");
        }

        sleep(120L);

        String rbody = "<?xml version='1.0' ?>" +
                "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"" +
                " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" +
                " xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">" +
                "<SOAP-ENV:Header><header><sessionID>09KOG0vSRPSxCLDZx11SHpYgusAB39b4P8ejCpkdkJ7RsVTZSDoTrHfNYRrYCaXF</sessionID>" +
                "</header></SOAP-ENV:Header><SOAP-ENV:Body>" +
                "<"+op+"Response><Result><ResultCode>0</ResultCode>" +
                "<ResultDesc>SUCCESS0001:Operation is successful</ResultDesc></Result></"+op+"Response></SOAP-ENV:Body></SOAP-ENV:Envelope>";
        resp.setContentType("text/xml");
        resp.setCharacterEncoding("UTF-8");
        resp.getWriter().write(rbody);
        //super.doPost(req, resp);
    }
}
