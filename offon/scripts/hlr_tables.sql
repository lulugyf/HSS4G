#!/bin/ksh

# guanyf 20060519
#本shell是在新增HLR的时候数据库部分的配置, 首先是新建表
#然后是各必须的配置数据
#文件配置部分没有包含在这里
#文件配置文件需要以下内容: 西门子Q3部分.... 
#爱立信：
# 需要复制 order_???.cfg

hlrstr=$1
hlrcode=$2
region_code=$3
regionname=$4
copyhlr=$5

if [ "$hlrstr" = "" -o "$regionname" = "" ]; then
 echo "         Usage: $0 <hlr_codestr> <hlrcode> <region_code> <region_name> <参照的HLRstr>"
 echo "   Example: $0 hax X 07 泸州 hba"
 exit 0
fi

SQLCONN="sqlplus -s offon/opr_offon292@offondb"

$SQLCONN <<!

CREATE TABLE SSUBCMD${hlrstr}
(
  STREAM_ID     NUMBER(16)                      NOT NULL,
  COMMAND_ID    NUMBER(16)                      NOT NULL,
  COMMAND_CODE  CHAR(4 BYTE)                    NOT NULL,
  ORDER_CODE    CHAR(4 BYTE)                    NOT NULL,
  SERIAL        NUMBER(2)                       NOT NULL,
  MSISDN        CHAR(15 BYTE)                   NOT NULL,
  IMSI_NO       CHAR(20 BYTE)                   NOT NULL,
  SS_INFO1      CHAR(15 BYTE)                   NOT NULL,
  SS_INFO2      CHAR(20 BYTE)                   NOT NULL,
  SS_INFO3      VARCHAR2(120 BYTE),
  REPLY_CODE    CHAR(4 BYTE)                    NOT NULL,
  REPLY_TIME    DATE                            NOT NULL,
  PARSE_CODE    CHAR(4 BYTE)                    NOT NULL
)
TABLESPACE OFFON;


CREATE INDEX SSUBCMD${hlrstr}_IDX ON SSUBCMD${hlrstr}
(STREAM_ID)
TABLESPACE OFFON_IDX;


CREATE INDEX SSUBCMD${hlrstr}_IDX1 ON SSUBCMD${hlrstr}
(COMMAND_ID)
TABLESPACE OFFON_IDX;


CREATE INDEX SSUBCMD${hlrstr}_IDX2 ON SSUBCMD${hlrstr}
(REPLY_TIME, ORDER_CODE)
TABLESPACE OFFON_IDX;


CREATE TABLE SNDCMDDAY${hlrstr}
(
  COMMAND_ID       NUMBER(14)                   NOT NULL,
  HLR_CODE         CHAR(2 BYTE)                 NOT NULL,
  COMMAND_CODE     CHAR(2 BYTE)                 NOT NULL,
  COMMAND_ORDER    NUMBER(10)                   NOT NULL,
  ID_NO            NUMBER(14)                   NOT NULL,
  BELONG_CODE      CHAR(7 BYTE)                 NOT NULL,
  SM_CODE          CHAR(2 BYTE)                 NOT NULL,
  PHONE_NO         CHAR(15 BYTE)                NOT NULL,
  NEW_PHONE        CHAR(15 BYTE)                NOT NULL,
  IMSI_NO          CHAR(20 BYTE)                NOT NULL,
  NEW_IMSI         CHAR(20 BYTE)                NOT NULL,
  OTHER_CHAR       CHAR(40 BYTE)                NOT NULL,
  OP_CODE          CHAR(4 BYTE)                 NOT NULL,
  TOTAL_DATE       NUMBER(8)                    NOT NULL,
  OP_TIME          DATE                         NOT NULL,
  LOGIN_NO         CHAR(6 BYTE)                 NOT NULL,
  ORG_CODE         CHAR(9 BYTE)                 NOT NULL,
  LOGIN_ACCEPT     NUMBER(14)                   NOT NULL,
  REQUEST_TIME     DATE                         NOT NULL,
  BUSINESS_STATUS  CHAR(1 BYTE)                 NOT NULL,
  SEND_STATUS      CHAR(1 BYTE)                 NOT NULL,
  SEND_TIME        DATE                         NOT NULL,
  GROUP_ID         VARCHAR2(10 BYTE),
  ORG_ID           VARCHAR2(10 BYTE)
)
TABLESPACE OFFON;


CREATE INDEX SNDCMDDAY${hlrstr}_IDX1 ON SNDCMDDAY${hlrstr}
(COMMAND_ID)
TABLESPACE OFFON_IDX;


