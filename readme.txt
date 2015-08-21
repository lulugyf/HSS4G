# HSS4G

开关机SOAP指令客户端

 1. 20150723 支持以 .yaml 格式配置指令文件， 如果 orderPath 是以.yaml 结尾的文件，则以yaml解析， 
=====================================
  hss.cfg 也转换为 hss.yaml, order指令文件也改为.yaml
  转换可以通过本目录中的 2yaml.py 脚本来处理

 			否则如果是目录， 则仍然以原有方式处理
    yaml 文件格式如下（header footer 必须要有）：
header: |
  <soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:hss="http://www.chinamobile.com/HSS/">
   <soapenv:Header>
    <hss:PassWord>hss9860#^(</hss:PassWord>
    <hss:UserName>NJBOSS</hss:UserName>
   </soapenv:Header>
  <soapenv:Body>
  
footer: |
  </soapenv:Body>
  </soapenv:Envelope>

OP0001: |
  <MOD_ODB>
  <HLRSN>${HLRSN}</HLRSN>
  <ISDN>86${phoneNo}</ISDN>
  <ODBOC>BOIC</ODBOC>
  </MOD_ODB>
OP0011: |
  <MOD_ODB>
  <HLRSN>${HLRSN}</HLRSN>
  <ISDN>86${phoneNo}</ISDN>
  <ODBOC>NOBOC</ODBOC>
  </MOD_ODB>
OP8607: |
  <ADD_KI>
  <HLRSN>${HLRSN}</HLRSN>
  <IMSI>${imsi}</IMSI>
  <OPERTYPE>ADD</OPERTYPE>
  <% var kival=strutil.subStringTo(ssInfo1,0,14)+strutil.subStringTo(ssInfo2,0,18); %>
  <KIVALUE>${kival}</KIVALUE>
  <K4SNO>1</K4SNO>
  <ALG>MILENAGE</ALG>
  <OPCVALUE>${ssInfo3}</OPCVALUE>
  <AMFSNO>1</AMFSNO>
  </ADD_KI>
  
    
    
 2. 20150723 支持查询指令后取查询结果中的内容作为参数传到下一条指令中
=====================================
      这个实现的前提条件是 manager 的指令按号码锁定以及顺序控制机制， 基本上会把同一条指令的子指令给同一个接口程序
    **** 因此使用限制是同一条command的相邻两条order之间
   order存放形式如果是 .yaml, 则查找 query_param 的key找到文件， 如果是目录， 则在目录中找query_param.yaml
   本配置的内容也可以直接加在 order指令配置文件中
   QP配置中可以有 query_order 项， 用于配置使用哪个指令来执行查询， 如果没有配置 query_order, 则使用最近一次执行指令的结果解析
   文件格式如下, 解析参数的正则表达式可以有多个：
QP3002: 
 - apntplid=<Group><CNTXID>(\d+)</CNTXID><QOSTPLID>119</QOSTPLID>
     
     两条指令配置如下:  关闭 blackberry 的指令分解 300 -> 9101/3002
QP3002:
 - query_order=9222
 - cntxid=<Group><CNTXID>(\d+)</CNTXID><PDPTYPE>IPV4</PDPTYPE><QOSTPLID>119</QOSTPLID>
OP3002: |
  <hss:MOD_GPRS_CONTEXT>
  <hss:HLRSN>${HLRSN}</hss:HLRSN>
  <hss:ISDN>86${phoneNo}</hss:ISDN>
  <hss:PROV>RMVPDPCNTX</hss:PROV>
  <hss:CNTXID>${cntxid}</hss:CNTXID>
  </hss:MOD_GPRS_CONTEXT>
QP3202:
 - query_order=9222
 - cntxid=<Group><CNTXID>(\d+)</CNTXID><PDPTYPE>IPV4</PDPTYPE><QOSTPLID>50</QOSTPLID>
OP3202: |
  <hss:MOD_GPRS_CONTEXT>
  <hss:HLRSN>${HLRSN}</hss:HLRSN>
  <hss:ISDN>86${phoneNo}</hss:ISDN>
  <hss:PROV>RMVPDPCNTX</hss:PROV>
  <hss:CNTXID>${cntxid}</hss:CNTXID>
  </hss:MOD_GPRS_CONTEXT>
