import socket
import re
import sys
import httplib

def test():
    con = httplib.HTTPConnection("wwww.baidu.com")
    while True:
        con.request("GET", "/", headers={"Connection": " keep-alive"})
        result = con.getresponse()
        result.read()
        print result.status, result.reason, result.getheaders()

def recvn(sock, n):
    s = ""
    while n > 0:
        try:
            s1 = sock.recv(n)
        except Exception, x:
            print "recv failed[%s]" % x
            return None
        if len(s1) == 0:
            return None
        n -= len(s1)
        s = s + s1
    return s


class Conn:
    def __init__(self, url):
        p = '^http://([\\d\\.]+):?(\\d*)(.*)'
        m = re.search(p, url)
        if m is None:
            print 'invalid url', url
            return
        host, port, uri = m.groups()
        if port == '':
            port = 80
        else:
            port = int(port)
        if uri == '': uri = '/'

        self.co = socket.create_connection((host, port))
        self.uri = uri
        self.host = '%s:%d' % (host, port)

    def post(self, data):
        if self.co is None:
            print 'not connected'
            return None
        co = self.co
        head = 'POST %s HTTP/1.1\r\nContent-Length: %d\r\nAccept-Encoding: deflate\r\nSoapaction: \r\nHost: %s\r\nUser-Agent: BossApp1.0\r\nConnection: Keep-Alive\r\nContent-Type: text/xml; charset=UTF-8\r\n\r\n' % (
            self.uri, len(data), self.host)
        co.sendall(head)
        co.sendall(data)
        # print 'send[%s]'%data

        h = ''
        while 1:
            s = co.recv(1024)
            if s == '':
                print 'Connection reset'
                self.close()
                return None
            h = '%s%s' % (h, s)
            if h.find('\r\n\r\n') > 0:
                break
        p = h.find('\r\n\r\n') + 4
        header = h[:p]
        data = h[p:]

        m = re.search('HTTP/1.1[ |\t]+(\\d+)[ |\t]*(.*)\r\n', header, re.IGNORECASE)
        if m is None:
            print "header:", header
            print "Invalid http response"
            self.close()
            return None
        rcode, rstatus = m.group(1), m.group(2)
        print rcode, rstatus
        m = re.search('\r\nContent-Length[ |\t]*:[ |\t](\\d+)', header, re.IGNORECASE)
        if m is None:
            print 'ERROR: not found Content-Length'
            self.close()
            return None
        clen = int(m.group(1))
        clen -= len(data)
        data1 = recvn(co, clen)
        if data1 is None:
            self.close()
            return None
        return data + data1

    def close(self):
        if self.co != None:
            self.co.close()
            self.co = None

##################################### for test purpose

class TAS:
    def __init__(self, url):
        self.c = Conn(url)
    packet = '''
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:SOAP-ENC="http://
schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchem
a">
<SOAP-ENV:Header xmlns:m="http://www.chinamobile.com/IMS/VoLTEAS/">
 <m:Authentication >
  <m:Username>CDBOSS</m:Username>
  <m:Password>7C20F57649F7A945E61E4D895455A7E5</m:Password>
 </m:Authentication>
 <m:MEName>CDVOLTETAS5BHW</m:MEName>
 <m:MessageID>1</m:MessageID>
</SOAP-ENV:Header>
<SOAP-ENV:Body xmlns:v="http://www.chinamobile.com/IMS/VoLTEAS/">
%s
</SOAP-ENV:Body></SOAP-ENV:Envelope>'''
    cmds = {
        "01":"<v:RPL_CFNR><v:IMPU>tel:+86%s</v:IMPU></v:RPL_CFNR>",
        "02":"<v:DSP_CFNR><v:IMPU>tel:+86%s</v:IMPU></v:DSP_CFNR>",
        "03":"<v:DSP_CFNRC><v:IMPU>tel:+86%s</v:IMPU></v:DSP_CFNRC>",
    }
    def _getresult(self, s):
        m = re.search('<m:Result>(.+)</m:Result>', s)
        if m is None: return s
        return m.group(1)
    def exec_order(self, ph, cmdcode):
        cmdstr = self.cmds[cmdcode]
        cmd = cmdstr % ph
        print '---', cmd
        data = self.packet % cmd
        s = self.c.post(data)
        #print s.decode('utf-8').encode('gbk')
        return self._getresult(s).decode('utf-8').encode('gbk')
    def close(self):
        self.c.close()


