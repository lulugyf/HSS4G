# HSS4G

���ػ�SOAPָ��ͻ���

 1. 20150723 ֧���� .yaml ��ʽ����ָ���ļ��� ��� orderPath ����.yaml ��β���ļ�������yaml������ 
=====================================
 			���������Ŀ¼�� ����Ȼ��ԭ�з�ʽ����
    yaml �ļ���ʽ���£�header footer ����Ҫ�У���
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
  
    
    
 2. 20150723 ֧�ֲ�ѯָ���ȡ��ѯ����е�������Ϊ����������һ��ָ����
=====================================
      ���ʵ�ֵ�ǰ�������� manager ��ָ����������Լ�˳����ƻ��ƣ� �����ϻ��ͬһ��ָ�����ָ���ͬһ���ӿڳ���
    **** ���ʹ��������ͬһ��command����������order֮��
   order�����ʽ����� .yaml, ����� query_param ��key�ҵ��ļ��� �����Ŀ¼�� ����Ŀ¼����query_param.yaml
   �ļ���ʽ����, ����������������ʽ�����ж����
QP3002: 
 - apntplid=<Group><CNTXID>(\d+)</CNTXID><QOSTPLID>119</QOSTPLID>
     
     ����ָ����������:  �ر� blackberry ��ָ��ֽ� 300 -> 9101/3002
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


 3. �����ŵ�ip��ַת���� ǰ̨��������hex��ʽ����Ҫת���� n.n.n.n ��ʽ��
=====================================
   ���ò��� IPHEX_TO_INT: 3781~4011    ����ʾ 3781 4011 ������order��Ҫת�� ss_info2 �е�hexΪʮ���ƣ�
 
   3781|18483590060|||a0a0a0a0|CDNONGHANG.SC
ָ���������£�
OP3781: |
  <hss:MOD_GPRS_CONTEXT>
  <hss:HLRSN>${HLRSN}</hss:HLRSN>
  <hss:ISDN>86${phoneNo}</hss:ISDN>
  <hss:PROV>ADDPDPCNTX</hss:PROV>
  <hss:PSAPNQOSTPLID>${ssInfo3}</hss:PSAPNQOSTPLID>
  <hss:IPV4ADDIND>STATIC</hss:IPV4ADDIND>
  <hss:IPV4ADD>${ssInfo2}</hss:IPV4ADD>
  </hss:MOD_GPRS_CONTEXT>
    
 4. apn ������ģ��id��ת����
=====================================
   ���ò��� APN_TPL_ID: ericsson.YA.APN.cfg  ��.cfg�ļ�λ�� ETCDIR Ŀ¼�У��������Դ�Сд)
�����ļ���ʽ���£�
_orders=3781
_paramIndex=3               ��ţ� phoneNo|imsi|ssInfo1|ssInfo2|ssInfo3  ��Ӧ 1 | 2 | 3 | 4 | 5, ����ֻ����ʹ�� 3 4 5
abkrd.sc=1
abncxys.sc=2
bcsw.sc=3
bzkrd.sc=4
bznjxy.sc=5  