CREATE INDEX SNDCMDDAY${hlrstr}_IDX2 ON SNDCMDDAY${hlrstr}
(SEND_STATUS)
TABLESPACE OFFON_IDX;

CREATE TABLE DUSRDATA${hlrstr}
(
  PART_NUM      CHAR(1 BYTE)                    NOT NULL,
  MSISDN        CHAR(15 BYTE)                   NOT NULL,
  FUNC_CODE     CHAR(4 BYTE)                    NOT NULL,
  FUNC_STATUS   CHAR(1 BYTE)                    NOT NULL,
  OP_TIME       DATE                            NOT NULL,
  REQUEST_TIME  CHAR(14 BYTE)                   NOT NULL,
  SS_INFO       VARCHAR2(128 BYTE)
)
TABLESPACE OFFON
PARTITION BY RANGE (PART_NUM) 
(  
  PARTITION PART_0 VALUES LESS THAN ('1')
    TABLESPACE OFFON,
  PARTITION PART_1 VALUES LESS THAN ('2')
    TABLESPACE OFFON,
  PARTITION PART_2 VALUES LESS THAN ('3')
    TABLESPACE OFFON,
  PARTITION PART_3 VALUES LESS THAN ('4')
    TABLESPACE OFFON,
  PARTITION PART_4 VALUES LESS THAN ('5')
    TABLESPACE OFFON,
  PARTITION PART_5 VALUES LESS THAN ('6')
    TABLESPACE OFFON,
  PARTITION PART_6 VALUES LESS THAN ('7')
    TABLESPACE OFFON,
  PARTITION PART_7 VALUES LESS THAN ('8')
    TABLESPACE OFFON,
  PARTITION PART_8 VALUES LESS THAN ('9')
    TABLESPACE OFFON,
  PARTITION PART_9 VALUES LESS THAN ('A')
    TABLESPACE OFFON
);


CREATE INDEX DUSRDATA${hlrstr}_IDX1 ON DUSRDATA${hlrstr}
(PART_NUM, MSISDN)
TABLESPACE OFFON_IDX;



CREATE TABLE SOPRCMD${hlrstr}
(
  STREAM_ID       NUMBER(16)                    NOT NULL,
  COMMAND_ID      NUMBER(16)                    NOT NULL,
  TERM_ID         CHAR(6 BYTE)                  NOT NULL,
  MSISDN          CHAR(15 BYTE)                 NOT NULL,
  IMSI_NO         CHAR(20 BYTE)                 NOT NULL,
  OP_CODE         CHAR(4 BYTE)                  NOT NULL,
  COMMAND_CODE    CHAR(4 BYTE)                  NOT NULL,
  CMD_STATUS      CHAR(1 BYTE)                  NOT NULL,
  PHONE_PRIORITY  CHAR(1 BYTE)                  NOT NULL,
  REQUEST_TIME    DATE                          NOT NULL,
  SS_INFO1        CHAR(15 BYTE)                 NOT NULL,
  SS_INFO2        CHAR(20 BYTE)                 NOT NULL,
  SS_INFO3        VARCHAR2(120 BYTE),
  REPLY_CODE      CHAR(4 BYTE)                  NOT NULL,
  REPLY_TIME      DATE                          NOT NULL,
  REPLY_INFO      VARCHAR2(200 BYTE),
  BAK_STATUS      CHAR(1 BYTE)                  NOT NULL,
  BAK_TIME        DATE                          NOT NULL,
  LOGIN_NO        CHAR(6 BYTE)                  NOT NULL
)
TABLESPACE OFFON;

CREATE INDEX SOPRCMD${hlrstr}_IDX6 ON SOPRCMD${hlrstr}
(CMD_STATUS)
TABLESPACE OFFON;


CREATE INDEX SOPRCMD${hlrstr}_IDX3 ON SOPRCMD${hlrstr}
(REPLY_TIME, REPLY_CODE, CMD_STATUS)
TABLESPACE OFFON;


CREATE INDEX SOPRCMD${hlrstr}_IDX ON SOPRCMD${hlrstr}
(STREAM_ID)
TABLESPACE OFFON;


CREATE INDEX SOPRCMD${hlrstr}_IDX5 ON SOPRCMD${hlrstr}
(MSISDN)
TABLESPACE OFFON;


insert into cBossUserInfo values('${hlrstr}', 'offondb', 'offon', 'opr_offon292', 'sndcmdday${hlrstr}', '${regionname}');

