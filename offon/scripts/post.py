import urllib2


data='''
<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:hss="http://www.chinamobile.com/HSS/">
<soapenv:Header>
<hss:PassWord>boss@123</hss:PassWord>
<hss:UserName>boss1</hss:UserName>
</soapenv:Header>
<soapenv:Body>
<hss:LST_SUB>
<hss:HLRSN>3</hss:HLRSN>
<hss:ISDN>8618381327933</hss:ISDN>
<hss:DETAIL>TRUE</hss:DETAIL>
</hss:LST_SUB>
</soapenv:Body>
</soapenv:Envelope>'''

url = 'http://127.0.0.1:8080/LTEProxy/LTESOAPEndPoint'
#url = 'http://10.112.141.153:8080/LTEProxy/LTESOAPEndPoint'
#url = 'http://10.112.141.73:8080/LTEProxy/LTESOAPEndPoint'
#url = 'http://10.109.63.148:7777'
print '====post: ',url
print data
r = urllib2.Request(url, data=data,
                     headers={
'Content-Type': 'text/xml; charset=UTF-8', 
'User-Agent':'Apache-HttpClient/4.4.1 (Java/1.6.0_31)', 
'Accept-Encoding': 'gzip,deflate',
'Connection': 'Keep-Alive',
'SOAPAction': ''})
try:
        u = urllib2.urlopen(r)
        print '===receive',
        print u.read()
except urllib2.HTTPError,ex:
        print "====exception"
        print ex.read()
