
/* Result Sets Interface */
#ifndef SQL_CRSR
#  define SQL_CRSR
  struct sql_cursor
  {
    unsigned int curocn;
    void *ptr1;
    void *ptr2;
    unsigned int magic;
  };
  typedef struct sql_cursor sql_cursor;
  typedef struct sql_cursor SQL_CURSOR;
#endif /* SQL_CRSR */

/* Thread Safety */
typedef void * sql_context;
typedef void * SQL_CONTEXT;

/* Object support */
struct sqltvn
{
  unsigned char *tvnvsn; 
  unsigned short tvnvsnl; 
  unsigned char *tvnnm;
  unsigned short tvnnml; 
  unsigned char *tvnsnm;
  unsigned short tvnsnml;
};
typedef struct sqltvn sqltvn;

struct sqladts
{
  unsigned int adtvsn; 
  unsigned short adtmode; 
  unsigned short adtnum;  
  sqltvn adttvn[1];       
};
typedef struct sqladts sqladts;

static struct sqladts sqladt = {
  1,1,0,
};

/* Binding to PL/SQL Records */
struct sqltdss
{
  unsigned int tdsvsn; 
  unsigned short tdsnum; 
  unsigned char *tdsval[1]; 
};
typedef struct sqltdss sqltdss;
static struct sqltdss sqltds =
{
  1,
  0,
};

/* File name & Package Name */
struct sqlcxp
{
  unsigned short fillen;
           char  filnam[15];
};
static struct sqlcxp sqlfpn =
{
    14,
    "offon_fordb.pc"
};


static unsigned int sqlctx = 1241067;


static struct sqlexd {
   unsigned long  sqlvsn;
   unsigned int   arrsiz;
   unsigned int   iters;
   unsigned int   offset;
   unsigned short selerr;
   unsigned short sqlety;
   unsigned int   occurs;
            short *cud;
   unsigned char  *sqlest;
            char  *stmt;
   sqladts *sqladtp;
   sqltdss *sqltdsp;
   unsigned char  **sqphsv;
   unsigned long  *sqphsl;
            int   *sqphss;
            short **sqpind;
            int   *sqpins;
   unsigned long  *sqparm;
   unsigned long  **sqparc;
   unsigned short  *sqpadto;
   unsigned short  *sqptdso;
   unsigned int   sqlcmax;
   unsigned int   sqlcmin;
   unsigned int   sqlcincr;
   unsigned int   sqlctimeout;
   unsigned int   sqlcnowait;
            int   sqfoff;
   unsigned int   sqcmod;
   unsigned int   sqfmod;
   unsigned char  *sqhstv[16];
   unsigned long  sqhstl[16];
            int   sqhsts[16];
            short *sqindv[16];
            int   sqinds[16];
   unsigned long  sqharm[16];
   unsigned long  *sqharc[16];
   unsigned short  sqadto[16];
   unsigned short  sqtdso[16];
} sqlstm = {12,16};

/* SQLLIB Prototypes */
extern sqlcxt ( void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * );
extern sqlcx2t( void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * );
extern sqlbuft( void **, char * );
extern sqlgs2t( void **, char * );
extern sqlorat( void **, unsigned int *, void * );

/* Forms Interface */
static int IAPSUCC = 0;
static int IAPFAIL = 1403;
static int IAPFTL  = 535;
extern void sqliem( unsigned char *, signed int * );

typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* CUD (Compilation Unit Data) Array */
static short sqlcud0[] =
{12,4130,873,8,0,
5,0,0,1,0,0,17,561,0,0,1,1,0,1,0,1,97,0,0,
24,0,0,1,0,0,21,562,0,0,15,15,0,1,0,1,3,0,0,1,97,0,0,1,97,0,0,1,3,0,0,1,97,0,0,
1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,1,0,
0,1,97,0,0,
99,0,0,2,0,0,17,584,0,0,1,1,0,1,0,1,97,0,0,
118,0,0,2,0,0,21,585,0,0,16,16,0,1,0,1,3,0,0,1,3,0,0,1,97,0,0,1,97,0,0,1,97,0,
0,1,97,0,0,1,1,0,0,1,1,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,
0,1,97,0,0,1,97,0,0,
197,0,0,3,0,0,17,602,0,0,1,1,0,1,0,1,97,0,0,
216,0,0,3,0,0,21,603,0,0,1,1,0,1,0,1,3,0,0,
235,0,0,4,0,0,17,619,0,0,1,1,0,1,0,1,97,0,0,
254,0,0,4,0,0,21,620,0,0,4,4,0,1,0,1,97,0,0,1,97,0,0,1,97,0,0,1,3,0,0,
285,0,0,5,0,0,17,637,0,0,1,1,0,1,0,1,97,0,0,
304,0,0,5,0,0,21,638,0,0,12,12,0,1,0,1,3,0,0,1,3,0,0,1,97,0,0,1,97,0,0,1,1,0,0,
1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,
367,0,0,6,0,0,29,649,0,0,0,0,0,1,0,
};


#line 1 "offon_fordb.pc"
/*********************************************************
 *
 *  FileName:
 *      offon_fordb.pc
 *
 *  Version:
 *      2006-6-1 17:59
 *
 *  tabstop = 4
 *
 ********************************************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "sqlca.h"
#include "sqlda.h"

#include "file_func.h"
#include "db_func.h"

/* EXEC SQL INCLUDE SQLCA;
 */ 
#line 1 "/usr/lib/oracle/cli/rdbms/public/sqlca.h"
/*
 * $Header: sqlca.h 24-apr-2003.12:50:58 mkandarp Exp $ sqlca.h 
 */

/* Copyright (c) 1985, 2003, Oracle Corporation.  All rights reserved.  */
 
/*
NAME
  SQLCA : SQL Communications Area.
FUNCTION
  Contains no code. Oracle fills in the SQLCA with status info
  during the execution of a SQL stmt.
NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************

  If the symbol SQLCA_STORAGE_CLASS is defined, then the SQLCA
  will be defined to have this storage class. For example:
 
    #define SQLCA_STORAGE_CLASS extern
 
  will define the SQLCA as an extern.
 
  If the symbol SQLCA_INIT is defined, then the SQLCA will be
  statically initialized. Although this is not necessary in order
  to use the SQLCA, it is a good pgming practice not to have
  unitialized variables. However, some C compilers/OS's don't
  allow automatic variables to be init'd in this manner. Therefore,
  if you are INCLUDE'ing the SQLCA in a place where it would be
  an automatic AND your C compiler/OS doesn't allow this style
  of initialization, then SQLCA_INIT should be left undefined --
  all others can define SQLCA_INIT if they wish.

  If the symbol SQLCA_NONE is defined, then the SQLCA variable will
  not be defined at all.  The symbol SQLCA_NONE should not be defined
  in source modules that have embedded SQL.  However, source modules
  that have no embedded SQL, but need to manipulate a sqlca struct
  passed in as a parameter, can set the SQLCA_NONE symbol to avoid
  creation of an extraneous sqlca variable.
 
MODIFIED
    lvbcheng   07/31/98 -  long to int
    jbasu      12/12/94 -  Bug 217878: note this is an SOSD file
    losborne   08/11/92 -  No sqlca var if SQLCA_NONE macro set 
  Clare      12/06/84 - Ch SQLCA to not be an extern.
  Clare      10/21/85 - Add initialization.
  Bradbury   01/05/86 - Only initialize when SQLCA_INIT set
  Clare      06/12/86 - Add SQLCA_STORAGE_CLASS option.
*/
 
#ifndef SQLCA
#define SQLCA 1
 
struct   sqlca
         {
         /* ub1 */ char    sqlcaid[8];
         /* b4  */ int     sqlabc;
         /* b4  */ int     sqlcode;
         struct
           {
           /* ub2 */ unsigned short sqlerrml;
           /* ub1 */ char           sqlerrmc[70];
           } sqlerrm;
         /* ub1 */ char    sqlerrp[8];
         /* b4  */ int     sqlerrd[6];
         /* ub1 */ char    sqlwarn[8];
         /* ub1 */ char    sqlext[8];
         };

#ifndef SQLCA_NONE 
#ifdef   SQLCA_STORAGE_CLASS
SQLCA_STORAGE_CLASS struct sqlca sqlca
#else
         struct sqlca sqlca
#endif
 