OP9222: |
  <hss:LST_GPRS>
  <hss:HLRSN>${HLRSN}</hss:HLRSN>
  <hss:ISDN>86${phoneNo}</hss:ISDN>
  </hss:LST_GPRS>

3011|18483590060||||
9101|18483590060||||
3002|18483590060||||


 3. 爱立信的ip地址转换（ 前台送来的是hex格式，需要转换成 n.n.n.n 格式）
=====================================
   设置参数 IPHEX_TO_INT: 3781~4011    （表示 3781 4011 这两个order需要转换 ss_info2 中的hex为十进制）
 
   3781|18483590060|||a0a0a0a0|CDNONGHANG.SC
指令配置如下：
OP3781: |
  <hss:MOD_GPRS_CONTEXT>
  <hss:HLRSN>${HLRSN}</hss:HLRSN>
  <hss:ISDN>86${phoneNo}</hss:ISDN>
  <hss:PROV>ADDPDPCNTX</hss:PROV>
  <hss:PSAPNQOSTPLID>${ssInfo3}</hss:PSAPNQOSTPLID>
  <hss:IPV4ADDIND>STATIC</hss:IPV4ADDIND>
  <hss:IPV4ADD>${ssInfo2}</hss:IPV4ADD>
  </hss:MOD_GPRS_CONTEXT>
    
 4. apn 域名到模板id的转换：
=====================================
   配置参数 APN_TPL_ID: ericsson.YA.APN.cfg  （.cfg文件位于 ETCDIR 目录中，域名忽略大小写)
配置文件格式如下：
_orders=3781
_paramIndex=3               序号： phoneNo|imsi|ssInfo1|ssInfo2|ssInfo3  对应 1 | 2 | 3 | 4 | 5, 这里只允许使用 3 4 5
abkrd.sc=1
abncxys.sc=2
bcsw.sc=3
bzkrd.sc=4
bznjxy.sc=5  


5. 手工测试
=====================================
   在目录 gyf/lte/test 中， 修改 hss.yaml 文件， 格式与 etc/hss.yaml 相同， 可以只保留要测试的部分
   启动脚本run1内容如下：
JVM_OPTS="${JVM_OPTS} -Duser.language=Zh -Duser.region=CN"
JVM_OPTS="${JVM_OPTS} -Ddefault.client.encoding=GBK"
JVM_OPTS="${JVM_OPTS} -Dfile.encoding=GBK"
JVM_OPTS="${JVM_OPTS} -Dhlrname=$1"
j2se631/bin/java ${JVM_OPTS} -cp .:offon-soap-0.0.1-SNAPSHOT-jar-with-dependencies.jar com.sitech.crmpd.inter.client.jmx.HandCmd $@

  运行 ./run1 ${hlrcode}.${hlrport}
  
6. 合并进程的生产运行
======================================
  单独启动主进程
 java -javaagent:agent.jar=port=7777,host=localhost,agentContext=/j4p -cp offon-soap-0.0.1-SNAPSHOT-jar-with-dependencies.jar com.sitech.crmpd.inter.client.jmx.Control
 
 然后通过 jolokia的 http端口 7777 发送http请求来操作工作线程：
 列出工作线程：
 curl http://127.0.0.1:7777/j4p/exec/offon:name=PortControl/list
 启动工作线程：
 curl http://127.0.0.1:7777/j4p/exec/offon:name=PortControl/start/haa/2
 停止工作线程：
 curl http://127.0.0.1:7777/j4p/exec/offon:name=PortControl/stop/haa/2
 

