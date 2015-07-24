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
   文件格式如下, 解析参数的正则表达式可以有多个：
QP3002: 
 - apntplid=<Group><CNTXID>(\d+)</CNTXID><QOSTPLID>119</QOSTPLID>
     
     两条指令配置如下:  关闭 blackberry 的指令分解 300 -> 9101/3002
OP9101: |
  <LST_SUB>
  <HLRSN>${HLRSN}</HLRSN>
  <ISDN>86${phoneNo}</ISDN>
  <DETAIL>TRUE</DETAIL>
  </LST_SUB>
OP3002: |
  <hss:MOD_GPRS_CONTEXT>
  <hss:HLRSN>13</hss:HLRSN>
  <hss:ISDN>86${phoneNo}</hss:ISDN>
  <hss:PROV>RMVPDPCNTX</hss:PROV>
  <hss:CNTXID>${apntplid}</hss:CNTXID>
  </hss:MOD_GPRS_CONTEXT>

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
 

 