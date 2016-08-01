#encoding=utf-8

import web
import time

urls = (
  '/', 'index',
  '/spg', 'mysoap',
)

class index:
    def GET(self):
        return "Hello, world!"

class mysoap:
    def POST(self):
        web.header('Content-Type', 'text/xml')
        data = web.data()
        op = "OP_None"
        sig = '<SOAP-ENV:Body'
        if data.find(sig) > 0:
            p = data.find(sig) + len(sig)
            p1 = data.find('>', p)
            s = data[p:p1]
            if s.find(':') > 0: s = s[s.find(':')+1:]
            s = s.replace('<', ''); s = s.replace('>', ''); s = s.strip()
            op = s
        time.sleep(1.5)
        return '''<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><SOAP-ENV:Header><MessageID xmlns="http://www.chinamobile.com/IMS/VoLTEAS/">1</MessageID></SOAP-ENV:Header><SOAP-ENV:Body><m:${op}Response xmlns:m="http://www.chinamobile.com/IMS/VoLTEAS/"><m:Result><m:ResultCode>0</m:ResultCode><m:ResultDesc>操作成功。</m:ResultDesc></m:Result></m:${op}Response></SOAP-ENV:Body></SOAP-ENV:Envelope>'''.replace('${op}', op)


if __name__ == '__main__':
    app = web.application(urls, globals())
    app.run()
