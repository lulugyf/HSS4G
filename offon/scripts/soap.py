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

        m = re.search('HTTP/1.1[ |\t]+(\\d+)[ |\t]+(.+)\r\n', header, re.IGNORECASE)
        if m is None:
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

cmd1 = "<v:RPL_CFNR><v:IMPU>tel:+86%s</v:IMPU></v:RPL_CFNR>"
cmd2 = "<v:DSP_CFNR><v:IMPU>tel:+86%s</v:IMPU></v:DSP_CFNR>"
cmd3 = "<v:DSP_CFNRC><v:IMPU>tel:+86%s</v:IMPU></v:DSP_CFNRC>"
tas_url = "http://10.112.175.212:8080/spg"


def getresult(s):
    m = re.search('<m:Result>(.+)</m:Result>', s)
    if m is None: return s
    return m.group(1)


def order(c, ph, cmdstr):
    cmd = cmdstr % ph
    print '---', cmd
    data = packet % cmd
    s = c.post(data)
    # print s.decode('utf-8').encode('gbk')
    print getresult(s)


def main():
    c = Conn(tas_url)
    ph = '13881623308'
    order(c, ph, '<v:DSP_CFNR><v:IMPU>tel:+86%s</v:IMPU></v:DSP_CFNR>')
    order(c, ph, '<v:DSP_CFNRC><v:IMPU>tel:+86%s</v:IMPU></v:DSP_CFNRC>')
    order(c, ph, '<v:DSP_CFB><v:IMPU>tel:+86%s</v:IMPU></v:DSP_CFB>')
    c.close()


if __name__ == '__main__':
    main()