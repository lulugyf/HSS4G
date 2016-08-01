drop table cbossuserinfo;
drop table cqueryparse;
drop table cqueryrevise;
drop table corderresend;
drop table corderreplyparse;
drop table ccmdtoorder;
drop table cmaincfginfo;
drop table sphonecomm;
drop table shlrcodemap;
drop table cloginuser;
drop table cphonehlrcode;
drop table CORDERREDUCE;
drop table CUSRDATAMGR;
drop table COPCODEPRIORITY;

-- Create table
create table CMAINCFGINFO
(
  hlr_code    CHAR(3) not null,
  ip_addr     CHAR(16) not null,
  comm_port   NUMBER(8) not null,
  usrdata_mgr CHAR(1) not null,
  note        CHAR(256)
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Create/Recreate indexes 
create unique index CMAINCFGINFO_IDX1 on CMAINCFGINFO (HLR_CODE, COMM_PORT)
  tablespace OFFON_IDX
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
create unique index CMAINCFGINFO_IDX2 on CMAINCFGINFO (COMM_PORT)
  tablespace OFFON_IDX
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Grant/Revoke object privileges 
-- grant insert, update, delete on CMAINCFGINFO to "WEIHU-ROLE";

-- Create table
create table CBOSSUSERINFO
(
  hlr_code CHAR(3) not null,
  dbname   CHAR(30) not null,
  dbuser   CHAR(30) not null,
  dbpswd   CHAR(30) not null,
  tname    CHAR(64) not null,
  note     CHAR(128)
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Grant/Revoke object privileges 
-- grant insert, update, delete on CBOSSUSERINFO to "WEIHU-ROLE";


-- Create table
create table CQUERYPARSE
(
  hlr_code     CHAR(3) not null,
  command_code CHAR(4) not null,
  order_code   CHAR(4) not null,
  src_beg      NUMBER(3) not null,
  data_len     NUMBER(3) not null,
  dest_station NUMBER(1) not null,
  dest_beg     NUMBER(3) not null,
  note         CHAR(32) not null
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Grant/Revoke object privileges 
-- grant insert, update, delete on CQUERYPARSE to "WEIHU-ROLE";


-- Create table
create table CQUERYREVISE
(
  hlr_code     CHAR(3) not null,
  command_code CHAR(4) not null,
  order_code   CHAR(4) not null,
  reply_code   CHAR(4) not null,
  func_code    CHAR(4) not null,
  status_flag  CHAR(1) not null,
  info_flag    CHAR(1) not null,
  status_addr  NUMBER(3) not null,
  status_null  CHAR(1) not null,
  info_beg     NUMBER(3) not null,
  info_len     NUMBER(3) not null,
  note         CHAR(32)
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Create/Recreate indexes 
create index CQUERYREVISE_IDX on CQUERYREVISE (HLR_CODE)
  tablespace OFFON_IDX
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Grant/Revoke object privileges 
-- grant insert, update, delete on CQUERYREVISE to "WEIHU-ROLE";


-- Create table
create table CORDERRESEND
(
  hlr_code   CHAR(3) not null,
  order_code CHAR(4) not null,
  func_code  CHAR(4) not null
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Create/Recreate indexes 
create index CORDERRESEND_IDX on CORDERRESEND (HLR_CODE)
  tablespace OFFON_IDX
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Grant/Revoke object privileges 
-- grant insert, update, delete on CORDERRESEND to "WEIHU-ROLE";


-- Create table
create table CORDERREPLYPARSE
(
  hlr_code       CHAR(3) not null,
  command_code   CHAR(4) not null,
  src_order_code CHAR(4) not null,
  src_reply_code CHAR(4) not null,
  parse_result   CHAR(4) not null,
  revise         CHAR(1) not null,
  dest_order     CHAR(4) not null,
  serial         NUMBER(2) not null,
  order_type     CHAR(1) not null
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Create/Recreate indexes 
create index CORDERREPLYPARSE_IDX1 on CORDERREPLYPARSE (HLR_CODE)
  tablespace OFFON_IDX
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Grant/Revoke object privileges 
-- grant insert, update, delete on CORDERREPLYPARSE to "WEIHU-ROLE";

-- Create table
create table CCMDTOORDER
(
  hlr_code         CHAR(3) not null,
  command_code     CHAR(4) not null,
  command_priority NUMBER(4) not null,
  order_type       CHAR(1) not null,
  order_code       CHAR(4) not null,
  serial           NUMBER(2) not null,
  note             CHAR(128)
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
create index CCMDTOORDER_idx1 on CCMDTOORDER(hlr_code) tablespace offon_idx;

  
-- Create table
create table SPHONECOMM
(
  query_type   CHAR(2) not null,
  phone_head   CHAR(7) not null,
  ip_addr      CHAR(15) not null,
  comm_port    NUMBER not null,
  server_name  CHAR(32) not null,
  bak_field    CHAR(20),
  default_flag INTEGER not null,
  stop_limit   NUMBER(8) not null
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Grant/Revoke object privileges 
-- grant insert, update, delete on SPHONECOMM to "WEIHU-ROLE";


-- Create table
create table SHLRCODEMAP
(
  region_code CHAR(2),
  bo_hlr_code CHAR(2),
  of_hlr_code CHAR(2),
  hlr_name    CHAR(20),
  dul_dip     CHAR(30),
  dul_dport   NUMBER(6),
  dul_rport   NUMBER(6),
  dul_status  CHAR(1),
  hlr_code    CHAR(3) not null
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Add comments to the columns 
comment on column SHLRCODEMAP.dul_dip
  is '双通道同步数据备用侧ip';
comment on column SHLRCODEMAP.dul_dport
  is '双通道同步数据备用侧端口';
comment on column SHLRCODEMAP.dul_rport
  is '双通道同步数据作为备用侧接收同步数据的端口';
comment on column SHLRCODEMAP.dul_status
  is '双通道启用状态 1-启用 2-备用 0-无双通道';
-- Create/Recreate primary, unique and foreign key constraints 
alter table SHLRCODEMAP
  add constraint SHLRCODEMAP_PK primary key (HLR_CODE);
-- Create/Recreate indexes 
create index IDX1_SHLRCODEMAP on SHLRCODEMAP (HLR_CODE)
  tablespace OFFON_IDX
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
create index IDX2_SHLRCODEMAP on SHLRCODEMAP (BO_HLR_CODE)
  tablespace OFFON_IDX
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Grant/Revoke object privileges 
-- grant select on SHLRCODEMAP to AIOXMONITOR;
-- grant select on SHLRCODEMAP to CAPES;


-- Create table
create table CLOGINUSER
(
  user_name CHAR(8) not null,
  user_pswd CHAR(8) not null,
  hlr_code  CHAR(3) not null,
  hlr_port  CHAR(1) not null,
  term_type CHAR(1) not null,
  encrypt   CHAR(1) not null,
  term_id   CHAR(8) not null
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Grant/Revoke object privileges 
-- grant insert, update, delete on CLOGINUSER to "WEIHU-ROLE";

-- Create table
create table CPHONEHLRCODE
(
  phone_head  CHAR(9) not null,
  hlr_code    CHAR(3) not null,
  phone_type  CHAR(3) not null,
  region_code CHAR(2) not null,
  note        CHAR(32)
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Create/Recreate indexes 
create unique index CPHONEHLRCODE_IDX on CPHONEHLRCODE (HLR_CODE, PHONE_HEAD)
  tablespace OFFON_IDX
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Grant/Revoke object privileges 
-- grant select on CPHONEHLRCODE to BNMS;
-- grant insert, update, delete on CPHONEHLRCODE to "WEIHU-ROLE";

create table CORDERREDUCE
(
  hlr_code       CHAR(3) not null,
  old_order_code CHAR(4) not null,
  new_order_code CHAR(4) not null,
  operate_code   CHAR(1) not null
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
create index CORDERREDUCE_idx1 on CORDERREDUCE(hlr_code) tablespace offon_idx;

-- Create table
create table CUSRDATAMGR
(
  hlr_code    CHAR(3) not null,
  order_code  CHAR(4) not null,
  func_code   CHAR(4) not null,
  func_status CHAR(1) not null,
  ss_info     VARCHAR2(128),
  note        VARCHAR2(128)
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Create/Recreate indexes 
create index CUSRDATAMGR_IDX on CUSRDATAMGR (HLR_CODE)
  tablespace OFFON_IDX
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );

-- Create table
create table COPCODEPRIORITY
(
  op_code  CHAR(4) not null,
  priority NUMBER(4) not null,
  note     CHAR(64)
)
tablespace OFFON
  pctfree 10
  pctused 40
  initrans 1
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
-- Create/Recreate indexes 
create unique index COPCODEPRIORITY_IDX1 on COPCODEPRIORITY (OP_CODE)
  tablespace OFFON_IDX
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 1M
    next 1M
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
