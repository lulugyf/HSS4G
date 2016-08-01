
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
TABLESPACE OFFON;

CREATE INDEX DUSRDATA${hlrstr}_IDX1 ON DUSRDATA${hlrstr}
(PART_NUM, MSISDN)
TABLESPACE OFFON_IDX;