insert into cQueryParse(hlr_code, command_code, order_code, src_beg, data_len, dest_station, dest_beg, note)
    select '${hlrstr}', command_code, order_code, src_beg, data_len, dest_station, dest_beg, note from cQueryParse
  where hlr_code='${copyhlr}';

insert into cQueryRevise(hlr_code, command_code, order_code, reply_code, func_code,
                status_flag, info_flag, status_addr, status_null, info_beg, info_len, note)
        select '${hlrstr}', command_code, order_code, reply_code, func_code,
                status_flag, info_flag, status_addr, status_null, info_beg, info_len, note
  from cQueryRevise where hlr_code='${copyhlr}';

insert into cOrderResend
        select '${hlrstr}', order_code, func_code from cOrderResend where hlr_code='${copyhlr}';

insert into cOrderReplyParse
        select '${hlrstr}', command_code, src_order_code, src_reply_code, parse_result, revise, dest_order, serial, order_type
                 from cOrderReplyParse where hlr_code='${copyhlr}';

insert into cCmdToOrder select '${hlrstr}', command_code, command_priority, order_type, order_code, serial, '${hlrstr}'
        from cCmdToOrder where hlr_code='${copyhlr}';

insert into cMainCfgInfo(hlr_code, ip_addr, comm_port, usrdata_mgr, note)
  values('${hlrstr}', '127.0.0.1', (select max(comm_port)+1 from cMainCfgInfo), 'n', '${regionname}');

insert into sphonecomm (QUERY_TYPE, PHONE_HEAD, IP_ADDR, COMM_PORT, SERVER_NAME, BAK_FIELD, DEFAULT_FLAG, STOP_LIMIT)
values ('${region_code}', '${hlrcode}', '0000', 100, '开关机批量指令复制控制', '0', 0, 0);

--双通道必须的表
insert into shlrcodemap (REGION_CODE, BO_HLR_CODE, OF_HLR_CODE, HLR_NAME, DUL_DIP, DUL_DPORT, DUL_RPORT, DUL_STATUS, HLR_CODE)
select '${region_code}', '${hlrcode}', substr('$hlrstr',2,2), '${regionname}', '10.109.2.164', mport, mport, '1', '${hlrstr}' from
(select max(dul_dport)+1 mport from shlrcodemap);


!

echo "配置 cLoginUser:"
for hlr_port  in 1 2 3 4 5 6
do
$SQLCONN <<!
insert into cLoginUser(user_name, user_pswd, hlr_code, hlr_port, term_type, encrypt, term_id)
  values('hlr${hlrstr}', 'hlr${hlrstr}', '${hlrstr}', ${hlr_port}, '1', '0', 'hlr${hlrstr}');
!
done


echo "配置 cHlrCfgInfo:"
echo "insert into cHlrCfgInfo(hlr_code, hlr_port, gsm_ip, gsm_port, gsm_user, gsm_pswd,"
echo " order_cfg, reply_cfg, query_cfg, note) values('${hlrstr}', ..."
echo "insert into cHlrCfgInfo(hlr_code, hlr_port, gsm_ip, gsm_port, gsm_user, gsm_pswd,
 order_cfg, reply_cfg, query_cfg, note) values('${hlrstr}', 1, 'ip', 0, 'user', 'pswd',
  '/offon/etc/q3_nj2_1.cfg',
  '/offon/etc/reply_q3_sim.cfg',
  '/offon/etc/query_q3_nj2.cfg', '内江');"
echo "insert into cHlrCfgInfo(hlr_code, hlr_port, gsm_ip, gsm_port, gsm_user, gsm_pswd,
 order_cfg, reply_cfg, query_cfg, note) values('${hlrstr}', 6, '10.109.178.72', 19896, 'KJTJ#1', 'KJTJ#1',
  '/offon/etc/order_cu_px.cfg',
  '/offon/etc/reply_cu_sim.cfg',
  '/offon/etc/query_cu_px.cfg', '成都郫县');"


echo "配置号段信息 cPhoneHlrCode:"
echo "insert into cPhoneHlrCode values('\$phno_head', '${hlrstr}', '1', '\$region_code', '${regionname}');"
echo "手工配置hlr_name.txt"
echo "手工配置hlrcode.txt"
echo "更新双通道环境的共享内存配置: dul_shmtool updateall"


'cBossUserInfo
'cQueryParse
'cQueryRevise
'cOrderResend
'cOrderReplyParse
'cCmdToOrder
'cMainCfgInfo
'sphonecomm
'shlrcodemap
'cLoginUser
'cPhoneHlrCode