7. hss.yaml 配置调整， 支持一个port多端口执行指令， 并允许指令二次分解（2015-8-17）
=================================================
hzx.1:
  manager.ip: 
  manager.port: 
  manager.user: 
  manager.password: 
  order_reloadable: true
  remote.url: http://10.109.230.132:8080/spg
  remote.timeout: 60000
  http.keep.test: false
  http.keep.cfg: /offon/gyf/lte/test/http.keep.properties
  order.path: /offon/gyf/lte/test/orders/hzx.yaml
  start.tag: <ResultCode>
  end.tag: </ResultCode>
  term.type: 1
  APN_TPL_ID: 
  IPHEX_TO_INT:
  PROPERTIES:
  caller.1:  #接口1， 为了兼容原有的配置， 原配置中的接口理解为 caller.0, 每个接口使用各自的指令配置文件
                 # caller的数字编号必须从1开始，按1累加，否则无效
                 # caller配置中的以下项， 支持从 caller.0 中继承（如果没有配置的话）：
                 # "order_reloadable", "remote.url", "remote.timeout", "http.keep.test", "http.keep.cfg", 
				 # "order.path", "start.tag", "end.tag", " APN_TPL_ID", "IPHEX_TO_INT", "PROPERTIES"
    remote.url: http://10.95.233.202:8002
    remote.timeout: 60000
    http.keep.test: false
    order.path: /offon/etc/hss_orders/HUAWEI_cd25.yaml
  caller.2:
    remote.url: http://10.95.233.202:8002
    remote.timeout: 60000
    http.keep.test: false
    order.path: /offon/etc/hss_orders/HUAWEI_cd25.yaml
  group.orders:  #组合指令， 允许在不同的接口上执行,  指令代码后的 0 1 2 为指定的接口编号
    8001:
      - 8002 0
      - 8003 1
      - 8004 2
      
8. hss.yaml 文件配置优化， 支持 copy_from 选项 以及 clone 模式 (2015-8-17)
===================================================
clone模式： 完全复制另一个根配置， 如下的配置则是hf1.2 与 hf1.1 的内容完全相同
hf1.2: hf1.1
 clone只处理一次， 不会因为配置成递归而崩溃
copy_from:  hlr端口配置中增加一个配置串 copy_from, 指向另一个port配置， 则会复制该port的配置，本配置下的相同名称项将覆盖被复制项的内容

  处理顺序： 先处理 clone, 然后处理 copy_from, 所以下面的配置是允许的,  最终h1与h2相同 { a:aa; b:bb; c:cc }
h0:
  a: 00
  c: cc
h1:
  a: aa
  b: bb
  copy_from: h0
h2: h1

9. order.yaml 的配置优化， 支持衍生指令（2015-8-17）
==========================
 有些指令之间只有些微的差别， 完全重复配置显得太累赘， 可以向下面这样配置：
 OPt0005: |
  <v:SET_OWSBR>
   <v:IMPU>tel:+86${phoneNo}</v:IMPU>
  <v:US>__var__</v:US>
  </v:SET_OWSBR>
OPt0005.1: 0
OPt0005.2: 1
OPt0005.3: 3

 通过 OPt0005.1 后的  .1 .2 .3 来判断是否衍生指令， .前面的是被衍生指令的代码， 其值用来替换被衍生指令的 __var__ 部分
   

10. hss.yaml 配置优化： removenamespace 可选项 （2015-8-17）
=============================
针对caller的配置， 可以配置一个removenamespace项， 其内容为ns: http://www.chinamobile.com/IMS/VoLTEAS/
对执行指令的结果串做处理， 移除ns， 便于结果的处理
<m:Response xmlns:m="http://www.chinamobile.com/IMS/VoLTEAS/">
 <m:ResultCode>0</m:ResultCode>
</m:Response>
===>>
<Response xmlns:m="http://www.chinamobile.com/IMS/VoLTEAS/">
 <ResultCode>0</ResultCode>
</Response>

11. 针对Enum接口的处理， 需要把号码和号段转换为域名 （2015-8-18）
=============================
在 order.yaml 文件中配置一个 OP__enum__: enum 后， 则每个指令执行前会增加2个参数 phoneNoDomain phoneHeadDomain
 他们都是由参数 phoneNo 转换来的(字符倒序并加.分隔)， 假设phoneNo = "13541380001", 则：
  phoneNoDomain = "1.0.0.0.8.3.1.4.5.3.1."
  phoneHeadDomain = "8.3.1.4.5.3.1."

  
12. 手工接口， 文件批量指令接口， 以及manager正式运行接口统一
    手工和批量执行入口仍然在 ...jmx.HandCmd 中
    三种接口分别为 FileComm, ConsoleComm, ManagerComm,  都继承自BaseComm
