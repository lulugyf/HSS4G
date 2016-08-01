-- 查询指令记录
select hlr_code from cphonehlrcode where trim(phone_head)=substr('13982074209', 1, 7);
select stream_id,command_id,msisdn,imsi_no,op_code, command_code,b.name, cmd_status,request_time,ss_info1,ss_info2,ss_info3,reply_code,reply_time,command_id from 
       soprcmdhc5 a left outer join ccommandcode b on b.cmd_code=a.command_code
        where msisdn='13982074209' order by stream_id desc;
        
update sndcmddayhei set send_status='0' where phone_no='15700240014' and command_id > 16012967640464
select * from sndcmddayhei where phone_no='15700240014' and command_id > 16012967640464 order by command_id

--create table ccommandcode (cmd_code char(3) primary key, name varchar2(128) not null, info varchar2(250) null, op_time date)
--insert into ccommandcode values('003', '删除IMS用户', 'VOLTE业务中删除IMS用户', sysdate)

select * from ccommandcode where cmd_code in ('556', '557', '559', '558', '463') for update
select * from ccommandcode where name like '%在线%'

select distinct command_code, note from ccmdtoorder where command_code in ('316', '499', '495', '314', '463')

select * from ccmdtoorder where  command_code='556' and hlr_code='hei'


select * from ccmdtoorder where command_code='557'

select * from cphonehlrcode where hlr_code='hei' phone_head='1570024'

create table ccmdtoorder_hei_20160125 as
select * from ccmdtoorder where hlr_code='hei' and command_code in ('257', '357')



insert into ccmdtoorder
select 'hei', command_code, command_priority, order_type, order_code, serial, note from ccmdtoorder where hlr_code='hbn'

select * from sndcmddayhei

-- 发送指令
insert into sndcmddayhei (COMMAND_ID, HLR_CODE, COMMAND_CODE, BUSINESS_STATUS, COMMAND_ORDER, ID_NO, BELONG_CODE, SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR, OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE, LOGIN_ACCEPT, REQUEST_TIME, SEND_STATUS, SEND_TIME, GROUP_ID, ORG_ID)
select cmd_seq.nextval, 'ei', '52', '0', 1, 1, '3', 'sm', '15700240041', '0', '460077296271684', '0', 'null', '1104', 1, sysdate, 'xxxxxx', 'org', 1, sysdate, '0', sysdate, 'group', 'org' from dual;

create sequence cmd_seq

select * from cphonehlrcode where phone_head='1570024'
insert into cphonehlrcode values('1570024', 'hei', '1', '02', '绵阳')

select (max(reply_time)-min(reply_time))*3600*24
 from soprcmdhc6 a left outer join ccommandcode b on b.cmd_code=a.command_code
 where msisdn='13982775530' and login_no='aagh0e' and op_code='0000';
 
select msisdn, request_time, command_code, reply_time, reply_code
 from soprcmdhc6 a left outer join ccommandcode b on b.cmd_code=a.command_code
 where reply_time between to_date('2016-6-24 00:00:00', 'yyyy-mm-dd HH24:mi:ss') and to_date('2016-6-25 00:00:00', 'yyyy-mm-dd HH24:mi:ss')
  and op_code='0000' and command_code in ('012', '338')
  order by request_time, msisdn, reply_time


select * from soprcmdhei

INSERT INTO sOprCmdhei (stream_id,command_id,term_id,
					msisdn,imsi_no, op_code, command_code, cmd_status, phone_priority,
					ss_info1,ss_info2, ss_info3,request_time,reply_code,bak_status, 
					bak_time,login_no, reply_time, reply_info) 
					VALUES(:streamid, :command_id, 'def', :phone_no, :imsi_no, 
					:op_code, :cmdcode, :cmd_status, :priority, :ss_info1, :ss_info2, 
					:ss_info3, to_date(:req_time, 'yyyymmddhh24miss'), :retn, '0', 
				sysdate,:login_no, to_date(:reply_time, 'yyyymmddhh24miss'), :reply_info)
        


-- 计算两个指令的时间间隔， volte的紧挨着的两个 hss 和 tas 指令， 用以衡量tas指令积压情况
select msisdn, request_time, command_code, to_char(reply_time,'yyyy-mm-dd HH24:mi:ss'), reply_code
 from soprcmdhc6 a left outer join ccommandcode b on b.cmd_code=a.command_code
 where reply_time between to_date('2016-6-29 00:00:00', 'yyyy-mm-dd HH24:mi:ss') and to_date('2016-6-30 00:00:00', 'yyyy-mm-dd HH24:mi:ss')
  and op_code='0000' and command_code in ('012', '338')
  order by request_time, msisdn, reply_time
