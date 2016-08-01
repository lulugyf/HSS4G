
import yaml
import re

def format(s, vars):
    r = []
    while 1:
        p = s.find('${')
        if p < 0:
            r.append(s)
            break
        p += 2
        p1 = s.find('}', p)
        if p1 < 0:
            r.append(s)
            break
        r.append(s[0:p-2])
        var = s[p:p1]
        #print '----', var
        r.append(vars.get(var, ""))
        s = s[p1 + 1:]

    return "".join(r)

# format string by map, if var not in map, keep it
def format_keepvar(s, vars):
    r = []
    while 1:
        p = s.find('${')
        if p < 0:
            r.append(s)
            break
        p += 2
        p1 = s.find('}', p)
        if p1 < 0:
            r.append(s)
            break
        r.append(s[0:p-2])
        var = s[p:p1]
        #print '----', var
        if vars.has_key(var):
            r.append(vars.get(var))
        else:
            r.append(s[p-2:p1+1])
        s = s[p1 + 1:]

    return "".join(r)


class Order:
    def __init__(self, order_file,  constants):
        self.order_file = order_file
        p = yaml.load(file(order_file))
        self.header = format_keepvar(p['header'], constants)
        self.footer = p['footer']
        orders = {}
        qry_orders = {}
        for k, v in p.items():
            if k.startswith('OP'):
                k = k[2:]
                orders[k] = format_keepvar(v, constants)
            elif k.startswith('QP'):
                q = {}
                for l in v:
                    l = l.split('=', 1)
                    q[l[0].strip()] = l[1].strip()
                qry_orders[k[2:]] = q
        self.orders = orders
        self.qry_orders = qry_orders

    def order(self, vars):
        #8601|phoneNo|imsi|ssInfo1|ssInfo2|ssInfo3
        vars = vars.split('|')
        for i in range(6 - len(vars)):
            vars.append("")
        code = vars[0]
        vars = vars[1:]
        args = {}
        vnames = ('phoneNo', 'imsi', 'ssInfo1', 'ssInfo2', 'ssInfo3')
        for i in range(len(vars)):
            args[vnames[i]] = vars[i]
        qry = self.qry_orders.get(code, None)
        if qry is None:
            cstr = self.orders.get(code, "")
        else:
            cstr = self.orders.get(qry['query_order'], "")
        if cstr == "":
            return cstr
        if cstr.find('${strutil.subStringTo(imsi,2,5)}') >= 0:
            args['strutil.subStringTo(imsi,2,5)'] = args['imsi'][2:5]
        if cstr.find('${strutil.subStringTo(ssInfo2,2,5)}') >= 0:
            args['strutil.subStringTo(ssInfo2,2,5)'] = args['ssInfo2'][2:5]
        #return self.header+format(cstr, args)+self.footer, qry is not None
        return format(cstr, args), qry is not None


    def order_qry(self, vars, qry_result):
        # 8601|phoneNo|imsi|ssInfo1|ssInfo2|ssInfo3
        vars = vars.split('|')
        for i in range(6 - len(vars)):
            vars.append("")
        code = vars[0]
        vars = vars[1:]
        args = {}
        vnames = ('phoneNo', 'imsi', 'ssInfo1', 'ssInfo2', 'ssInfo3')
        for i in range(len(vars)):
            args[vnames[i]] = vars[i]
        cstr = self.orders.get(code, "")
        if cstr == "":
            return cstr
        if cstr.find('${strutil.subStringTo(imsi,2,5)}') >= 0:
            args['strutil.subStringTo(imsi,2,5)'] = args['imsi'][2:5]
        if cstr.find('${strutil.subStringTo(ssInfo2,2,5)}') >= 0:
            args['strutil.subStringTo(ssInfo2,2,5)'] = args['ssInfo2'][2:5]

        #parse qry into args
        q = self.qry_orders.get(code, {})
        for k, v in q.items():
            if k == 'query_order': continue
            if v.find('${') >= 0:
                v = format(v, args)
            m = re.search(v, qry_result)
            if m is not None:
                args[k] = m.group(1)
        return format(cstr, args)