#ifdef  SQLCA_INIT
         = {
         {'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '},
         sizeof(struct sqlca),
         0,
         { 0, {0}},
         {'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0}
         }
#endif
         ;
#endif
 
#endif
 
/* end SQLCA */
/*  */ 
#line 1 "/usr/lib/oracle/cli/rdbms/public/sqlca.h"
/*
 * $Header: sqlca.h 24-apr-2003.12:50:58 mkandarp Exp $ sqlca.h 
 */

/* Copyright (c) 1985, 2003, Oracle Corporation.  All rights reserved.  */
 
/*
NAME
  SQLCA : SQL Communications Area.
FUNCTION
  Contains no code. Oracle fills in the SQLCA with status info
  during the execution of a SQL stmt.
NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************

  If the symbol SQLCA_STORAGE_CLASS is defined, then the SQLCA
  will be defined to have this storage class. For example:
 
    #define SQLCA_STORAGE_CLASS extern
 
  will define the SQLCA as an extern.
 
  If the symbol SQLCA_INIT is defined, then the SQLCA will be
  statically initialized. Although this is not necessary in order
  to use the SQLCA, it is a good pgming practice not to have
  unitialized variables. However, some C compilers/OS's don't
  allow automatic variables to be init'd in this manner. Therefore,
  if you are INCLUDE'ing the SQLCA in a place where it would be
  an automatic AND your C compiler/OS doesn't allow this style
  of initialization, then SQLCA_INIT should be left undefined --
  all others can define SQLCA_INIT if they wish.

  If the symbol SQLCA_NONE is defined, then the SQLCA variable will
  not be defined at all.  The symbol SQLCA_NONE should not be defined
  in source modules that have embedded SQL.  However, source modules
  that have no embedded SQL, but need to manipulate a sqlca struct
  passed in as a parameter, can set the SQLCA_NONE symbol to avoid
  creation of an extraneous sqlca variable.
 
MODIFIED
    lvbcheng   07/31/98 -  long to int
    jbasu      12/12/94 -  Bug 217878: note this is an SOSD file
    losborne   08/11/92 -  No sqlca var if SQLCA_NONE macro set 
  Clare      12/06/84 - Ch SQLCA to not be an extern.
  Clare      10/21/85 - Add initialization.
  Bradbury   01/05/86 - Only initialize when SQLCA_INIT set
  Clare      06/12/86 - Add SQLCA_STORAGE_CLASS option.
*/
 
#ifndef SQLCA
#define SQLCA 1
 
struct   sqlca
         {
         /* ub1 */ char    sqlcaid[8];
         /* b4  */ int     sqlabc;
         /* b4  */ int     sqlcode;
         struct
           {
           /* ub2 */ unsigned short sqlerrml;
           /* ub1 */ char           sqlerrmc[70];
           } sqlerrm;
         /* ub1 */ char    sqlerrp[8];
         /* b4  */ int     sqlerrd[6];
         /* ub1 */ char    sqlwarn[8];
         /* ub1 */ char    sqlext[8];
         };

#ifndef SQLCA_NONE 
#ifdef   SQLCA_STORAGE_CLASS
SQLCA_STORAGE_CLASS struct sqlca sqlca
#else
         struct sqlca sqlca
#endif
 
#ifdef  SQLCA_INIT
         = {
         {'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '},
         sizeof(struct sqlca),
         0,
         { 0, {0}},
         {'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0}
         }
#endif
         ;
#endif
 
#endif
 
/* end SQLCA */

#line 22 "offon_fordb.pc"
char hlrcode[8];
FILE *logfp;


#define SQLERRMSG sqlca.sqlerrm.sqlerrmc

extern int db_logout();


extern int GenDaemon();
extern int db_login();

void usage(char *s)
{
	printf("%s\n\nUsage:offon_fordb -Hhlrcode [-NS(not import ssubcmd)] [-V]\n\n\n", s);
	exit(1);
}


long g_fpos;
void LogOut(int sig)
{
    FILE *fp;
    char fname[128];

	printf("LogOut signal==%d\n",sig);
    fflush(stdout);

	exit(1);
}

#define MAXLINE 500


struct
{
	char rectype[20];
	char hlrcode[6];
	time_t rec_time;
	int rc4opr;
	int tm4opr;
	int rc4opru;
	int tm4opru;
	int rc4ssub;
	int tm4ssub;
	int rc4snd;
	int tm4snd;
	int tm4fil;
} yy_time;

void yy_time_log(time_t t)
{
	char h[5];
	if(t-yy_time.rec_time >= 300){
		file_log("perf",
				"rectime=%ld,rectype=fordb,hlrcode=%s,rc4opr=%d,"
				"tm4opr=%d,rc4opru=%d,tm4opru=%d,rc4ssub=%d,tm4ssub=%d"
				",rc4snd=%d,tm4snd=%d,tm4fil=%d",
				t, yy_time.hlrcode, yy_time.rc4opr, yy_time.tm4opr/1000,
				yy_time.rc4opru, yy_time.tm4opru/1000,
				yy_time.rc4ssub, yy_time.tm4ssub/1000,
				yy_time.rc4snd, yy_time.tm4snd/1000, yy_time.tm4fil/1000);
		strcpy(h, yy_time.hlrcode);
		memset(&yy_time, 0, sizeof(yy_time));
		strcpy(yy_time.hlrcode, h);
		yy_time.rec_time = t;
	}
}

static int splitc(char *string, char *fields[], int  nfields, char sep )
{
    char *p, *p1;
    int i;

    p = string;
    fields[0] = p;
    i = 1;
    while( (p1=strchr(p,sep))!= NULL && i<nfields){
        p1[0] = 0; p1++;
        fields[i] = p1;
        p = p1; i++;
    }
    return i;
}


int sav_file_pos(char *hlr_code, char *date, long rpos){
	FILE *fp;
	char fname[128];

	sprintf(fname, "%s/.%s.pos1",
			getenv("DATABUF"), hlr_code);
	fp = fopen(fname, "w");
	if(fp == NULL)
		return -1;
	fprintf(fp, "%s %ld\n", date, rpos);
	fclose(fp);
	return 0;
}


int io_check_read (int *fds, int count, int timeout)
{
    fd_set rfd;
    int i, ret, mfid, j;
    struct timeval tvl;

    mfid = 0;
    FD_ZERO (&rfd);
    for (i = 0; i < count; i++)
    {
        FD_SET (fds[i], &rfd);
        mfid = (fds[i] > mfid) ? fds[i] : mfid;
    }
    mfid++;
    tvl.tv_sec = timeout / 1000;
    tvl.tv_usec = (timeout % 1000) * 1000;

    ret = select (mfid, &rfd, NULL, NULL, &tvl);
    fprintf(logfp, "select return %d\n", ret);
    fflush(logfp);
    if (ret <= 0)
    {
        return ret;
    }
    j = 0;
    for (i = 0; i < count; i++)
    {
        if (FD_ISSET (fds[i], &rfd))
        {
              fds[j++] = fds[i];
        }
    }
    return ret;
}

struct tp_soprcmd
{
	long stream_id;
	long command_id;
	char msisdn[16];
	char imsi_no[22];
	char op_code[5];
	char cmdcode[4];
	char cmd_status;
	char priority;
	char ss_info1[16];
	char ss_info2[22];
	char ss_info3[122];
	char req_time[16];
	char retn[5];
	char login_no[7];
	char reply_time[16];
	char reply_info[200];
};

struct tp_soprcmdu
{
	char retn[5];
	char reply_time[16];
	char reply_info[200];
	long stream_id;
};

struct tp_ssubcmd
{
	long stream_id;
	long command_id;
	char cmdcode[4];
	char order_code[5];
	char serial;
	char msisdn[16];
	char imsi_no[21];
	char ss_info1[16];
	char ss_info2[22];
	char ss_info3[122];
	char retn[5];
	char parse_code[5];
};



struct tp_sndcmd
{
	long command_id;
	char hlr_code[3];
	char command_code[3];
	long id_no;
	char belong_code[8];
	char phone_no[16];
	char new_phone[16];
	char imsi_no[21];
	char new_imsi[21];
	char other_char[21];
	char op_code[5];
	char login_no[7];
	char request_time[15];
	char business_status;
	char group_id[11];
};


struct tp_databuf
{
	struct tp_soprcmd *soprcmd;
	int ct_sopr;
	struct tp_soprcmdu *soprcmdu;
	int ct_sopru;
	struct tp_ssubcmd *ssubcmd;
	int ct_ssubcmd;
	struct tp_sndcmd *sndcmd;
	int ct_sndcmd;
} databuf;

int read_buf(char *hlr_code, char *sdate, long *prpos, int *fno)
{
	static FILE *fp = NULL;
	static char cdate[9];
	static long rpos;
	int count = 0, llen, i;
	char filename[128];
	char line[2048], line1[2048];
	char timestr[30];
	struct tp_soprcmd *opr;
	struct tp_soprcmdu *popru, opru;
	struct tp_ssubcmd *sub;
	struct tp_sndcmd *snd;
	char *rp[30], *ptr;
	int data_flag = 0;
	struct timeval t1, t2;
	int x;

	gettimeofday(&t1, NULL);
	filename[0] = 0;
	databuf.ct_sopr = 0;
	databuf.ct_sopru = 0;
	databuf.ct_ssubcmd = 0;
	databuf.ct_sndcmd = 0;
	if(fp == NULL){
		cdate[0] = 0;
		get_file_pos(hlr_code, cdate, &rpos);
		get_data_file(filename, hlr_code, cdate);
		fp = fopen(filename, "r");
		if(fp != NULL && rpos > 0)
			fseek(fp, rpos, SEEK_SET);
		fprintf(logfp, "get_read_pos date:%s, fpos:%ld fp is null :%d\n", cdate, rpos, fp==NULL);
		fflush(logfp);
	}
	while(fp == NULL){
		rpos = 0;
		cdate[0] = 0;
		get_data_file(filename, hlr_code, cdate);
		fp = fopen(filename, "r");
		if(fp == NULL){
			fprintf(logfp, "fopen datafile111 [%s] fail\n", filename);
			fflush(logfp);
			sleep(1);
			continue;
		}
		fprintf(logfp, "open file %s, rpos:%ld\n", filename, rpos);
		fflush(logfp);
	}
	/*memset(&databuf, 0, sizeof(databuf)); */
	count = 0;
	while( (ptr = fgets(line, sizeof(line)-1, fp)) != NULL)
	{
		data_flag = 1;
		llen = strlen(line);
		if(line[llen-1] != '\n')
			continue;
		line[llen-1] = 0;
		strcpy(line1, line);
		llen = splitc(line+1, rp, 30, '|');
        fflush(stdout);
		if(line[0] == 'I'){
            if(llen < 14)
                continue;
			opr = &databuf.soprcmd[databuf.ct_sopr++];
			memset(opr, 0, sizeof(struct tp_soprcmd));
			opr->stream_id = atol(rp[0]);
			opr->command_id = atol(rp[1]);
			strcpy(opr->msisdn, rp[2]);
			strcpy(opr->imsi_no, rp[3]);
			strcpy(opr->op_code, rp[4]);
			sprintf(opr->cmdcode, "%s%c", rp[5], rp[6][0]);
			opr->cmd_status = '0';
			opr->priority = rp[7][0];
			strcpy(opr->ss_info1, rp[8]);
			strcpy(opr->ss_info2, rp[9]);
			strcpy(opr->ss_info3, rp[10]);
			strcpy(opr->req_time, rp[11]);
			strcpy(opr->retn, rp[12]);
			strcpy(opr->login_no, rp[13]);
			if(atoi(opr->retn) != 0)
				opr->cmd_status = '1';
			strcpy(opr->reply_time, opr->req_time);
			strcpy(opr->reply_info, "00");
		}else if(line[0] == 'U'){
			int xx = 0;
            if(llen < 4) continue;
			opru.stream_id = atol(rp[0]);
			strcpy(opru.retn, rp[1]);
			strcpy(opru.reply_time, rp[2]);
			strcpy(opru.reply_info, rp[3]);
			/*查找对应的insert记录*/
			for(i=0; i<databuf.ct_sopr; i++){
				opr = &databuf.soprcmd[i];
				if(opr->stream_id == opru.stream_id){
					strcpy(opr->retn, opru.retn);
					strcpy(opr->reply_time, opru.reply_time);
					strcpy(opr->reply_info, opru.reply_info);
					opr->cmd_status='1';
					xx = 1;
					break;
				}
			}
			if(xx == 0){
				/*没找到*/
				popru = &databuf.soprcmdu[databuf.ct_sopru++];
				memcpy(popru, &opru, sizeof(opru));
			}
		}else if(line[0] == 'S'){
            if(llen < 12) continue;
			sub = &databuf.ssubcmd[databuf.ct_ssubcmd++];
			sub->stream_id = atol(rp[0]);
			sub->command_id = atol(rp[1]);
			strcpy(sub->cmdcode, rp[2]);
			strcpy(sub->order_code, rp[3]);
			sub->serial = rp[4][0];
			strcpy(sub->msisdn, rp[5]);
			strcpy(sub->imsi_no, rp[6]);
			strcpy(sub->ss_info1, rp[7]);
			strcpy(sub->ss_info2, rp[8]);
			strcpy(sub->ss_info3, rp[9]);
			strcpy(sub->retn, rp[10]);
			strcpy(sub->parse_code, rp[11]);
		}else if(line[0] == 'D'){
            if(llen < 15) continue;
			/*D200150150612|X|56|1|hba-hc5|13880076349|13880076349|460008830666387|460008830666387|0|3212|vvvvvv|20081225191757|0|group*/
			snd = &databuf.sndcmd[databuf.ct_sndcmd++];
			snd->command_id = atol(rp[0]);
			strcpy(snd->hlr_code, rp[1]);
			strcpy(snd->command_code, rp[2]);
			snd->id_no = atol(rp[3]);
			strcpy(snd->belong_code, rp[4]);
			strcpy(snd->phone_no, rp[5]);
			strcpy(snd->new_phone, rp[6]);
			strcpy(snd->imsi_no, rp[7]);
			strcpy(snd->new_imsi, rp[8]);
			strcpy(snd->other_char, rp[9]);
			strcpy(snd->op_code, rp[10]);
			strcpy(snd->login_no, rp[11]);
			strcpy(snd->request_time, rp[12]);
			snd->business_status = rp[13][0];
			strcpy(snd->group_id, rp[14]);
		}
		if(++ count >= MAXLINE)
			break;
	}
    *fno = -1;
    /*fprintf(logfp, "readbuf: count:%d, ptr is null: %d\n",count, ptr==NULL);
    fflush(logfp); */
	if(ptr == NULL){
		/*已经读到文件末尾了, 检查是否需要切换文件*/
		char date[9];
		get_cur_timestr(date, 1);
		if(strcmp(date, cdate) != 0){
			char oldfile[128], bakfile[128];
			fclose(fp);
			/*移动旧文件到bak目录*/
			get_data_file(oldfile, hlr_code, cdate);
			sprintf(bakfile, "%s/databuf/bak/%s.%s", getenv("HOME"), hlr_code, cdate);
			rename(oldfile, bakfile);
			rpos = 0;
			get_data_file(filename, hlr_code, date);
			strcpy(cdate, date);
			fp = fopen(filename, "r");
		}else{
            *fno = fileno(fp);
        }
	}
	if(fp != NULL){
		rpos = ftell(fp);
	}
	strcpy(sdate, cdate);
	*prpos = rpos;
	gettimeofday(&t2, NULL);
	x = time_diff1(t1, t2);
	if(data_flag == 1)
		yy_time.tm4fil += x;
	return data_flag;
}


FILE * open_logfp(time_t t, FILE *fp1, char *hlr_code)
{
	struct tm *tv;
	char fname[128];

	static int curt = 0;

	tv=localtime(&t);
	if(tv->tm_mday != curt)
	{
		sprintf(fname,"%s/fdb%04d%02d%02d.%s",
				getenv("LOGDIR"), tv->tm_year+1900, tv->tm_mon+1,
				tv->tm_mday, hlr_code);
		if(fp1 != NULL)
			fclose(fp1);
		fp1 = fopen(fname,"a");
		if(fp1 == NULL)
		{
			printf("can't open %s for LOG[%d]\n", fname, errno);
			exit(1);
		}
		curt = tv->tm_mday;
	}
	return fp1;
}

int main(int argc,char **argv)
{

	int ret,count;
	int i;
	FILE *cfgfp;
	char cfgstr[256];
	char dbuser[32],dbpswd[32],dbname[32],bossdb[32],tname[64];
	char logfilename[256],logdir[256];
	time_t t;
	char tempstr[1024];
	char buf[1024], *rp[30];
	char cmdcode[4];
	char cmd_status[2];
    long c1, c2;
    time_t t1, t2, cur_tm;
    int counter = 0;
	int rec_ct;
	struct tp_soprcmd soprcmd[MAXLINE];
	struct tp_soprcmdu soprcmdu[MAXLINE];
	struct tp_ssubcmd ssubcmd[MAXLINE];
	struct tp_sndcmd sndcmd[MAXLINE];
	long cmdids[MAXLINE];
	int no_ssubcmd = 0; /*如果为1, 则不入库 ssubcmd数据*/
	struct timeval tv1, tv2;

	if(argc<2)
		usage("Incorrect Parameter!");

	GenDaemon();

	signal(SIGPIPE,SIG_IGN);
	signal(SIGCLD,SIG_IGN);
	signal(SIGINT,SIG_IGN);

	while(--argc>0)
	{
		if(argv[argc][0]!='-')
			usage("Incorrect Parameter!");

		switch(argv[argc][1])
		{
		case 'H':
			strncpy(hlrcode, argv[argc]+2,4);
			break;
		case 'V':
		case 'v':
			printf("\n\n	version = %s\n\n",VERSION);
			exit(0);
		case 'N':
			if(argv[argc][2] == 'S')
				no_ssubcmd = 1;
			break;
		default:
			usage("Unknow Parameter!");
			break;
		}
	}

	if(strlen(hlrcode) != 3 )
		usage("Incorrect parameter [hlrcode]!\n");

	logfp = open_logfp(time(NULL), NULL, hlrcode);
	set_hlrcode(hlrcode);

	if(logindb("gdblogin.cfg") != SQL_OK)
	{
		fprintf(logfp,"db_login(%s,%s,%s) failed!\n",dbuser,dbpswd,dbname);
		exit(1);
	}
    fprintf(logfp, "db login success\n");
    fflush(logfp);

    strcpy(yy_time.hlrcode, hlrcode);
    yy_time.rec_time = cur_tm = time(NULL);
    c2 = c1 = 0;
    counter = 0;
    time(&t1); t2 = t1;
	signal(SIGTERM,LogOut);

	databuf.soprcmd = soprcmd;
	databuf.soprcmdu = soprcmdu;
	databuf.ssubcmd = ssubcmd;
	databuf.sndcmd = sndcmd;
	while(1){
		long rpos = 0;
		char sdate[10];
        int fno[2];
		memset(buf, 0, sizeof(buf));
		if((t1 = time(NULL))-cur_tm > 20){ /*记录性能日志和检查日志文件翻转*/
			yy_time_log(t1);
			logfp = open_logfp(t1, logfp, hlrcode);
			cur_tm = t1;
		}

		ret = read_buf(hlrcode, sdate, &rpos, &fno[0]);
		if(ret == 0){
			sleep(1);
            /*if(fno[0] != -1){
                io_check_read(fno, 1, 5000);
            } */
			continue;
		}

		rec_ct = databuf.ct_sndcmd;
		if(rec_ct > 0){
			yy_time.rc4snd += rec_ct;
			gettimeofday(&tv1, NULL);
			sprintf(tempstr,"INSERT INTO sndcmdday%s (COMMAND_ID,HLR_CODE,COMMAND_CODE,COMMAND_ORDER,ID_NO,"
		    "BELONG_CODE,SM_CODE,PHONE_NO,NEW_PHONE,IMSI_NO, "
		     " NEW_IMSI,OTHER_CHAR,OP_CODE,TOTAL_DATE,OP_TIME, "
		      "LOGIN_NO,ORG_CODE,LOGIN_ACCEPT,REQUEST_TIME,BUSINESS_STATUS, "
		      "SEND_STATUS,SEND_TIME,GROUP_ID,ORG_ID) "
		      "VALUES(:v0,:v1,:v2,1,:v3,"
		      ":v4,'sm',:v5,:v6,:v7,"
		      ":v8,:v9,:v10,1,sysdate,"
		      ":v11,'org',1,to_date(:v12,'yyyymmddhh24miss'),:v13,"
		      "'0',sysdate,:v14,'org')",
		          hlrcode);
		    /* EXEC SQL PREPARE stmt4 FROM :tempstr; */ 
#line 561 "offon_fordb.pc"

{
#line 561 "offon_fordb.pc"
      struct sqlexd sqlstm;
#line 561 "offon_fordb.pc"
      sqlstm.sqlvsn = 12;
#line 561 "offon_fordb.pc"
      sqlstm.arrsiz = 1;
#line 561 "offon_fordb.pc"
      sqlstm.sqladtp = &sqladt;
#line 561 "offon_fordb.pc"
      sqlstm.sqltdsp = &sqltds;
#line 561 "offon_fordb.pc"
      sqlstm.stmt = "";
#line 561 "offon_fordb.pc"
      sqlstm.iters = (unsigned int  )1;
#line 561 "offon_fordb.pc"
      sqlstm.offset = (unsigned int  )5;
#line 561 "offon_fordb.pc"
      sqlstm.cud = sqlcud0;
#line 561 "offon_fordb.pc"
      sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 561 "offon_fordb.pc"
      sqlstm.sqlety = (unsigned short)4352;
#line 561 "offon_fordb.pc"
      sqlstm.occurs = (unsigned int  )0;
#line 561 "offon_fordb.pc"
      sqlstm.sqhstv[0] = (unsigned char  *)tempstr;
#line 561 "offon_fordb.pc"
      sqlstm.sqhstl[0] = (unsigned long )1024;
#line 561 "offon_fordb.pc"
      sqlstm.sqhsts[0] = (         int  )0;
#line 561 "offon_fordb.pc"
      sqlstm.sqindv[0] = (         short *)0;
#line 561 "offon_fordb.pc"
      sqlstm.sqinds[0] = (         int  )0;
#line 561 "offon_fordb.pc"
      sqlstm.sqharm[0] = (unsigned long )0;
#line 561 "offon_fordb.pc"
      sqlstm.sqadto[0] = (unsigned short )0;
#line 561 "offon_fordb.pc"
      sqlstm.sqtdso[0] = (unsigned short )0;
#line 561 "offon_fordb.pc"
      sqlstm.sqphsv = sqlstm.sqhstv;
#line 561 "offon_fordb.pc"
      sqlstm.sqphsl = sqlstm.sqhstl;
#line 561 "offon_fordb.pc"
      sqlstm.sqphss = sqlstm.sqhsts;
#line 561 "offon_fordb.pc"
      sqlstm.sqpind = sqlstm.sqindv;
#line 561 "offon_fordb.pc"
      sqlstm.sqpins = sqlstm.sqinds;
#line 561 "offon_fordb.pc"
      sqlstm.sqparm = sqlstm.sqharm;
#line 561 "offon_fordb.pc"
      sqlstm.sqparc = sqlstm.sqharc;
#line 561 "offon_fordb.pc"
      sqlstm.sqpadto = sqlstm.sqadto;
#line 561 "offon_fordb.pc"
      sqlstm.sqptdso = sqlstm.sqtdso;
#line 561 "offon_fordb.pc"
      sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 561 "offon_fordb.pc"
}

#line 561 "offon_fordb.pc"

		    /* EXEC SQL for :rec_ct EXECUTE stmt4 USING :sndcmd; */ 
#line 562 "offon_fordb.pc"

{
#line 562 "offon_fordb.pc"
      struct sqlexd sqlstm;
#line 562 "offon_fordb.pc"
      sqlstm.sqlvsn = 12;
#line 562 "offon_fordb.pc"
      sqlstm.arrsiz = 15;
#line 562 "offon_fordb.pc"
      sqlstm.sqladtp = &sqladt;
#line 562 "offon_fordb.pc"
      sqlstm.sqltdsp = &sqltds;
#line 562 "offon_fordb.pc"
      sqlstm.stmt = "";
#line 562 "offon_fordb.pc"
      sqlstm.iters = (unsigned int  )rec_ct;
#line 562 "offon_fordb.pc"
      sqlstm.offset = (unsigned int  )24;
#line 562 "offon_fordb.pc"
      sqlstm.cud = sqlcud0;
#line 562 "offon_fordb.pc"
      sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 562 "offon_fordb.pc"
      sqlstm.sqlety = (unsigned short)4352;
#line 562 "offon_fordb.pc"
      sqlstm.occurs = (unsigned int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[0] = (unsigned char  *)&sndcmd->command_id;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[0] = (unsigned long )sizeof(long);
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[0] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[0] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[0] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[0] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[0] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[0] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[0] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[1] = (unsigned char  *)sndcmd->hlr_code;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[1] = (unsigned long )3;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[1] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[1] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[1] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[1] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[1] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[1] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[1] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[2] = (unsigned char  *)sndcmd->command_code;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[2] = (unsigned long )3;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[2] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[2] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[2] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[2] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[2] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[2] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[2] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[3] = (unsigned char  *)&sndcmd->id_no;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[3] = (unsigned long )sizeof(long);
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[3] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[3] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[3] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[3] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[3] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[3] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[3] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[4] = (unsigned char  *)sndcmd->belong_code;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[4] = (unsigned long )8;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[4] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[4] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[4] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[4] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[4] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[4] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[4] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[5] = (unsigned char  *)sndcmd->phone_no;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[5] = (unsigned long )16;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[5] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[5] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[5] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[5] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[5] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[5] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[5] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[6] = (unsigned char  *)sndcmd->new_phone;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[6] = (unsigned long )16;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[6] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[6] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[6] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[6] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[6] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[6] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[6] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[7] = (unsigned char  *)sndcmd->imsi_no;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[7] = (unsigned long )21;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[7] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[7] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[7] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[7] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[7] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[7] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[7] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[8] = (unsigned char  *)sndcmd->new_imsi;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[8] = (unsigned long )21;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[8] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[8] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[8] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[8] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[8] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[8] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[8] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[9] = (unsigned char  *)sndcmd->other_char;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[9] = (unsigned long )21;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[9] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[9] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[9] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[9] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[9] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[9] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[9] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[10] = (unsigned char  *)sndcmd->op_code;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[10] = (unsigned long )5;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[10] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[10] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[10] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[10] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[10] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[10] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[10] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[11] = (unsigned char  *)sndcmd->login_no;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[11] = (unsigned long )7;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[11] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[11] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[11] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[11] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[11] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[11] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[11] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[12] = (unsigned char  *)sndcmd->request_time;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[12] = (unsigned long )15;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[12] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[12] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[12] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[12] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[12] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[12] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[12] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[13] = (unsigned char  *)&sndcmd->business_status;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[13] = (unsigned long )1;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[13] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[13] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[13] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[13] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[13] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[13] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[13] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstv[14] = (unsigned char  *)sndcmd->group_id;
#line 562 "offon_fordb.pc"
      sqlstm.sqhstl[14] = (unsigned long )11;
#line 562 "offon_fordb.pc"
      sqlstm.sqhsts[14] = (         int  )sizeof(struct tp_sndcmd);
#line 562 "offon_fordb.pc"
      sqlstm.sqindv[14] = (         short *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqinds[14] = (         int  )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharm[14] = (unsigned long )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqharc[14] = (unsigned long  *)0;
#line 562 "offon_fordb.pc"
      sqlstm.sqadto[14] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqtdso[14] = (unsigned short )0;
#line 562 "offon_fordb.pc"
      sqlstm.sqphsv = sqlstm.sqhstv;
#line 562 "offon_fordb.pc"
      sqlstm.sqphsl = sqlstm.sqhstl;
#line 562 "offon_fordb.pc"
      sqlstm.sqphss = sqlstm.sqhsts;
#line 562 "offon_fordb.pc"
      sqlstm.sqpind = sqlstm.sqindv;
#line 562 "offon_fordb.pc"
      sqlstm.sqpins = sqlstm.sqinds;
#line 562 "offon_fordb.pc"
      sqlstm.sqparm = sqlstm.sqharm;
#line 562 "offon_fordb.pc"
      sqlstm.sqparc = sqlstm.sqharc;
#line 562 "offon_fordb.pc"
      sqlstm.sqpadto = sqlstm.sqadto;
#line 562 "offon_fordb.pc"
      sqlstm.sqptdso = sqlstm.sqtdso;
#line 562 "offon_fordb.pc"
      sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 562 "offon_fordb.pc"
}

#line 562 "offon_fordb.pc"

			if(SQLCODE != SQL_OK){
				fprintf(logfp, "fail insert sndcmd %d[%s]\n", SQLCODE, SQLERRMSG);
				fflush(logfp);
			}
			gettimeofday(&tv2, NULL);
			yy_time.tm4snd += time_diff1(tv1, tv2);
		}

		rec_ct = databuf.ct_sopr;
		if(rec_ct > 0){
			yy_time.rc4opr += rec_ct;
			gettimeofday(&tv1, NULL);
			sprintf(tempstr,"INSERT INTO sOprCmd%s (stream_id,command_id,term_id,"
					"msisdn,imsi_no, op_code, command_code, cmd_status, phone_priority,"
					"ss_info1,ss_info2, ss_info3,request_time,reply_code,bak_status, "
					"bak_time,login_no, reply_time, reply_info) "
					"VALUES(:streamid, :command_id, 'def', :phone_no, :imsi_no, "
					":op_code, :cmdcode, :cmd_status, :priority, :ss_info1, :ss_info2, "
					":ss_info3, to_date(:req_time, 'yyyymmddhh24miss'), :retn, '0', "
					"sysdate,:login_no, to_date(:reply_time, 'yyyymmddhh24miss'), :reply_info)",
				hlrcode);
			/* EXEC SQL PREPARE stmt1 FROM :tempstr; */ 
#line 584 "offon_fordb.pc"

{
#line 584 "offon_fordb.pc"
   struct sqlexd sqlstm;
#line 584 "offon_fordb.pc"
   sqlstm.sqlvsn = 12;
#line 584 "offon_fordb.pc"
   sqlstm.arrsiz = 15;
#line 584 "offon_fordb.pc"
   sqlstm.sqladtp = &sqladt;
#line 584 "offon_fordb.pc"
   sqlstm.sqltdsp = &sqltds;
#line 584 "offon_fordb.pc"
   sqlstm.stmt = "";
#line 584 "offon_fordb.pc"
   sqlstm.iters = (unsigned int  )1;
#line 584 "offon_fordb.pc"
   sqlstm.offset = (unsigned int  )99;
#line 584 "offon_fordb.pc"
   sqlstm.cud = sqlcud0;
#line 584 "offon_fordb.pc"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 584 "offon_fordb.pc"
   sqlstm.sqlety = (unsigned short)4352;
#line 584 "offon_fordb.pc"
   sqlstm.occurs = (unsigned int  )0;
#line 584 "offon_fordb.pc"
   sqlstm.sqhstv[0] = (unsigned char  *)tempstr;
#line 584 "offon_fordb.pc"
   sqlstm.sqhstl[0] = (unsigned long )1024;
#line 584 "offon_fordb.pc"
   sqlstm.sqhsts[0] = (         int  )0;
#line 584 "offon_fordb.pc"
   sqlstm.sqindv[0] = (         short *)0;
#line 584 "offon_fordb.pc"
   sqlstm.sqinds[0] = (         int  )0;
#line 584 "offon_fordb.pc"
   sqlstm.sqharm[0] = (unsigned long )0;
#line 584 "offon_fordb.pc"
   sqlstm.sqadto[0] = (unsigned short )0;
#line 584 "offon_fordb.pc"
   sqlstm.sqtdso[0] = (unsigned short )0;
#line 584 "offon_fordb.pc"
   sqlstm.sqphsv = sqlstm.sqhstv;
#line 584 "offon_fordb.pc"
   sqlstm.sqphsl = sqlstm.sqhstl;
#line 584 "offon_fordb.pc"
   sqlstm.sqphss = sqlstm.sqhsts;
#line 584 "offon_fordb.pc"
   sqlstm.sqpind = sqlstm.sqindv;
#line 584 "offon_fordb.pc"
   sqlstm.sqpins = sqlstm.sqinds;
#line 584 "offon_fordb.pc"
   sqlstm.sqparm = sqlstm.sqharm;
#line 584 "offon_fordb.pc"
   sqlstm.sqparc = sqlstm.sqharc;
#line 584 "offon_fordb.pc"
   sqlstm.sqpadto = sqlstm.sqadto;
#line 584 "offon_fordb.pc"
   sqlstm.sqptdso = sqlstm.sqtdso;
#line 584 "offon_fordb.pc"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 584 "offon_fordb.pc"
}

#line 584 "offon_fordb.pc"

			/* EXEC SQL for :rec_ct EXECUTE stmt1 USING :soprcmd; */ 
#line 585 "offon_fordb.pc"

{
#line 585 "offon_fordb.pc"
   struct sqlexd sqlstm;
#line 585 "offon_fordb.pc"
   sqlstm.sqlvsn = 12;
#line 585 "offon_fordb.pc"
   sqlstm.arrsiz = 16;
#line 585 "offon_fordb.pc"
   sqlstm.sqladtp = &sqladt;
#line 585 "offon_fordb.pc"
   sqlstm.sqltdsp = &sqltds;
#line 585 "offon_fordb.pc"
   sqlstm.stmt = "";
#line 585 "offon_fordb.pc"
   sqlstm.iters = (unsigned int  )rec_ct;
#line 585 "offon_fordb.pc"
   sqlstm.offset = (unsigned int  )118;
#line 585 "offon_fordb.pc"
   sqlstm.cud = sqlcud0;
#line 585 "offon_fordb.pc"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 585 "offon_fordb.pc"
   sqlstm.sqlety = (unsigned short)4352;
#line 585 "offon_fordb.pc"
   sqlstm.occurs = (unsigned int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[0] = (unsigned char  *)&soprcmd->stream_id;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[0] = (unsigned long )sizeof(long);
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[0] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[0] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[0] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[0] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[0] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[0] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[0] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[1] = (unsigned char  *)&soprcmd->command_id;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[1] = (unsigned long )sizeof(long);
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[1] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[1] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[1] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[1] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[1] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[1] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[1] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[2] = (unsigned char  *)soprcmd->msisdn;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[2] = (unsigned long )16;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[2] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[2] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[2] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[2] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[2] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[2] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[2] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[3] = (unsigned char  *)soprcmd->imsi_no;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[3] = (unsigned long )22;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[3] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[3] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[3] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[3] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[3] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[3] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[3] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[4] = (unsigned char  *)soprcmd->op_code;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[4] = (unsigned long )5;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[4] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[4] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[4] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[4] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[4] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[4] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[4] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[5] = (unsigned char  *)soprcmd->cmdcode;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[5] = (unsigned long )4;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[5] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[5] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[5] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[5] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[5] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[5] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[5] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[6] = (unsigned char  *)&soprcmd->cmd_status;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[6] = (unsigned long )1;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[6] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[6] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[6] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[6] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[6] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[6] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[6] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[7] = (unsigned char  *)&soprcmd->priority;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[7] = (unsigned long )1;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[7] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[7] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[7] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[7] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[7] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[7] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[7] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[8] = (unsigned char  *)soprcmd->ss_info1;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[8] = (unsigned long )16;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[8] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[8] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[8] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[8] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[8] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[8] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[8] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[9] = (unsigned char  *)soprcmd->ss_info2;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[9] = (unsigned long )22;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[9] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[9] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[9] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[9] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[9] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[9] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[9] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[10] = (unsigned char  *)soprcmd->ss_info3;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[10] = (unsigned long )122;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[10] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[10] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[10] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[10] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[10] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[10] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[10] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[11] = (unsigned char  *)soprcmd->req_time;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[11] = (unsigned long )16;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[11] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[11] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[11] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[11] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[11] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[11] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[11] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[12] = (unsigned char  *)soprcmd->retn;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[12] = (unsigned long )5;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[12] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[12] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[12] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[12] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[12] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[12] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[12] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[13] = (unsigned char  *)soprcmd->login_no;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[13] = (unsigned long )7;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[13] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[13] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[13] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[13] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[13] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[13] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[13] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[14] = (unsigned char  *)soprcmd->reply_time;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[14] = (unsigned long )16;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[14] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[14] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[14] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[14] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[14] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[14] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[14] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstv[15] = (unsigned char  *)soprcmd->reply_info;
#line 585 "offon_fordb.pc"
   sqlstm.sqhstl[15] = (unsigned long )200;
#line 585 "offon_fordb.pc"
   sqlstm.sqhsts[15] = (         int  )sizeof(struct tp_soprcmd);
#line 585 "offon_fordb.pc"
   sqlstm.sqindv[15] = (         short *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqinds[15] = (         int  )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharm[15] = (unsigned long )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqharc[15] = (unsigned long  *)0;
#line 585 "offon_fordb.pc"
   sqlstm.sqadto[15] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqtdso[15] = (unsigned short )0;
#line 585 "offon_fordb.pc"
   sqlstm.sqphsv = sqlstm.sqhstv;
#line 585 "offon_fordb.pc"
   sqlstm.sqphsl = sqlstm.sqhstl;
#line 585 "offon_fordb.pc"
   sqlstm.sqphss = sqlstm.sqhsts;
#line 585 "offon_fordb.pc"
   sqlstm.sqpind = sqlstm.sqindv;
#line 585 "offon_fordb.pc"
   sqlstm.sqpins = sqlstm.sqinds;
#line 585 "offon_fordb.pc"
   sqlstm.sqparm = sqlstm.sqharm;
#line 585 "offon_fordb.pc"
   sqlstm.sqparc = sqlstm.sqharc;
#line 585 "offon_fordb.pc"
   sqlstm.sqpadto = sqlstm.sqadto;
#line 585 "offon_fordb.pc"
   sqlstm.sqptdso = sqlstm.sqtdso;
#line 585 "offon_fordb.pc"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 585 "offon_fordb.pc"
}

#line 585 "offon_fordb.pc"

			if(SQLCODE != SQL_OK){
				fprintf(logfp, "fail insert soprcmd %d[%s]\n", SQLCODE, SQLERRMSG);
				fflush(logfp);
			}
			for(i=0; i<rec_ct; i++){
				cmdids[i] = soprcmd[i].command_id;
				if(soprcmd[i].stream_id > 2100000000000){
					fprintf(logfp, "sssssss %ld %ld %s %s rc:%d idx:%d\n",
							soprcmd[i].stream_id, soprcmd[i].command_id, soprcmd[i].msisdn,
							soprcmd[i].cmdcode, rec_ct, i);

				}
			}
		    sprintf(tempstr, "UPDATE sndcmdday%s set send_status='1' "
		    		"where command_id=:command_id and send_status='0'",
		        hlrcode);
		    /* EXEC SQL PREPARE stmt5 FROM :tempstr; */ 
#line 602 "offon_fordb.pc"

{
#line 602 "offon_fordb.pc"
      struct sqlexd sqlstm;
#line 602 "offon_fordb.pc"
      sqlstm.sqlvsn = 12;
#line 602 "offon_fordb.pc"
      sqlstm.arrsiz = 16;
#line 602 "offon_fordb.pc"
      sqlstm.sqladtp = &sqladt;
#line 602 "offon_fordb.pc"
      sqlstm.sqltdsp = &sqltds;
#line 602 "offon_fordb.pc"
      sqlstm.stmt = "";
#line 602 "offon_fordb.pc"
      sqlstm.iters = (unsigned int  )1;
#line 602 "offon_fordb.pc"
      sqlstm.offset = (unsigned int  )197;
#line 602 "offon_fordb.pc"
      sqlstm.cud = sqlcud0;
#line 602 "offon_fordb.pc"
      sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 602 "offon_fordb.pc"
      sqlstm.sqlety = (unsigned short)4352;
#line 602 "offon_fordb.pc"
      sqlstm.occurs = (unsigned int  )0;
#line 602 "offon_fordb.pc"
      sqlstm.sqhstv[0] = (unsigned char  *)tempstr;
#line 602 "offon_fordb.pc"
      sqlstm.sqhstl[0] = (unsigned long )1024;
#line 602 "offon_fordb.pc"
      sqlstm.sqhsts[0] = (         int  )0;
#line 602 "offon_fordb.pc"
      sqlstm.sqindv[0] = (         short *)0;
#line 602 "offon_fordb.pc"
      sqlstm.sqinds[0] = (         int  )0;
#line 602 "offon_fordb.pc"
      sqlstm.sqharm[0] = (unsigned long )0;
#line 602 "offon_fordb.pc"
      sqlstm.sqadto[0] = (unsigned short )0;
#line 602 "offon_fordb.pc"
      sqlstm.sqtdso[0] = (unsigned short )0;
#line 602 "offon_fordb.pc"
      sqlstm.sqphsv = sqlstm.sqhstv;
#line 602 "offon_fordb.pc"
      sqlstm.sqphsl = sqlstm.sqhstl;
#line 602 "offon_fordb.pc"
      sqlstm.sqphss = sqlstm.sqhsts;
#line 602 "offon_fordb.pc"
      sqlstm.sqpind = sqlstm.sqindv;
#line 602 "offon_fordb.pc"
      sqlstm.sqpins = sqlstm.sqinds;
#line 602 "offon_fordb.pc"
      sqlstm.sqparm = sqlstm.sqharm;
#line 602 "offon_fordb.pc"
      sqlstm.sqparc = sqlstm.sqharc;
#line 602 "offon_fordb.pc"
      sqlstm.sqpadto = sqlstm.sqadto;
#line 602 "offon_fordb.pc"
      sqlstm.sqptdso = sqlstm.sqtdso;
#line 602 "offon_fordb.pc"
      sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 602 "offon_fordb.pc"
}

#line 602 "offon_fordb.pc"

		    /* EXEC SQL for :rec_ct EXECUTE stmt5 USING :cmdids; */ 
#line 603 "offon_fordb.pc"

{
#line 603 "offon_fordb.pc"
      struct sqlexd sqlstm;
#line 603 "offon_fordb.pc"
      sqlstm.sqlvsn = 12;
#line 603 "offon_fordb.pc"
      sqlstm.arrsiz = 16;
#line 603 "offon_fordb.pc"
      sqlstm.sqladtp = &sqladt;
#line 603 "offon_fordb.pc"
      sqlstm.sqltdsp = &sqltds;
#line 603 "offon_fordb.pc"
      sqlstm.stmt = "";
#line 603 "offon_fordb.pc"
      sqlstm.iters = (unsigned int  )rec_ct;
#line 603 "offon_fordb.pc"
      sqlstm.offset = (unsigned int  )216;
#line 603 "offon_fordb.pc"
      sqlstm.cud = sqlcud0;
#line 603 "offon_fordb.pc"
      sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 603 "offon_fordb.pc"
      sqlstm.sqlety = (unsigned short)4352;
#line 603 "offon_fordb.pc"
      sqlstm.occurs = (unsigned int  )0;
#line 603 "offon_fordb.pc"
      sqlstm.sqhstv[0] = (unsigned char  *)cmdids;
#line 603 "offon_fordb.pc"
      sqlstm.sqhstl[0] = (unsigned long )sizeof(long);
#line 603 "offon_fordb.pc"
      sqlstm.sqhsts[0] = (         int  )sizeof(long);
#line 603 "offon_fordb.pc"
      sqlstm.sqindv[0] = (         short *)0;
#line 603 "offon_fordb.pc"
      sqlstm.sqinds[0] = (         int  )0;
#line 603 "offon_fordb.pc"
      sqlstm.sqharm[0] = (unsigned long )0;
#line 603 "offon_fordb.pc"
      sqlstm.sqharc[0] = (unsigned long  *)0;
#line 603 "offon_fordb.pc"
      sqlstm.sqadto[0] = (unsigned short )0;
#line 603 "offon_fordb.pc"
      sqlstm.sqtdso[0] = (unsigned short )0;
#line 603 "offon_fordb.pc"
      sqlstm.sqphsv = sqlstm.sqhstv;
#line 603 "offon_fordb.pc"
      sqlstm.sqphsl = sqlstm.sqhstl;
#line 603 "offon_fordb.pc"
      sqlstm.sqphss = sqlstm.sqhsts;
#line 603 "offon_fordb.pc"
      sqlstm.sqpind = sqlstm.sqindv;
#line 603 "offon_fordb.pc"
      sqlstm.sqpins = sqlstm.sqinds;
#line 603 "offon_fordb.pc"
      sqlstm.sqparm = sqlstm.sqharm;
#line 603 "offon_fordb.pc"
      sqlstm.sqparc = sqlstm.sqharc;
#line 603 "offon_fordb.pc"
      sqlstm.sqpadto = sqlstm.sqadto;
#line 603 "offon_fordb.pc"
      sqlstm.sqptdso = sqlstm.sqtdso;
#line 603 "offon_fordb.pc"
      sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 603 "offon_fordb.pc"
}

#line 603 "offon_fordb.pc"

			if(SQLCODE != SQL_OK && SQLCODE != 1403){
				fprintf(logfp, "fail update sndcmd %d[%s]\n", SQLCODE, SQLERRMSG);
				fflush(logfp);
			}
			gettimeofday(&tv2, NULL);
			yy_time.tm4opr += time_diff1(tv1, tv2);
		}
		rec_ct = databuf.ct_sopru;
		if(rec_ct > 0){
			yy_time.rc4opru += rec_ct;
			gettimeofday(&tv1, NULL);
			sprintf(tempstr, "UPDATE sOprCmd%s SET cmd_status='1',reply_code=:v1, "
					"reply_time=to_date(:v4, 'yyyymmddhh24miss'), reply_info=:v2,bak_time=sysdate "
					"WHERE stream_id=to_number(:v3)",
				hlrcode);
			/* EXEC SQL PREPARE stmt2 FROM :tempstr; */ 
#line 619 "offon_fordb.pc"

{
#line 619 "offon_fordb.pc"
   struct sqlexd sqlstm;
#line 619 "offon_fordb.pc"
   sqlstm.sqlvsn = 12;
#line 619 "offon_fordb.pc"
   sqlstm.arrsiz = 16;
#line 619 "offon_fordb.pc"
   sqlstm.sqladtp = &sqladt;
#line 619 "offon_fordb.pc"
   sqlstm.sqltdsp = &sqltds;
#line 619 "offon_fordb.pc"
   sqlstm.stmt = "";
#line 619 "offon_fordb.pc"
   sqlstm.iters = (unsigned int  )1;
#line 619 "offon_fordb.pc"
   sqlstm.offset = (unsigned int  )235;
#line 619 "offon_fordb.pc"
   sqlstm.cud = sqlcud0;
#line 619 "offon_fordb.pc"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 619 "offon_fordb.pc"
   sqlstm.sqlety = (unsigned short)4352;
#line 619 "offon_fordb.pc"
   sqlstm.occurs = (unsigned int  )0;
#line 619 "offon_fordb.pc"
   sqlstm.sqhstv[0] = (unsigned char  *)tempstr;
#line 619 "offon_fordb.pc"
   sqlstm.sqhstl[0] = (unsigned long )1024;
#line 619 "offon_fordb.pc"
   sqlstm.sqhsts[0] = (         int  )0;
#line 619 "offon_fordb.pc"
   sqlstm.sqindv[0] = (         short *)0;
#line 619 "offon_fordb.pc"
   sqlstm.sqinds[0] = (         int  )0;
#line 619 "offon_fordb.pc"
   sqlstm.sqharm[0] = (unsigned long )0;
#line 619 "offon_fordb.pc"
   sqlstm.sqadto[0] = (unsigned short )0;
#line 619 "offon_fordb.pc"
   sqlstm.sqtdso[0] = (unsigned short )0;
#line 619 "offon_fordb.pc"
   sqlstm.sqphsv = sqlstm.sqhstv;
#line 619 "offon_fordb.pc"
   sqlstm.sqphsl = sqlstm.sqhstl;
#line 619 "offon_fordb.pc"
   sqlstm.sqphss = sqlstm.sqhsts;
#line 619 "offon_fordb.pc"
   sqlstm.sqpind = sqlstm.sqindv;
#line 619 "offon_fordb.pc"
   sqlstm.sqpins = sqlstm.sqinds;
#line 619 "offon_fordb.pc"
   sqlstm.sqparm = sqlstm.sqharm;
#line 619 "offon_fordb.pc"
   sqlstm.sqparc = sqlstm.sqharc;
#line 619 "offon_fordb.pc"
   sqlstm.sqpadto = sqlstm.sqadto;
#line 619 "offon_fordb.pc"
   sqlstm.sqptdso = sqlstm.sqtdso;
#line 619 "offon_fordb.pc"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 619 "offon_fordb.pc"
}

#line 619 "offon_fordb.pc"

			/* EXEC SQL for :rec_ct EXECUTE stmt2 USING :soprcmdu; */ 
#line 620 "offon_fordb.pc"

{
#line 620 "offon_fordb.pc"
   struct sqlexd sqlstm;
#line 620 "offon_fordb.pc"
   sqlstm.sqlvsn = 12;
#line 620 "offon_fordb.pc"
   sqlstm.arrsiz = 16;
#line 620 "offon_fordb.pc"
   sqlstm.sqladtp = &sqladt;
#line 620 "offon_fordb.pc"
   sqlstm.sqltdsp = &sqltds;
#line 620 "offon_fordb.pc"
   sqlstm.stmt = "";
#line 620 "offon_fordb.pc"
   sqlstm.iters = (unsigned int  )rec_ct;
#line 620 "offon_fordb.pc"
   sqlstm.offset = (unsigned int  )254;
#line 620 "offon_fordb.pc"
   sqlstm.cud = sqlcud0;
#line 620 "offon_fordb.pc"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 620 "offon_fordb.pc"
   sqlstm.sqlety = (unsigned short)4352;
#line 620 "offon_fordb.pc"
   sqlstm.occurs = (unsigned int  )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqhstv[0] = (unsigned char  *)soprcmdu->retn;
#line 620 "offon_fordb.pc"
   sqlstm.sqhstl[0] = (unsigned long )5;
#line 620 "offon_fordb.pc"
   sqlstm.sqhsts[0] = (         int  )sizeof(struct tp_soprcmdu);
#line 620 "offon_fordb.pc"
   sqlstm.sqindv[0] = (         short *)0;
#line 620 "offon_fordb.pc"
   sqlstm.sqinds[0] = (         int  )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqharm[0] = (unsigned long )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqharc[0] = (unsigned long  *)0;
#line 620 "offon_fordb.pc"
   sqlstm.sqadto[0] = (unsigned short )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqtdso[0] = (unsigned short )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqhstv[1] = (unsigned char  *)soprcmdu->reply_time;
#line 620 "offon_fordb.pc"
   sqlstm.sqhstl[1] = (unsigned long )16;
#line 620 "offon_fordb.pc"
   sqlstm.sqhsts[1] = (         int  )sizeof(struct tp_soprcmdu);
#line 620 "offon_fordb.pc"
   sqlstm.sqindv[1] = (         short *)0;
#line 620 "offon_fordb.pc"
   sqlstm.sqinds[1] = (         int  )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqharm[1] = (unsigned long )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqharc[1] = (unsigned long  *)0;
#line 620 "offon_fordb.pc"
   sqlstm.sqadto[1] = (unsigned short )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqtdso[1] = (unsigned short )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqhstv[2] = (unsigned char  *)soprcmdu->reply_info;
#line 620 "offon_fordb.pc"
   sqlstm.sqhstl[2] = (unsigned long )200;
#line 620 "offon_fordb.pc"
   sqlstm.sqhsts[2] = (         int  )sizeof(struct tp_soprcmdu);
#line 620 "offon_fordb.pc"
   sqlstm.sqindv[2] = (         short *)0;
#line 620 "offon_fordb.pc"
   sqlstm.sqinds[2] = (         int  )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqharm[2] = (unsigned long )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqharc[2] = (unsigned long  *)0;
#line 620 "offon_fordb.pc"
   sqlstm.sqadto[2] = (unsigned short )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqtdso[2] = (unsigned short )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqhstv[3] = (unsigned char  *)&soprcmdu->stream_id;
#line 620 "offon_fordb.pc"
   sqlstm.sqhstl[3] = (unsigned long )sizeof(long);
#line 620 "offon_fordb.pc"
   sqlstm.sqhsts[3] = (         int  )sizeof(struct tp_soprcmdu);
#line 620 "offon_fordb.pc"
   sqlstm.sqindv[3] = (         short *)0;
#line 620 "offon_fordb.pc"
   sqlstm.sqinds[3] = (         int  )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqharm[3] = (unsigned long )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqharc[3] = (unsigned long  *)0;
#line 620 "offon_fordb.pc"
   sqlstm.sqadto[3] = (unsigned short )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqtdso[3] = (unsigned short )0;
#line 620 "offon_fordb.pc"
   sqlstm.sqphsv = sqlstm.sqhstv;
#line 620 "offon_fordb.pc"
   sqlstm.sqphsl = sqlstm.sqhstl;
#line 620 "offon_fordb.pc"
   sqlstm.sqphss = sqlstm.sqhsts;
#line 620 "offon_fordb.pc"
   sqlstm.sqpind = sqlstm.sqindv;
#line 620 "offon_fordb.pc"
   sqlstm.sqpins = sqlstm.sqinds;
#line 620 "offon_fordb.pc"
   sqlstm.sqparm = sqlstm.sqharm;
#line 620 "offon_fordb.pc"
   sqlstm.sqparc = sqlstm.sqharc;
#line 620 "offon_fordb.pc"
   sqlstm.sqpadto = sqlstm.sqadto;
#line 620 "offon_fordb.pc"
   sqlstm.sqptdso = sqlstm.sqtdso;
#line 620 "offon_fordb.pc"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 620 "offon_fordb.pc"
}

#line 620 "offon_fordb.pc"

			if(SQLCODE != SQL_OK){
				fprintf(logfp, "fail update soprcmd %d[%s]\n", SQLCODE, SQLERRMSG);
				fflush(logfp);
			}
			gettimeofday(&tv2, NULL);
			yy_time.tm4opru += time_diff1(tv1, tv2);
		}
		rec_ct = databuf.ct_ssubcmd;
		if(rec_ct > 0){
			yy_time.rc4ssub += rec_ct;
			gettimeofday(&tv1, NULL);
			sprintf(tempstr,"INSERT INTO sSubCmd%s (stream_id,command_id,command_code,"
					"order_code,serial, msisdn,imsi_no,ss_info1,ss_info2,ss_info3,"
				" reply_code,reply_time,parse_code) "
				"VALUES(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:10,:v11,sysdate,:v12)",
				hlrcode);
			/* EXEC SQL PREPARE stmt3 FROM :tempstr; */ 
#line 637 "offon_fordb.pc"

{
#line 637 "offon_fordb.pc"
   struct sqlexd sqlstm;
#line 637 "offon_fordb.pc"
   sqlstm.sqlvsn = 12;
#line 637 "offon_fordb.pc"
   sqlstm.arrsiz = 16;
#line 637 "offon_fordb.pc"
   sqlstm.sqladtp = &sqladt;
#line 637 "offon_fordb.pc"
   sqlstm.sqltdsp = &sqltds;
#line 637 "offon_fordb.pc"
   sqlstm.stmt = "";
#line 637 "offon_fordb.pc"
   sqlstm.iters = (unsigned int  )1;
#line 637 "offon_fordb.pc"
   sqlstm.offset = (unsigned int  )285;
#line 637 "offon_fordb.pc"
   sqlstm.cud = sqlcud0;
#line 637 "offon_fordb.pc"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 637 "offon_fordb.pc"
   sqlstm.sqlety = (unsigned short)4352;
#line 637 "offon_fordb.pc"
   sqlstm.occurs = (unsigned int  )0;
#line 637 "offon_fordb.pc"
   sqlstm.sqhstv[0] = (unsigned char  *)tempstr;
#line 637 "offon_fordb.pc"
   sqlstm.sqhstl[0] = (unsigned long )1024;
#line 637 "offon_fordb.pc"
   sqlstm.sqhsts[0] = (         int  )0;
#line 637 "offon_fordb.pc"
   sqlstm.sqindv[0] = (         short *)0;
#line 637 "offon_fordb.pc"
   sqlstm.sqinds[0] = (         int  )0;
#line 637 "offon_fordb.pc"
   sqlstm.sqharm[0] = (unsigned long )0;
#line 637 "offon_fordb.pc"
   sqlstm.sqadto[0] = (unsigned short )0;
#line 637 "offon_fordb.pc"
   sqlstm.sqtdso[0] = (unsigned short )0;
#line 637 "offon_fordb.pc"
   sqlstm.sqphsv = sqlstm.sqhstv;
#line 637 "offon_fordb.pc"
   sqlstm.sqphsl = sqlstm.sqhstl;
#line 637 "offon_fordb.pc"
   sqlstm.sqphss = sqlstm.sqhsts;
#line 637 "offon_fordb.pc"
   sqlstm.sqpind = sqlstm.sqindv;
#line 637 "offon_fordb.pc"
   sqlstm.sqpins = sqlstm.sqinds;
#line 637 "offon_fordb.pc"
   sqlstm.sqparm = sqlstm.sqharm;
#line 637 "offon_fordb.pc"
   sqlstm.sqparc = sqlstm.sqharc;
#line 637 "offon_fordb.pc"
   sqlstm.sqpadto = sqlstm.sqadto;
#line 637 "offon_fordb.pc"
   sqlstm.sqptdso = sqlstm.sqtdso;
#line 637 "offon_fordb.pc"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 637 "offon_fordb.pc"
}

#line 637 "offon_fordb.pc"

			/* EXEC SQL for :rec_ct EXECUTE stmt3 USING :ssubcmd; */ 
#line 638 "offon_fordb.pc"

{
#line 638 "offon_fordb.pc"
   struct sqlexd sqlstm;
#line 638 "offon_fordb.pc"
   sqlstm.sqlvsn = 12;
#line 638 "offon_fordb.pc"
   sqlstm.arrsiz = 16;
#line 638 "offon_fordb.pc"
   sqlstm.sqladtp = &sqladt;
#line 638 "offon_fordb.pc"
   sqlstm.sqltdsp = &sqltds;
#line 638 "offon_fordb.pc"
   sqlstm.stmt = "";
#line 638 "offon_fordb.pc"
   sqlstm.iters = (unsigned int  )rec_ct;
#line 638 "offon_fordb.pc"
   sqlstm.offset = (unsigned int  )304;
#line 638 "offon_fordb.pc"
   sqlstm.cud = sqlcud0;
#line 638 "offon_fordb.pc"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 638 "offon_fordb.pc"
   sqlstm.sqlety = (unsigned short)4352;
#line 638 "offon_fordb.pc"
   sqlstm.occurs = (unsigned int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[0] = (unsigned char  *)&ssubcmd->stream_id;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[0] = (unsigned long )sizeof(long);
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[0] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[0] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[0] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[0] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[0] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[0] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[0] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[1] = (unsigned char  *)&ssubcmd->command_id;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[1] = (unsigned long )sizeof(long);
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[1] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[1] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[1] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[1] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[1] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[1] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[1] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[2] = (unsigned char  *)ssubcmd->cmdcode;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[2] = (unsigned long )4;
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[2] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[2] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[2] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[2] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[2] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[2] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[2] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[3] = (unsigned char  *)ssubcmd->order_code;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[3] = (unsigned long )5;
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[3] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[3] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[3] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[3] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[3] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[3] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[3] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[4] = (unsigned char  *)&ssubcmd->serial;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[4] = (unsigned long )1;
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[4] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[4] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[4] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[4] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[4] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[4] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[4] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[5] = (unsigned char  *)ssubcmd->msisdn;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[5] = (unsigned long )16;
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[5] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[5] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[5] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[5] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[5] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[5] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[5] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[6] = (unsigned char  *)ssubcmd->imsi_no;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[6] = (unsigned long )21;
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[6] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[6] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[6] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[6] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[6] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[6] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[6] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[7] = (unsigned char  *)ssubcmd->ss_info1;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[7] = (unsigned long )16;
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[7] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[7] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[7] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[7] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[7] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[7] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[7] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[8] = (unsigned char  *)ssubcmd->ss_info2;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[8] = (unsigned long )22;
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[8] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[8] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[8] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[8] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[8] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[8] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[8] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[9] = (unsigned char  *)ssubcmd->ss_info3;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[9] = (unsigned long )122;
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[9] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[9] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[9] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[9] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[9] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[9] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[9] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[10] = (unsigned char  *)ssubcmd->retn;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[10] = (unsigned long )5;
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[10] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[10] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[10] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[10] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[10] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[10] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[10] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstv[11] = (unsigned char  *)ssubcmd->parse_code;
#line 638 "offon_fordb.pc"
   sqlstm.sqhstl[11] = (unsigned long )5;
#line 638 "offon_fordb.pc"
   sqlstm.sqhsts[11] = (         int  )sizeof(struct tp_ssubcmd);
#line 638 "offon_fordb.pc"
   sqlstm.sqindv[11] = (         short *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqinds[11] = (         int  )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharm[11] = (unsigned long )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqharc[11] = (unsigned long  *)0;
#line 638 "offon_fordb.pc"
   sqlstm.sqadto[11] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqtdso[11] = (unsigned short )0;
#line 638 "offon_fordb.pc"
   sqlstm.sqphsv = sqlstm.sqhstv;
#line 638 "offon_fordb.pc"
   sqlstm.sqphsl = sqlstm.sqhstl;
#line 638 "offon_fordb.pc"
   sqlstm.sqphss = sqlstm.sqhsts;
#line 638 "offon_fordb.pc"
   sqlstm.sqpind = sqlstm.sqindv;
#line 638 "offon_fordb.pc"
   sqlstm.sqpins = sqlstm.sqinds;
#line 638 "offon_fordb.pc"
   sqlstm.sqparm = sqlstm.sqharm;
#line 638 "offon_fordb.pc"
   sqlstm.sqparc = sqlstm.sqharc;
#line 638 "offon_fordb.pc"
   sqlstm.sqpadto = sqlstm.sqadto;
#line 638 "offon_fordb.pc"
   sqlstm.sqptdso = sqlstm.sqtdso;
#line 638 "offon_fordb.pc"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 638 "offon_fordb.pc"
}

#line 638 "offon_fordb.pc"

			if(SQLCODE != SQL_OK){
				fprintf(logfp, "fail insert ssubcmd %d[%s]\n", SQLCODE, SQLERRMSG);
				fflush(logfp);
			}
			gettimeofday(&tv2, NULL);
			yy_time.tm4ssub += time_diff1(tv1, tv2);
		}
		if(databuf.ct_ssubcmd + databuf.ct_sopru + databuf.ct_sopr
				+ databuf.ct_sndcmd > 0){
			/*提交并保存断点, 提交失败则退出进程*/
			/* EXEC SQL commit; */ 
#line 649 "offon_fordb.pc"

{
#line 649 "offon_fordb.pc"
   struct sqlexd sqlstm;
#line 649 "offon_fordb.pc"
   sqlstm.sqlvsn = 12;
#line 649 "offon_fordb.pc"
   sqlstm.arrsiz = 16;
#line 649 "offon_fordb.pc"
   sqlstm.sqladtp = &sqladt;
#line 649 "offon_fordb.pc"
   sqlstm.sqltdsp = &sqltds;
#line 649 "offon_fordb.pc"
   sqlstm.iters = (unsigned int  )1;
#line 649 "offon_fordb.pc"
   sqlstm.offset = (unsigned int  )367;
#line 649 "offon_fordb.pc"
   sqlstm.cud = sqlcud0;
#line 649 "offon_fordb.pc"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 649 "offon_fordb.pc"
   sqlstm.sqlety = (unsigned short)4352;
#line 649 "offon_fordb.pc"
   sqlstm.occurs = (unsigned int  )0;
#line 649 "offon_fordb.pc"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 649 "offon_fordb.pc"
}

#line 649 "offon_fordb.pc"

			if(SQLCODE == SQL_OK){
				fprintf(logfp, "commit ok, opr:%d opru:%d,ssub:%d,sndcmd:%d\n",
						databuf.ct_sopr, databuf.ct_sopru, databuf.ct_ssubcmd, databuf.ct_sndcmd);
				fflush(logfp);
			}else{
				fprintf(logfp, "commit failed %d[%s]\n", SQLCODE, SQLERRMSG);
				fflush(logfp);
				exit(2);
			}
		}
		if(ret == 1)
			sav_file_pos(hlrcode, sdate, rpos);
	}

	return 0;
}