def tas_test():
    #tas_url = "http://10.112.175.212:8080/spg"
    tas_url = "http://127.0.0.1:8080/tas"
    t = TAS(tas_url)
    ph = '13881623308'
    print t.exec_order(ph, '01')
    print t.exec_order(ph, '02')
    print t.exec_order(ph, '03')
    t.close()

class HSS_eri:
    def __init__(self, url):
        self.c = Conn(url)
    packet = '''<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/"
     xmlns:hss="http://www.chinamobile.com/HSS/">
 <soapenv:Header>
 <hss:PassWord>boss</hss:PassWord>
 <hss:UserName>boss</hss:UserName>
 </soapenv:Header>
 <soapenv:Body>
 %s
 </soapenv:Body>
 </soapenv:Envelope>'''
    cmds = {
        "01":"<hss:MOD_DIAMRRS><hss:HLRSN>2</hss:HLRSN><hss:ISDN>86%s</hss:ISDN><hss:PROV>FALSE</hss:PROV></hss:MOD_DIAMRRS>",
        "02":"<hss:MOD_CFB><hss:HLRSN>2</hss:HLRSN><hss:ISDN>86%s</hss:ISDN><hss:PROV>FALSE</hss:PROV></hss:MOD_CFB>",
        "03":"<hss:MOD_TS><hss:HLRSN>2</hss:HLRSN><hss:ISDN>86%s</hss:ISDN><hss:TS11>true</hss:TS11></hss:MOD_TS>",
    }
    def _getresult(self, s):
        m = re.search("<Result>(.+)</Result>", s)
        if m is None: return s
        return m.group(1)
    def exec_order(self, ph, cmdcode):
        cmdstr = self.cmds[cmdcode]
        cmd = cmdstr % ph
        print '---', cmd
        data = self.packet % cmd
        s = self.c.post(data)
        # print s.decode('utf-8').encode('gbk')
        return self._getresult(s).decode('utf-8').encode('gbk')

    def close(self):
        self.c.close()

def hss_test():
    #url = "http://10.112.141.121:8080/LTEProxy/LTESOAPEndPoint"
    url = "http://127.0.0.1:8080/hss"
    t = HSS_eri(url)
    ph = '15700201401'
    print t.exec_order(ph, '01')
    print t.exec_order(ph, '02')
    print t.exec_order(ph, '03')
    t.close()


class ENUM:
    def __init__(self, url):
        self.c = Conn(url)
    packet = '''<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:enum="http://www.chinamobile.com/IMS/ENUM">
 <soapenv:Header>
 <enum:Authentication>
 <enum:Username>bss123456</enum:Username>
 <enum:Password>524737C63C4A1FACD8E65176FC811EB2</enum:Password>
 </enum:Authentication>
 <enum:MessageID>1</enum:MessageID>
 <enum:MEName>CDENS2</enum:MEName>
 </soapenv:Header>
 <soapenv:Body>
 %s
 </soapenv:Body>
 </soapenv:Envelope>'''
    cmds = {
        "01":"<enum:deleteEnumRecordByName><enum:name>1.0.4.1.0.2.0.0.7.5.1.6.8.e164.arpa</enum:name></enum:deleteEnumRecordByName>",
        "02":"""<enum:addEnumRecord>
  <enum:zoneName>0.2.0.0.7.5.1.6.8.e164.arpa</enum:zoneName>
  <enum:name>1.0.4.1.0.2.0.0.7.5.1.6.8.e164.arpa</enum:name>
  <enum:order>1</enum:order>
  <enum:preference>1</enum:preference>
  <enum:flags>U</enum:flags>
  <enum:service>sip+e2u</enum:service>
  <enum:regexp>!^.*$!sip:+8615700201401@sc.ims.mnc000.mcc460.3gppnetwork.org!</enum:regexp>
</enum:addEnumRecord>""",
    }
    def _getresult(self, s):
        m = re.search("<Result>(.+)</Result>", s)
        if m is None: return s
        return m.group(1)
    def exec_order(self, ph, cmdcode):
        cmdstr = self.cmds[cmdcode]
        cmd = cmdstr % ph
        print '---', cmd
        data = self.packet % cmd
        s = self.c.post(data)
        # print s.decode('utf-8').encode('gbk')
        return self._getresult(s).decode('utf-8').encode('gbk')

    def close(self):
        self.c.close()

def enum_test():
    #url = "http://10.112.141.121:8080/LTEProxy/LTESOAPEndPoint"
    url = "http://127.0.0.1:8080/enum"
    t = HSS_eri(url)
    ph = '15700201401'
    print t.exec_order(ph, '01')
    print t.exec_order(ph, '02')
    t.close()

if __name__ == '__main__':
    #tas_test()
    hss_test()