'''
---- test config file
------------------------
header: |
  <soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:hss="http://www.chinamobile.com/HSS/">
  <soapenv:Header>
  <hss:PassWord>${PASSWORD}</hss:PassWord>
  <hss:UserName>${USERNAME}</hss:UserName>
  </soapenv:Header>
  <soapenv:Body>
footer: |
  </soapenv:Body>
  </soapenv:Envelope>
QP3791:
 - query_order=9222
 - cntxid=<Group><CNTXID>(\d+)</CNTXID>.+<QOSTPLID>${ssInfo3}</QOSTPLID>
OP3791: <hss:MOD_GPRS_CONTEXT><hss:HLRSN>${HLRSN}</hss:HLRSN><hss:ISDN>86${phoneNo}</hss:ISDN><hss:PROV>RMVPDPCNTX</hss:PROV><hss:CNTXID>${cntxid}</hss:CNTXID></hss:MOD_GPRS_CONTEXT>
OP8601: <hss:MOD_LCK><hss:HLRSN>${HLRSN}</hss:HLRSN><hss:ISDN>86${phoneNo}</hss:ISDN><hss:IC>false</hss:IC><hss:OC>false</hss:OC></hss:MOD_LCK>
OP0091: <hss:ADD_TPLIMSSUB><hss:HLRSN>${HLRSN}</hss:HLRSN><hss:IMSI>${imsi}</hss:IMSI><hss:IMPI>${imsi}@ims.mnc${strutil.subStringTo(imsi,2,5)}.mcc460.3gppnetwork.org</hss:IMPI><hss:IMPULIST>sip:+86${phoneNo}@sc.ims.mnc000.mcc460.3gppnetwork.org&amp;tel:+86${phoneNo}&amp;sip:${imsi}@ims.mnc${strutil.subStringTo(imsi,2,5)}.mcc460.3gppnetwork.org</hss:IMPULIST><hss:BARIMPULIST>sip:${imsi}@ims.mnc${strutil.subStringTo(imsi,2,5)}.mcc460.3gppnetwork.org</hss:BARIMPULIST><hss:VOLTETAG>TRUE</hss:VOLTETAG><hss:IMSTPLID>1</hss:IMSTPLID></hss:ADD_TPLIMSSUB>
OP0041: <hss:ADD_TPLIMSSUB><hss:HLRSN>${HLRSN}</hss:HLRSN><hss:IMSI>${ssInfo2}</hss:IMSI><hss:IMPI>${ssInfo2}@ims.mnc${strutil.subStringTo(ssInfo2,2,5)}.mcc460.3gppnetwork.org</hss:IMPI><hss:IMPULIST>sip:+86${phoneNo}@sc.ims.mnc000.mcc460.3gppnetwork.org&amp;tel:+86${phoneNo}&amp;sip:${ssInfo2}@ims.mnc${strutil.subStringTo(ssInfo2,2,5)}.mcc460.3gppnetwork.org</hss:IMPULIST><hss:BARIMPULIST>sip:${ssInfo2}@ims.mnc${strutil.subStringTo(ssInfo2,2,5)}.mcc460.3gppnetwork.org</hss:BARIMPULIST><hss:VOLTETAG>TRUE</hss:VOLTETAG><hss:IMSTPLID>1</hss:IMSTPLID></hss:ADD_TPLIMSSUB>
'''
def main():
    fname = r'D:\00work\volte@201507\erc\ERI_mianyang.yaml'
    consts = {'HLRSN':"1", "USERNAME":"boss", "PASSWORD":"boss"}
    o = Order(fname, consts)
    print o.order('8601|15700240244|460077296271684|||')
    print o.order('0091|15700240244|460077296271684|||')

    cmd, qry = o.order('3791|15700240244|460077296271684|||911')
    if qry == True:
        qry_result = "<Group><CNTXID>119</CNTXID>.+<QOSTPLID>911</QOSTPLID>"
        cmd = o.order_qry('3791|15700240244|460077296271684|||911', qry_result)
        print cmd

    # s = '<hss:ADD_SIFC><hss:HLRSN>${HLRSN}</hss:HLRSN><hss:IMPU>sip:+86${phoneNo}@sc.ims.mnc000.mcc460.3gppnetwork.org</hss:IMPU><hss:SIFCID>721</hss:SIFCID></hss:ADD_SIFC>'
    # print format_keepvar(s, consts)
    # print format(s, consts)

main()