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



 huawei enum:
 ====================
 2016-08-02 16:54:56.642 INFO  - send = [<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:enum="http://www.chinamobile.com/IMS/ENUM">
 <soapenv:Header>
 <enum:Authentication>
 <enum:Username>bss123456</enum:Username>
 <enum:Password>524737C63C4A1FACD8E65176FC811EB2</enum:Password>
 </enum:Authentication>
 <enum:MessageID>1</enum:MessageID>
 <enum:MEName>CDENS2</enum:MEName>
 </soapenv:Header>
 <soapenv:Body>
 <enum:deleteEnumRecordByName>
 <enum:name>1.0.4.1.0.2.0.0.7.5.1.6.8.e164.arpa</enum:name>
 </enum:deleteEnumRecordByName>
 </soapenv:Body>
 </soapenv:Envelope>
 ]
 2016-08-02 16:54:56.683 INFO  - receive = [<?xml version="1.0" encoding="UTF-8"?>
 <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema"
 xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><SOAP-ENV:Header>
   <MessageID xmlns="http://www.chinamobile.com/IMS/ENUM">1</MessageID></SOAP-ENV:Header><SOAP-ENV:Body>
 <m:deleteEnumRecordByNameResponse xmlns:m="http://www.chinamobile.com/IMS/ENUM"><m:ResultCode>0</m:ResultCode>
 <m:ResultDesc>Operation Succeeded.</m:ResultDesc></m:deleteEnumRecordByNameResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>]

 */
public class ENUMServlet extends HttpServlet{
    private String readBody(InputStream pin) throws IOException {
        BufferedReader bf = new BufferedReader(new InputStreamReader(pin));
        StringBuilder sb = new StringBuilder();
        while(true) {
            String line = bf.readLine();
            if(line == null) break;
            sb.append(line);
        }
        return sb.toString();
    }
    @Override
    protected void doPost(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
        String content_type = req.getContentType();
        System.out.println("==content-type:"+content_type);
        String body = readBody(req.getInputStream());

        Pattern p = Pattern.compile("<soapenv:Body.*>[\\n\\r ]*<enum:([a-zA-Z_]+)>");
        Matcher m = p.matcher(body);
        System.out.println("---body"+body);
        String op = null;
        if(m != null && m.find()){
            op = m.group(1);
        }
        if(op == null){
            throw new ServletException("Invalid request");
        }

        String rbody = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
                " <SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\n" +
                " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"><SOAP-ENV:Header>\n" +
                "   <MessageID xmlns=\"http://www.chinamobile.com/IMS/ENUM\">1</MessageID></SOAP-ENV:Header><SOAP-ENV:Body>\n" +
                " <m:"+op+"Response xmlns:m=\"http://www.chinamobile.com/IMS/ENUM\"><m:ResultCode>0</m:ResultCode>\n" +
                " <m:ResultDesc>Operation Succeeded.</m:ResultDesc></m:"+op+"Response></SOAP-ENV:Body></SOAP-ENV:Envelope>";
        resp.setContentType("text/xml");
        resp.setCharacterEncoding("UTF-8");
        resp.getWriter().write(rbody);
        //super.doPost(req, resp);
    }
}
