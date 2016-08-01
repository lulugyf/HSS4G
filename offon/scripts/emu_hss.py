import web
import time

urls = (
  '/', 'index',
  '/LTEProxy/LTESOAPEndPoint', 'mysoap',
)

class index:
    def GET(self):
        return "Hello, world!"

class mysoap:
    def POST(self):
        web.header('Content-Type', 'text/xml')
        data = web.data()
        op = "OP_None"
        sig = '<soapenv:Body>'
        if data.find(sig) > 0:
            p = data.find(sig) + len(sig)
            p1 = data.find('>', p)
            s = data[p:p1]
            if s.find(':') > 0: s = s[s.find(':')+1:]
            s = s.replace('<', ''); s = s.replace('>', ''); s = s.strip()
            op = s
        time.sleep(0.2)
        return '<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/"><soap:Header/><soap:Body><${op}Response xmlns="http://www.chinamobile.com/HSS/"><Result><ResultCode>0</ResultCode><ResultDesc>SUCCESS0001:Operation is successful</ResultDesc><ResultData/></Result></${op}Response></soap:Body></soap:Envelope>'.replace('${op}', op)


if __name__ == '__main__':
    app = web.application(urls, globals())
    app.run()
