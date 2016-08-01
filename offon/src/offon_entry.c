
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
    "offon_entry.pc"
};


static unsigned int sqlctx = 1241347;


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
   unsigned char  *sqhstv[13];
   unsigned long  sqhstl[13];
            int   sqhsts[13];
            short *sqindv[13];
            int   sqinds[13];
   unsigned long  sqharm[13];
   unsigned long  *sqharc[13];
   unsigned short  sqadto[13];
   unsigned short  sqtdso[13];
} sqlstm = {12,13};

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
5,0,0,1,0,0,17,426,0,0,1,1,0,1,0,1,97,0,0,
24,0,0,2,0,0,17,429,0,0,1,1,0,1,0,1,97,0,0,
43,0,0,3,0,0,17,430,0,0,1,1,0,1,0,1,97,0,0,
62,0,0,1,0,0,45,435,0,0,0,0,0,1,0,
77,0,0,1,0,0,13,449,0,0,13,0,0,1,0,2,3,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,
2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,
144,0,0,2,0,0,21,499,0,0,1,1,0,1,0,1,3,0,0,
163,0,0,4,0,0,32,503,0,0,0,0,0,1,0,
178,0,0,3,0,0,21,509,0,0,1,1,0,1,0,1,3,0,0,
197,0,0,5,0,0,32,515,0,0,0,0,0,1,0,
212,0,0,6,0,0,29,567,0,0,0,0,0,1,0,
227,0,0,1,0,0,15,592,0,0,0,0,0,1,0,
};


#line 1 "offon_entry.pc"
/*********************************************************
 *
 *  FileName:
 *      offon_entry.pc
 *
 *  Version:
 *      1.00 2004.11.11
 *
 *  tabstop = 4
 *
 ********************************************************/

#include "offon.h"
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
/* EXEC SQL INCLUDE SQLDA;
 */ 
#line 1 "/usr/lib/oracle/cli/rdbms/public/sqlda.h"
/*
 * $Header: sqlda.h 08-may-2007.05:58:33 ardesai Exp $ sqlda.h 
 */

/***************************************************************
*      The SQLDA descriptor definition                         *
*--------------------------------------------------------------*
*      VAX/3B Version                                          *
*                                                              *
* Copyright (c) 1987, 2007, Oracle. All rights reserved.  *
***************************************************************/


/* NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************
*/

/*  MODIFIED
    ardesai    05/08/07  - Bug[6037057] Undef Y
    apopat     05/08/02  - [2362423] MVS PE to make lines shorter than 79
    apopat     07/31/99 -  [707588] TAB to blanks for OCCS
    lvbcheng   10/27/98 -  change long to int for sqlda
    lvbcheng   08/15/97 -  Move sqlda protos to sqlcpr.h
    lvbcheng   06/25/97 -  Move sqlda protos to this file
    jbasu      01/29/95 -  correct typo
    jbasu      01/27/95 -  correct comment - ub2->sb2
    jbasu      12/12/94 - Bug 217878: note this is an SOSD file
    Morse      12/01/87 - undef L and S for v6 include files
    Richey     07/13/87 - change int defs to long 
    Clare      09/13/84 - Port: Ch types to match SQLLIB structs
    Clare      10/02/86 - Add ifndef SQLDA
*/

#ifndef SQLDA_
#define SQLDA_ 1
 
#ifdef T
# undef T
#endif
#ifdef F
# undef F
#endif

#ifdef S
# undef S
#endif
#ifdef L
# undef L
#endif

#ifdef Y
 # undef Y
#endif
 
struct SQLDA {
  /* ub4    */ int        N; /* Descriptor size in number of entries        */
  /* text** */ char     **V; /* Ptr to Arr of addresses of main variables   */
  /* ub4*   */ int       *L; /* Ptr to Arr of lengths of buffers            */
  /* sb2*   */ short     *T; /* Ptr to Arr of types of buffers              */
  /* sb2**  */ short    **I; /* Ptr to Arr of addresses of indicator vars   */
  /* sb4    */ int        F; /* Number of variables found by DESCRIBE       */
  /* text** */ char     **S; /* Ptr to Arr of variable name pointers        */
  /* ub2*   */ short     *M; /* Ptr to Arr of max lengths of var. names     */
  /* ub2*   */ short     *C; /* Ptr to Arr of current lengths of var. names */
  /* text** */ char     **X; /* Ptr to Arr of ind. var. name pointers       */
  /* ub2*   */ short     *Y; /* Ptr to Arr of max lengths of ind. var. names*/
  /* ub2*   */ short     *Z; /* Ptr to Arr of cur lengths of ind. var. names*/
  };
 
typedef struct SQLDA SQLDA;
 
#endif

/* ----------------- */
/* defines for sqlda */
/* ----------------- */

#define SQLSQLDAAlloc(arg1, arg2, arg3, arg4) sqlaldt(arg1, arg2, arg3, arg4) 

#define SQLSQLDAFree(arg1, arg2) sqlclut(arg1, arg2) 




#line 18 "offon_entry.pc"



#define MAXLINE	300
#define zero(a) memset(a, 0, sizeof(a))
#define init(a) memset(a, 0, sizeof(a))


char hlrcode[8];
char bakhlr[8];
FILE *logfp;

char *procname;

struct comm_data	tcpbuf;
struct op_data_req	*reqinfo;
struct op_data_ack	*ackinfo;

struct user_info	userhead,*userptr;
struct mcfg_info	mcfghead,*mcfgptr;

int					connid;
int					commport;
struct sockaddr_in	srvaddr;
struct
{
	char				hlr;
	int					flag;
	int					status;
	struct sockaddr_in	addr;
} hlraddr[256];


struct
{
	char rectype[20];
	char hlrcode[6];
	char db1[10];
	time_t rec_time;
	int rows;
	int tm4sel;
	int tm4fch;
	int tm4req;
	int tm4upd;
	int tm4ins;
	int tm4dul;
} yy_time;

#define		VERSION		"2.2.0"

extern int GenDaemon();
extern int testdir();
extern int db_login();
extern char *rlspace(char *);

void usage(char *s)
{
	printf("%s\n\nUsage:%s -Hhlrcode [-Dhlrcode] [-Udbuser -Pdbpswd -Sdbname] [-Llogdir] [-v]\n\n\n",s,procname);
	exit(1);
}

void LogOut(int sig)
{
	printf("LogOut signal==%d\n",sig);
	if(logfp)
	{
		fprintf(logfp,"LogOut signal==%d\n",sig);
		fclose(logfp);
	}

	exit(1);
}

int send_req(int id,int encrypt,int finished,int srvtype,int transid,int length)
{
	char chlen[16];

	length+=32;
	memcpy(tcpbuf.flag,MSGFLAG,4);
	sprintf(chlen,"%04d",length);
	memcpy(tcpbuf.msglen,chlen,4);
	sprintf(tcpbuf.srvtype,"%08d",srvtype);
	sprintf(tcpbuf.transid,"%08d",transid);
	tcpbuf.finished=finished;
	tcpbuf.msgtype=REQ_TYPE;
	tcpbuf.encrypt=encrypt;
	/*memcpy(tcpbuf.reserve,"     ",5); */

printf("SND:%s[%d]\n",(char *)&tcpbuf,length);
	return writenet(id,(char *)&tcpbuf,length);
}

struct cmd11
{
    long	command_id;
    char	hlr_code[8];
    char	phone_no[20];
    char	command_code[8];
    char	op_code[8];
    char	imsi_no[32];
    char	new_phone[20];
    char	new_imsi[32];
    char	otherinfo[160];
    char	business_status[8];
    char	request_time[15];
    char    login_no[7];
    char    group_id[11];
};

void yy_time_log(time_t t)
{
	if(t-yy_time.rec_time >= 300){
		file_log("perf",
				"rectime=%ld,rectype=etr,hlrcode=%s,db1=%s,rows=%d,tm4sel=%d,"
				"tm4fch=%d,tm4req=%d,tm4upd=%d,tm4ins=%d,tm4dul=%d",
				t, yy_time.hlrcode, yy_time.db1, yy_time.rows, yy_time.tm4sel/1000,
				yy_time.tm4fch/1000, yy_time.tm4req/1000, yy_time.tm4upd/1000, yy_time.tm4ins/1000, yy_time.tm4dul/1000);
		yy_time.rows = 0;
		yy_time.tm4sel = 0;
		yy_time.tm4fch = 0;
		yy_time.tm4req = 0;
		yy_time.tm4upd = 0;
		yy_time.tm4ins = 0;
		yy_time.tm4dul = 0;
		yy_time.rec_time = t;
	}
}

FILE * open_logfp(time_t t, FILE *fp1, char *hlrcode)
{
	struct tm *tv;
	char fname[128];

	static int curt = 0;

	tv=localtime(&t);
	if(tv->tm_mday != curt)
	{
		sprintf(fname,"%s/etr%04d%02d%02d.%s",
				getenv("LOGDIR"), tv->tm_year+1900, tv->tm_mon+1,
				tv->tm_mday, hlrcode);
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
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 174 "offon_entry.pc"

		char	selsql[1024];
		char	updsql[1024];
		char	inssql[1024];

		char	retn[8];
		int		rec_count;
		int		total_count;
	/* EXEC SQL END DECLARE SECTION; */ 
#line 182 "offon_entry.pc"


	int ret,count,endflag;
	char inputstr[2048];
	int num,i,status,today;
	char srvip[32];
	int srvport;
	FILE *cfgfp;
	char cfgstr[256];
	char dbuser[32],dbpswd[32],dbname[32],bossdb[32],tname[64];
	char logfilename[256],logdir[256];
	time_t t;
	struct tm *tv;
    int x, y;

	char chhlr;
	int msglen,readlen,headlen,tcplen,bodylen;
	int transid;
	struct timeval t1, t2;
	int SNDLINE = 40;
	long cmdids[40];

	procname=argv[0];
	logfp=NULL;

	memset(inputstr,0x0,sizeof(inputstr));
	for(i=0;i<argc;i++)
	{
		strcat(inputstr,argv[i]);
		strcat(inputstr," ");
	}

	if(argc<2)
		usage("Incorrect Parameter!");

	if(GenDaemon()<0)
	{
		printf("GenDaemon() failed!\n");
		exit(1);
	}

	signal(SIGTERM,LogOut);
	signal(SIGPIPE,SIG_IGN);
	signal(SIGCLD,SIG_IGN);
	signal(SIGINT,SIG_IGN);

	memset(hlrcode,0x0,sizeof(hlrcode));
	memset(bakhlr,0x0,sizeof(bakhlr));
	memset(dbuser,0x0,sizeof(dbuser));
	memset(dbpswd,0x0,sizeof(dbpswd));
	memset(dbname,0x0,sizeof(dbname));
	memset(bossdb,0x0,sizeof(bossdb));
	memset(logdir,0x0,sizeof(logdir));

	while(--argc>0)
	{
		if(argv[argc][0]!='-')
			usage("Incorrect Parameter!");

		switch(argv[argc][1])
		{
		case 'H':
			strncpy(hlrcode,argv[argc]+2,4);
			break;
		case 'U':
			strncpy(dbuser,argv[argc]+2,30);
			break;
		case 'P':
			strncpy(dbpswd,argv[argc]+2,30);
			break;
		case 'S':
			strncpy(dbname,argv[argc]+2,30);
			break;
		case 'L':
			strncpy(logdir,argv[argc]+2,240);
			break;
		case 'D':
			strncpy(bakhlr, argv[argc]+2,4);
			break;
		case 'N':
			SNDLINE = atoi(argv[argc]+2);
			if(SNDLINE > 40 || SNDLINE < 1)
				SNDLINE = 40;
			break;
		case 'V':
		case 'v':
			printf("\n\nversion : %s\n\n",VERSION);
			exit(0);
		default:
			usage("Unknow Parameter!");
			break;
		}
	}

printf("hlrcode===%s~\n",hlrcode);
	if(strlen(hlrcode)!=3)
		usage("Incorrect parameter [hlrcode]!\n");

	if(strlen(logdir)<1)
		strcpy(logdir,getenv("LOGDIR"));

	if(testdir(logdir))
		usage("Incorrect parameter [logdir]!");

	time(&t);
	tv=localtime(&t);
	memset(logfilename,0x0,sizeof(logfilename));
	sprintf(logfilename,"%s/etr%04d%02d%02d.%s",logdir,
		tv->tm_year+1900,tv->tm_mon+1,tv->tm_mday,hlrcode);
	logfp = open_logfp(t, NULL, hlrcode);

	memset(&yy_time, 0, sizeof(yy_time));
	strcpy(yy_time.hlrcode, hlrcode);
	strcpy(yy_time.rectype, "etr");
	strcpy(yy_time.db1, "offdb");
	yy_time.rec_time = time(NULL);

	fprintf(logfp,"INIT %04d/%02d/%02d %02d:%02d:%02d\n",
		tv->tm_year+1900,tv->tm_mon+1,tv->tm_mday,
		tv->tm_hour,tv->tm_min,tv->tm_sec);
	fprintf(logfp,"%s[%d]\n",inputstr,argc);
	fflush(logfp);
    today=tv->tm_mday;

	if(strlen(dbuser)<1 || strlen(dbpswd)<1 || strlen(dbname)<1)
	{
		memset(cfgstr,0x0,sizeof(cfgstr));
		strcpy(cfgstr,getenv("ETCDIR"));
		strcat(cfgstr,DBLOGIN);

		cfgfp=fopen(cfgstr,"r");
		if(cfgfp==NULL)
		{
			fprintf(logfp,"can't open %s for read[%d]\n",cfgstr,errno);
			fclose(logfp);
			exit(1);
		}

		memset(dbuser,0x0,sizeof(dbuser));
		memset(dbpswd,0x0,sizeof(dbpswd));
		memset(dbname,0x0,sizeof(dbname));
		fscanf(cfgfp,"%s%s%s",dbuser,dbpswd,dbname);
		fclose(cfgfp);
	}

	if(db_login(dbuser,dbpswd,dbname))
	{
		fprintf(logfp,"db_login(%s,%s,%s) failed!\n",dbuser,dbpswd,dbname);
		fclose(logfp);
		exit(1);
	}
	fprintf(logfp,"db_login(%s) success!\n",dbname);
	fflush(logfp);

	commport=get_commport(hlrcode,srvip, NULL);
	if(commport<1024 || commport>65535)
	{
		fprintf(logfp,"get_commport(%s) == %d failed[%d]\n",commport,errno);
		fclose(logfp);
		exit(1);
	}

	memset(&srvaddr,0x0,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_addr.s_addr=inet_addr(srvip);
	srvaddr.sin_port=htons(commport);

	if(srvaddr.sin_addr.s_addr==-1)
	{
		fprintf(logfp,"Incorrect IPADDR in mcfghead[%s]\n",srvip);
		fclose(logfp);
		exit(1);
	}

	memset(dbuser,0x0,sizeof(dbuser));
	memset(dbpswd,0x0,sizeof(dbpswd));
	memset(tname,0x0,sizeof(tname));

	if(get_bossinfo(hlrcode,bossdb,dbuser,dbpswd,tname))
	{
		fprintf(logfp,"get_boss_db(%s) failed!\n",hlrcode);
		fclose(logfp);
		exit(1);
	}
	else
	{
		fprintf(logfp,"BOSSDB:%s[%d]	%d		%s		%s[%d]\n",
			dbuser,strlen(dbuser), strlen(dbpswd),bossdb, tname,strlen(tname));
		fflush(logfp);
	}
printf("tname====%s~\n",tname);

	db_logout();

	if(db_login(dbuser,dbpswd,bossdb))
	{
		fprintf(logfp,"db_login(%s) failed!\n",bossdb);
		fclose(logfp);
		exit(1);
	}
	fprintf(logfp,"db_login(%s) success!\n",bossdb);
	fflush(logfp);   

	connid=socket(AF_INET,SOCK_STREAM,0);
	if(connid<0)
	{
		fprintf(logfp,"socket() failed[%d]\n",errno);
		fclose(logfp);
		exit(1);
	}

	if(connect(connid,(struct sockaddr *)&srvaddr,sizeof(srvaddr))<0)
	{
		fprintf(logfp,"connect(%d) failed[%d]!\n",connid,errno);
		fclose(logfp);
		exit(1);
	}

	memset(selsql,0x0,sizeof(selsql));
	memset(updsql,0x0,sizeof(updsql));
	memset(inssql, 0, sizeof(inssql));

	/*** on 20050411 排序中增加op_time选项,以避免流水不规则问题 */
	sprintf(selsql,"SELECT command_id,hlr_code,phone_no,command_code,op_code,imsi_no,new_phone,new_imsi,"
        "other_char,business_status,to_char(request_time, 'yyyymmddhh24miss'), login_no,nvl(group_id,'1') "
        "FROM %s WHERE send_status='0' ORDER BY request_time,command_id",tname);
	sprintf(updsql,"UPDATE %s SET send_status='1',send_time=sysdate WHERE command_id=:v1",tname);
	if(bakhlr[0] != '\0'){
		sprintf(inssql, "insert into sndcmdday%s select COMMAND_ID,HLR_CODE,COMMAND_CODE,COMMAND_ORDER,"
            "ID_NO, BELONG_CODE, SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR, OP_CODE,"
            "TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE, LOGIN_ACCEPT, REQUEST_TIME, BUSINESS_STATUS, "
            "SEND_STATUS, SEND_TIME, 'kkkkkkkkkk', ORG_ID from sndcmdday%s "
            "where command_id=:cmdid and (group_id<>'kkkkkkkkkk' or group_id is null)",
                bakhlr, hlrcode);
	}

	SQLCODE=SQL_OK;
	for(;;)
	{
        struct cmd11 c1[MAXLINE], *c;
        int rec_idx;

        gettimeofday(&t1, NULL);

		/* EXEC SQL PREPARE sel_sql FROM :selsql; */ 
#line 426 "offon_entry.pc"

{
#line 426 "offon_entry.pc"
  struct sqlexd sqlstm;
#line 426 "offon_entry.pc"
  sqlstm.sqlvsn = 12;
#line 426 "offon_entry.pc"
  sqlstm.arrsiz = 1;
#line 426 "offon_entry.pc"
  sqlstm.sqladtp = &sqladt;
#line 426 "offon_entry.pc"
  sqlstm.sqltdsp = &sqltds;
#line 426 "offon_entry.pc"
  sqlstm.stmt = "";
#line 426 "offon_entry.pc"
  sqlstm.iters = (unsigned int  )1;
#line 426 "offon_entry.pc"
  sqlstm.offset = (unsigned int  )5;
#line 426 "offon_entry.pc"
  sqlstm.cud = sqlcud0;
#line 426 "offon_entry.pc"
  sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 426 "offon_entry.pc"
  sqlstm.sqlety = (unsigned short)4352;
#line 426 "offon_entry.pc"
  sqlstm.occurs = (unsigned int  )0;
#line 426 "offon_entry.pc"
  sqlstm.sqhstv[0] = (unsigned char  *)selsql;
#line 426 "offon_entry.pc"
  sqlstm.sqhstl[0] = (unsigned long )1024;
#line 426 "offon_entry.pc"
  sqlstm.sqhsts[0] = (         int  )0;
#line 426 "offon_entry.pc"
  sqlstm.sqindv[0] = (         short *)0;
#line 426 "offon_entry.pc"
  sqlstm.sqinds[0] = (         int  )0;
#line 426 "offon_entry.pc"
  sqlstm.sqharm[0] = (unsigned long )0;
#line 426 "offon_entry.pc"
  sqlstm.sqadto[0] = (unsigned short )0;
#line 426 "offon_entry.pc"
  sqlstm.sqtdso[0] = (unsigned short )0;
#line 426 "offon_entry.pc"
  sqlstm.sqphsv = sqlstm.sqhstv;
#line 426 "offon_entry.pc"
  sqlstm.sqphsl = sqlstm.sqhstl;
#line 426 "offon_entry.pc"
  sqlstm.sqphss = sqlstm.sqhsts;
#line 426 "offon_entry.pc"
  sqlstm.sqpind = sqlstm.sqindv;
#line 426 "offon_entry.pc"
  sqlstm.sqpins = sqlstm.sqinds;
#line 426 "offon_entry.pc"
  sqlstm.sqparm = sqlstm.sqharm;
#line 426 "offon_entry.pc"
  sqlstm.sqparc = sqlstm.sqharc;
#line 426 "offon_entry.pc"
  sqlstm.sqpadto = sqlstm.sqadto;
#line 426 "offon_entry.pc"
  sqlstm.sqptdso = sqlstm.sqtdso;
#line 426 "offon_entry.pc"
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 426 "offon_entry.pc"
}

#line 426 "offon_entry.pc"

		/* EXEC SQL DECLARE cur_sel CURSOR FOR sel_sql; */ 
#line 427 "offon_entry.pc"


        /* EXEC SQL PREPARE ins_sql from :inssql; */ 
#line 429 "offon_entry.pc"

{
#line 429 "offon_entry.pc"
        struct sqlexd sqlstm;
#line 429 "offon_entry.pc"
        sqlstm.sqlvsn = 12;
#line 429 "offon_entry.pc"
        sqlstm.arrsiz = 1;
#line 429 "offon_entry.pc"
        sqlstm.sqladtp = &sqladt;
#line 429 "offon_entry.pc"
        sqlstm.sqltdsp = &sqltds;
#line 429 "offon_entry.pc"
        sqlstm.stmt = "";
#line 429 "offon_entry.pc"
        sqlstm.iters = (unsigned int  )1;
#line 429 "offon_entry.pc"
        sqlstm.offset = (unsigned int  )24;
#line 429 "offon_entry.pc"
        sqlstm.cud = sqlcud0;
#line 429 "offon_entry.pc"
        sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 429 "offon_entry.pc"
        sqlstm.sqlety = (unsigned short)4352;
#line 429 "offon_entry.pc"
        sqlstm.occurs = (unsigned int  )0;
#line 429 "offon_entry.pc"
        sqlstm.sqhstv[0] = (unsigned char  *)inssql;
#line 429 "offon_entry.pc"
        sqlstm.sqhstl[0] = (unsigned long )1024;
#line 429 "offon_entry.pc"
        sqlstm.sqhsts[0] = (         int  )0;
#line 429 "offon_entry.pc"
        sqlstm.sqindv[0] = (         short *)0;
#line 429 "offon_entry.pc"
        sqlstm.sqinds[0] = (         int  )0;
#line 429 "offon_entry.pc"
        sqlstm.sqharm[0] = (unsigned long )0;
#line 429 "offon_entry.pc"
        sqlstm.sqadto[0] = (unsigned short )0;
#line 429 "offon_entry.pc"
        sqlstm.sqtdso[0] = (unsigned short )0;
#line 429 "offon_entry.pc"
        sqlstm.sqphsv = sqlstm.sqhstv;
#line 429 "offon_entry.pc"
        sqlstm.sqphsl = sqlstm.sqhstl;
#line 429 "offon_entry.pc"
        sqlstm.sqphss = sqlstm.sqhsts;
#line 429 "offon_entry.pc"
        sqlstm.sqpind = sqlstm.sqindv;
#line 429 "offon_entry.pc"
        sqlstm.sqpins = sqlstm.sqinds;
#line 429 "offon_entry.pc"
        sqlstm.sqparm = sqlstm.sqharm;
#line 429 "offon_entry.pc"
        sqlstm.sqparc = sqlstm.sqharc;
#line 429 "offon_entry.pc"
        sqlstm.sqpadto = sqlstm.sqadto;
#line 429 "offon_entry.pc"
        sqlstm.sqptdso = sqlstm.sqtdso;
#line 429 "offon_entry.pc"
        sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 429 "offon_entry.pc"
}

#line 429 "offon_entry.pc"

        /* EXEC SQL PREPARE upd_sql FROM :updsql; */ 
#line 430 "offon_entry.pc"

{
#line 430 "offon_entry.pc"
        struct sqlexd sqlstm;
#line 430 "offon_entry.pc"
        sqlstm.sqlvsn = 12;
#line 430 "offon_entry.pc"
        sqlstm.arrsiz = 1;
#line 430 "offon_entry.pc"
        sqlstm.sqladtp = &sqladt;
#line 430 "offon_entry.pc"
        sqlstm.sqltdsp = &sqltds;
#line 430 "offon_entry.pc"
        sqlstm.stmt = "";
#line 430 "offon_entry.pc"
        sqlstm.iters = (unsigned int  )1;
#line 430 "offon_entry.pc"
        sqlstm.offset = (unsigned int  )43;
#line 430 "offon_entry.pc"
        sqlstm.cud = sqlcud0;
#line 430 "offon_entry.pc"
        sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 430 "offon_entry.pc"
        sqlstm.sqlety = (unsigned short)4352;
#line 430 "offon_entry.pc"
        sqlstm.occurs = (unsigned int  )0;
#line 430 "offon_entry.pc"
        sqlstm.sqhstv[0] = (unsigned char  *)updsql;
#line 430 "offon_entry.pc"
        sqlstm.sqhstl[0] = (unsigned long )1024;
#line 430 "offon_entry.pc"
        sqlstm.sqhsts[0] = (         int  )0;
#line 430 "offon_entry.pc"
        sqlstm.sqindv[0] = (         short *)0;
#line 430 "offon_entry.pc"
        sqlstm.sqinds[0] = (         int  )0;
#line 430 "offon_entry.pc"
        sqlstm.sqharm[0] = (unsigned long )0;
#line 430 "offon_entry.pc"
        sqlstm.sqadto[0] = (unsigned short )0;
#line 430 "offon_entry.pc"
        sqlstm.sqtdso[0] = (unsigned short )0;
#line 430 "offon_entry.pc"
        sqlstm.sqphsv = sqlstm.sqhstv;
#line 430 "offon_entry.pc"
        sqlstm.sqphsl = sqlstm.sqhstl;
#line 430 "offon_entry.pc"
        sqlstm.sqphss = sqlstm.sqhsts;
#line 430 "offon_entry.pc"
        sqlstm.sqpind = sqlstm.sqindv;
#line 430 "offon_entry.pc"
        sqlstm.sqpins = sqlstm.sqinds;
#line 430 "offon_entry.pc"
        sqlstm.sqparm = sqlstm.sqharm;
#line 430 "offon_entry.pc"
        sqlstm.sqparc = sqlstm.sqharc;
#line 430 "offon_entry.pc"
        sqlstm.sqpadto = sqlstm.sqadto;
#line 430 "offon_entry.pc"
        sqlstm.sqptdso = sqlstm.sqtdso;
#line 430 "offon_entry.pc"
        sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 430 "offon_entry.pc"
}

#line 430 "offon_entry.pc"


		rec_count=0;
        endflag = 0;
        count = 0;
		/* EXEC SQL OPEN cur_sel; */ 
#line 435 "offon_entry.pc"

{
#line 435 "offon_entry.pc"
  struct sqlexd sqlstm;
#line 435 "offon_entry.pc"
  sqlstm.sqlvsn = 12;
#line 435 "offon_entry.pc"
  sqlstm.arrsiz = 1;
#line 435 "offon_entry.pc"
  sqlstm.sqladtp = &sqladt;
#line 435 "offon_entry.pc"
  sqlstm.sqltdsp = &sqltds;
#line 435 "offon_entry.pc"
  sqlstm.stmt = "";
#line 435 "offon_entry.pc"
  sqlstm.iters = (unsigned int  )1;
#line 435 "offon_entry.pc"
  sqlstm.offset = (unsigned int  )62;
#line 435 "offon_entry.pc"
  sqlstm.selerr = (unsigned short)1;
#line 435 "offon_entry.pc"
  sqlstm.cud = sqlcud0;
#line 435 "offon_entry.pc"
  sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 435 "offon_entry.pc"
  sqlstm.sqlety = (unsigned short)4352;
#line 435 "offon_entry.pc"
  sqlstm.occurs = (unsigned int  )0;
#line 435 "offon_entry.pc"
  sqlstm.sqcmod = (unsigned int )0;
#line 435 "offon_entry.pc"
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 435 "offon_entry.pc"
}

#line 435 "offon_entry.pc"

        if(SQLCODE != SQL_OK){
            fprintf(logfp, "select failed %d, %s\n", SQLCODE, SQLERRMSG);
            fclose(logfp);
            exit(1);
        }
		gettimeofday(&t2, NULL);
		yy_time.tm4sel += time_diff1(t1, t2);
		fprintf(logfp, "tm4sel:%d SQLCODE:%d\n", time_diff1(t1, t2), SQLCODE);
		fflush(logfp);
		while(SQLCODE == SQL_OK)
		{
			gettimeofday(&t1, NULL);
            memset(&c1, 0, sizeof(c1));
            /* EXEC SQL FETCH cur_sel INTO :c1; */ 
#line 449 "offon_entry.pc"

{
#line 449 "offon_entry.pc"
            struct sqlexd sqlstm;
#line 449 "offon_entry.pc"
            sqlstm.sqlvsn = 12;
#line 449 "offon_entry.pc"
            sqlstm.arrsiz = 13;
#line 449 "offon_entry.pc"
            sqlstm.sqladtp = &sqladt;
#line 449 "offon_entry.pc"
            sqlstm.sqltdsp = &sqltds;
#line 449 "offon_entry.pc"
            sqlstm.iters = (unsigned int  )300;
#line 449 "offon_entry.pc"
            sqlstm.offset = (unsigned int  )77;
#line 449 "offon_entry.pc"
            sqlstm.selerr = (unsigned short)1;
#line 449 "offon_entry.pc"
            sqlstm.cud = sqlcud0;
#line 449 "offon_entry.pc"
            sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 449 "offon_entry.pc"
            sqlstm.sqlety = (unsigned short)4352;
#line 449 "offon_entry.pc"
            sqlstm.occurs = (unsigned int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqfoff = (         int )0;
#line 449 "offon_entry.pc"
            sqlstm.sqfmod = (unsigned int )2;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[0] = (unsigned char  *)&c1->command_id;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[0] = (unsigned long )sizeof(long);
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[0] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[0] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[0] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[0] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[0] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[0] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[0] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[1] = (unsigned char  *)c1->hlr_code;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[1] = (unsigned long )8;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[1] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[1] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[1] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[1] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[1] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[1] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[1] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[2] = (unsigned char  *)c1->phone_no;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[2] = (unsigned long )20;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[2] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[2] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[2] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[2] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[2] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[2] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[2] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[3] = (unsigned char  *)c1->command_code;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[3] = (unsigned long )8;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[3] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[3] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[3] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[3] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[3] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[3] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[3] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[4] = (unsigned char  *)c1->op_code;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[4] = (unsigned long )8;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[4] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[4] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[4] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[4] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[4] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[4] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[4] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[5] = (unsigned char  *)c1->imsi_no;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[5] = (unsigned long )32;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[5] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[5] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[5] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[5] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[5] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[5] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[5] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[6] = (unsigned char  *)c1->new_phone;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[6] = (unsigned long )20;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[6] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[6] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[6] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[6] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[6] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[6] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[6] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[7] = (unsigned char  *)c1->new_imsi;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[7] = (unsigned long )32;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[7] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[7] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[7] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[7] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[7] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[7] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[7] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[8] = (unsigned char  *)c1->otherinfo;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[8] = (unsigned long )160;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[8] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[8] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[8] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[8] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[8] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[8] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[8] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[9] = (unsigned char  *)c1->business_status;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[9] = (unsigned long )8;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[9] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[9] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[9] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[9] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[9] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[9] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[9] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[10] = (unsigned char  *)c1->request_time;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[10] = (unsigned long )15;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[10] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[10] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[10] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[10] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[10] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[10] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[10] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[11] = (unsigned char  *)c1->login_no;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[11] = (unsigned long )7;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[11] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[11] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[11] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[11] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[11] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[11] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[11] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqhstv[12] = (unsigned char  *)c1->group_id;
#line 449 "offon_entry.pc"
            sqlstm.sqhstl[12] = (unsigned long )11;
#line 449 "offon_entry.pc"
            sqlstm.sqhsts[12] = (         int  )sizeof(struct cmd11);
#line 449 "offon_entry.pc"
            sqlstm.sqindv[12] = (         short *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqinds[12] = (         int  )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharm[12] = (unsigned long )0;
#line 449 "offon_entry.pc"
            sqlstm.sqharc[12] = (unsigned long  *)0;
#line 449 "offon_entry.pc"
            sqlstm.sqadto[12] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqtdso[12] = (unsigned short )0;
#line 449 "offon_entry.pc"
            sqlstm.sqphsv = sqlstm.sqhstv;
#line 449 "offon_entry.pc"
            sqlstm.sqphsl = sqlstm.sqhstl;
#line 449 "offon_entry.pc"
            sqlstm.sqphss = sqlstm.sqhsts;
#line 449 "offon_entry.pc"
            sqlstm.sqpind = sqlstm.sqindv;
#line 449 "offon_entry.pc"
            sqlstm.sqpins = sqlstm.sqinds;
#line 449 "offon_entry.pc"
            sqlstm.sqparm = sqlstm.sqharm;
#line 449 "offon_entry.pc"
            sqlstm.sqparc = sqlstm.sqharc;
#line 449 "offon_entry.pc"
            sqlstm.sqpadto = sqlstm.sqadto;
#line 449 "offon_entry.pc"
            sqlstm.sqptdso = sqlstm.sqtdso;
#line 449 "offon_entry.pc"
            sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 449 "offon_entry.pc"
}

#line 449 "offon_entry.pc"

            if(SQLCODE == 1403){
                endflag = 1;
            }else if(SQLCODE != 0){
                fprintf(logfp, "fetch failed %d %s", SQLCODE, SQLERRMSG);
                fflush(logfp);
                break;
            }
            rec_count = SQLROWS - count;
            fprintf(logfp, "rec_count:%d\n", rec_count); fflush(logfp);
            count = SQLROWS;
            gettimeofday(&t2, NULL);
            yy_time.tm4fch += time_diff1(t1, t2);
            yy_time.rows += rec_count;
            for(rec_idx=0; rec_idx<rec_count; rec_idx++)
            {
                /*批量向manager送指令, 一次最多 SNDLINE行*/
                y = SNDLINE;
                if(rec_count - rec_idx < SNDLINE)
                	y = rec_count - rec_idx;

                if(y > 99) y = 99;
				sprintf(tcpbuf.reserve, "VV%02d", y); tcpbuf.reserve[4] = ' ';
                for(x=0; x<y; x++)
                {
					c = &c1[rec_idx+x];
					cmdids[x] = c->command_id;
					rlspace(c->group_id);
					reqinfo=(struct op_data_req *)tcpbuf.data;
					reqinfo += x;
					sprintf(reqinfo->command_id,"%ld",c->command_id);
					strcpy(reqinfo->phone_no,rlspace(c->phone_no));
					strcpy(reqinfo->imsi_no,rlspace(c->imsi_no));
					strcpy(reqinfo->op_code,rlspace(c->op_code));
					strcpy(reqinfo->command_code,rlspace(c->command_code));
					if(atoi(c->command_code)>=60)
						reqinfo->cmd_status='0';
					else
						reqinfo->cmd_status=c->business_status[0];
					reqinfo->phone_priority='1';
					strcpy(reqinfo->ss_info1,rlspace(c->new_phone));
					strcpy(reqinfo->ss_info2,rlspace(c->new_imsi));
					strcpy(reqinfo->ss_info3,rlspace(c->otherinfo));
					strcpy(reqinfo->req_time,rlspace(c->request_time));
					strcpy(reqinfo->login_no,rlspace(c->login_no));

                }
                gettimeofday(&t1, NULL);
                if(bakhlr[0] != 0)
                {
					/* EXEC SQL for :y EXECUTE ins_sql USING :cmdids; */ 
#line 499 "offon_entry.pc"

{
#line 499 "offon_entry.pc"
     struct sqlexd sqlstm;
#line 499 "offon_entry.pc"
     sqlstm.sqlvsn = 12;
#line 499 "offon_entry.pc"
     sqlstm.arrsiz = 13;
#line 499 "offon_entry.pc"
     sqlstm.sqladtp = &sqladt;
#line 499 "offon_entry.pc"
     sqlstm.sqltdsp = &sqltds;
#line 499 "offon_entry.pc"
     sqlstm.stmt = "";
#line 499 "offon_entry.pc"
     sqlstm.iters = (unsigned int  )y;
#line 499 "offon_entry.pc"
     sqlstm.offset = (unsigned int  )144;
#line 499 "offon_entry.pc"
     sqlstm.cud = sqlcud0;
#line 499 "offon_entry.pc"
     sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 499 "offon_entry.pc"
     sqlstm.sqlety = (unsigned short)4352;
#line 499 "offon_entry.pc"
     sqlstm.occurs = (unsigned int  )0;
#line 499 "offon_entry.pc"
     sqlstm.sqhstv[0] = (unsigned char  *)cmdids;
#line 499 "offon_entry.pc"
     sqlstm.sqhstl[0] = (unsigned long )sizeof(long);
#line 499 "offon_entry.pc"
     sqlstm.sqhsts[0] = (         int  )sizeof(long);
#line 499 "offon_entry.pc"
     sqlstm.sqindv[0] = (         short *)0;
#line 499 "offon_entry.pc"
     sqlstm.sqinds[0] = (         int  )0;
#line 499 "offon_entry.pc"
     sqlstm.sqharm[0] = (unsigned long )0;
#line 499 "offon_entry.pc"
     sqlstm.sqharc[0] = (unsigned long  *)0;
#line 499 "offon_entry.pc"
     sqlstm.sqadto[0] = (unsigned short )0;
#line 499 "offon_entry.pc"
     sqlstm.sqtdso[0] = (unsigned short )0;
#line 499 "offon_entry.pc"
     sqlstm.sqphsv = sqlstm.sqhstv;
#line 499 "offon_entry.pc"
     sqlstm.sqphsl = sqlstm.sqhstl;
#line 499 "offon_entry.pc"
     sqlstm.sqphss = sqlstm.sqhsts;
#line 499 "offon_entry.pc"
     sqlstm.sqpind = sqlstm.sqindv;
#line 499 "offon_entry.pc"
     sqlstm.sqpins = sqlstm.sqinds;
#line 499 "offon_entry.pc"
     sqlstm.sqparm = sqlstm.sqharm;
#line 499 "offon_entry.pc"
     sqlstm.sqparc = sqlstm.sqharc;
#line 499 "offon_entry.pc"
     sqlstm.sqpadto = sqlstm.sqadto;
#line 499 "offon_entry.pc"
     sqlstm.sqptdso = sqlstm.sqtdso;
#line 499 "offon_entry.pc"
     sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 499 "offon_entry.pc"
}

#line 499 "offon_entry.pc"

					if(SQLCODE != SQL_OK){
						fprintf(logfp, "insert fail %d [%s]\n", SQLCODE, SQLERRMSG);
						fclose(logfp);
						/* EXEC SQL ROLLBACK WORK RELEASE; */ 
#line 503 "offon_entry.pc"

{
#line 503 "offon_entry.pc"
      struct sqlexd sqlstm;
#line 503 "offon_entry.pc"
      sqlstm.sqlvsn = 12;
#line 503 "offon_entry.pc"
      sqlstm.arrsiz = 13;
#line 503 "offon_entry.pc"
      sqlstm.sqladtp = &sqladt;
#line 503 "offon_entry.pc"
      sqlstm.sqltdsp = &sqltds;
#line 503 "offon_entry.pc"
      sqlstm.iters = (unsigned int  )1;
#line 503 "offon_entry.pc"
      sqlstm.offset = (unsigned int  )163;
#line 503 "offon_entry.pc"
      sqlstm.cud = sqlcud0;
#line 503 "offon_entry.pc"
      sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 503 "offon_entry.pc"
      sqlstm.sqlety = (unsigned short)4352;
#line 503 "offon_entry.pc"
      sqlstm.occurs = (unsigned int  )0;
#line 503 "offon_entry.pc"
      sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 503 "offon_entry.pc"
}

#line 503 "offon_entry.pc"

						exit(1);
					}
                }
				gettimeofday(&t2, NULL);
				yy_time.tm4ins += time_diff1(t1, t2);
				/* EXEC SQL for :y EXECUTE upd_sql USING :cmdids; */ 
#line 509 "offon_entry.pc"

{
#line 509 "offon_entry.pc"
    struct sqlexd sqlstm;
#line 509 "offon_entry.pc"
    sqlstm.sqlvsn = 12;
#line 509 "offon_entry.pc"
    sqlstm.arrsiz = 13;
#line 509 "offon_entry.pc"
    sqlstm.sqladtp = &sqladt;
#line 509 "offon_entry.pc"
    sqlstm.sqltdsp = &sqltds;
#line 509 "offon_entry.pc"
    sqlstm.stmt = "";
#line 509 "offon_entry.pc"
    sqlstm.iters = (unsigned int  )y;
#line 509 "offon_entry.pc"
    sqlstm.offset = (unsigned int  )178;
#line 509 "offon_entry.pc"
    sqlstm.cud = sqlcud0;
#line 509 "offon_entry.pc"
    sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 509 "offon_entry.pc"
    sqlstm.sqlety = (unsigned short)4352;
#line 509 "offon_entry.pc"
    sqlstm.occurs = (unsigned int  )0;
#line 509 "offon_entry.pc"
    sqlstm.sqhstv[0] = (unsigned char  *)cmdids;
#line 509 "offon_entry.pc"
    sqlstm.sqhstl[0] = (unsigned long )sizeof(long);
#line 509 "offon_entry.pc"
    sqlstm.sqhsts[0] = (         int  )sizeof(long);
#line 509 "offon_entry.pc"
    sqlstm.sqindv[0] = (         short *)0;
#line 509 "offon_entry.pc"
    sqlstm.sqinds[0] = (         int  )0;
#line 509 "offon_entry.pc"
    sqlstm.sqharm[0] = (unsigned long )0;
#line 509 "offon_entry.pc"
    sqlstm.sqharc[0] = (unsigned long  *)0;
#line 509 "offon_entry.pc"
    sqlstm.sqadto[0] = (unsigned short )0;
#line 509 "offon_entry.pc"
    sqlstm.sqtdso[0] = (unsigned short )0;
#line 509 "offon_entry.pc"
    sqlstm.sqphsv = sqlstm.sqhstv;
#line 509 "offon_entry.pc"
    sqlstm.sqphsl = sqlstm.sqhstl;
#line 509 "offon_entry.pc"
    sqlstm.sqphss = sqlstm.sqhsts;
#line 509 "offon_entry.pc"
    sqlstm.sqpind = sqlstm.sqindv;
#line 509 "offon_entry.pc"
    sqlstm.sqpins = sqlstm.sqinds;
#line 509 "offon_entry.pc"
    sqlstm.sqparm = sqlstm.sqharm;
#line 509 "offon_entry.pc"
    sqlstm.sqparc = sqlstm.sqharc;
#line 509 "offon_entry.pc"
    sqlstm.sqpadto = sqlstm.sqadto;
#line 509 "offon_entry.pc"
    sqlstm.sqptdso = sqlstm.sqtdso;
#line 509 "offon_entry.pc"
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 509 "offon_entry.pc"
}

#line 509 "offon_entry.pc"

				if(SQLCODE != SQL_OK)
				{
					fprintf(logfp,"update failed[%s][%ld][%d:%s][%ld]\n",
						updsql, cmdids[0], SQLCODE, SQLERRMSG, time(NULL));
					fclose(logfp);
					/* EXEC SQL ROLLBACK WORK RELEASE; */ 
#line 515 "offon_entry.pc"

{
#line 515 "offon_entry.pc"
     struct sqlexd sqlstm;
#line 515 "offon_entry.pc"
     sqlstm.sqlvsn = 12;
#line 515 "offon_entry.pc"
     sqlstm.arrsiz = 13;
#line 515 "offon_entry.pc"
     sqlstm.sqladtp = &sqladt;
#line 515 "offon_entry.pc"
     sqlstm.sqltdsp = &sqltds;
#line 515 "offon_entry.pc"
     sqlstm.iters = (unsigned int  )1;
#line 515 "offon_entry.pc"
     sqlstm.offset = (unsigned int  )197;
#line 515 "offon_entry.pc"
     sqlstm.cud = sqlcud0;
#line 515 "offon_entry.pc"
     sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 515 "offon_entry.pc"
     sqlstm.sqlety = (unsigned short)4352;
#line 515 "offon_entry.pc"
     sqlstm.occurs = (unsigned int  )0;
#line 515 "offon_entry.pc"
     sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 515 "offon_entry.pc"
}

#line 515 "offon_entry.pc"

					exit(1);
				}
				gettimeofday(&t1, NULL);
				yy_time.tm4upd += time_diff1(t2, t1);

                if(send_req(connid, DECRYPT, TX_END, SRVCODE0001, 1, sizeof(struct op_data_req)*y)<0)
                {
                    fprintf(logfp,"send_req(%d) failed!\n",connid);
                    fclose(logfp);
                    exit(1);
                }

                readlen = readnet(connid,(char *)&tcpbuf,8);
                if(readlen!=8)
                {
                    fprintf(logfp,"readnet(%d,0,8)=%d failed\n",connid,readlen);
                    fclose(logfp);
                    exit(1);
                }

                if(strncmp(tcpbuf.flag,MSGFLAG,4))
                {
                    tcpbuf.msglen[0]=0;
                    fprintf(logfp,"MSGFLAG is incorrect [%s]!\n",tcpbuf.flag);
                    fclose(logfp);
                    exit(1);
                }

                msglen=get_length(tcpbuf.msglen, 4);
                if(msglen<32)
                {
                    fprintf(logfp,"MSGLEN is incorrect[%04d]\n",msglen);
                    fclose(logfp);
                    exit(1);
                }

                readlen=readnet(connid,(char *)&tcpbuf+8,msglen-8);
                if(readlen!=msglen-8)
                {
                    fprintf(logfp,"readnet(%d,8,%d)=%d failed!\n",
                        connid,msglen-8,readlen);
                    fclose(logfp);
                    exit(1);
                }
                gettimeofday(&t2, NULL);
                yy_time.tm4req += time_diff1(t1, t2);

                ackinfo=(struct op_data_ack *)tcpbuf.data;
                memset(retn,0x0,sizeof(retn));
                strncpy(retn,ackinfo->retn,4);

                /* EXEC SQL commit; */ 
#line 567 "offon_entry.pc"

{
#line 567 "offon_entry.pc"
                struct sqlexd sqlstm;
#line 567 "offon_entry.pc"
                sqlstm.sqlvsn = 12;
#line 567 "offon_entry.pc"
                sqlstm.arrsiz = 13;
#line 567 "offon_entry.pc"
                sqlstm.sqladtp = &sqladt;
#line 567 "offon_entry.pc"
                sqlstm.sqltdsp = &sqltds;
#line 567 "offon_entry.pc"
                sqlstm.iters = (unsigned int  )1;
#line 567 "offon_entry.pc"
                sqlstm.offset = (unsigned int  )212;
#line 567 "offon_entry.pc"
                sqlstm.cud = sqlcud0;
#line 567 "offon_entry.pc"
                sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 567 "offon_entry.pc"
                sqlstm.sqlety = (unsigned short)4352;
#line 567 "offon_entry.pc"
                sqlstm.occurs = (unsigned int  )0;
#line 567 "offon_entry.pc"
                sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 567 "offon_entry.pc"
}

#line 567 "offon_entry.pc"

                gettimeofday(&t1, NULL);
                yy_time.tm4dul += time_diff1(t2, t1);
                rec_idx += y-1;
            }

            if(rec_count > 0)
            {
            	fprintf(logfp,
					"rectime=%ld,rectype=etr,hlrcode=%s,db1=%s,rows=%d,tm4sel=%d,"
					"tm4fch=%d,tm4req=%d,tm4upd=%d,tm4ins=%d,tm4dul=%d\n",
					t, yy_time.hlrcode, yy_time.db1, yy_time.rows, yy_time.tm4sel/1000,
					yy_time.tm4fch/1000, yy_time.tm4req/1000, yy_time.tm4upd/1000, yy_time.tm4ins/1000, yy_time.tm4dul/1000);
            	fflush(logfp);
            }

    		time(&t);
    		logfp = open_logfp(t, logfp, hlrcode);
    		/*记录性能日志, 暂时约定每5分钟记录一次 2008-12-1*/
    		yy_time_log(t);

            if(endflag == 1)
                break;
		}

		/* EXEC SQL CLOSE cur_sel; */ 
#line 592 "offon_entry.pc"

{
#line 592 "offon_entry.pc"
  struct sqlexd sqlstm;
#line 592 "offon_entry.pc"
  sqlstm.sqlvsn = 12;
#line 592 "offon_entry.pc"
  sqlstm.arrsiz = 13;
#line 592 "offon_entry.pc"
  sqlstm.sqladtp = &sqladt;
#line 592 "offon_entry.pc"
  sqlstm.sqltdsp = &sqltds;
#line 592 "offon_entry.pc"
  sqlstm.iters = (unsigned int  )1;
#line 592 "offon_entry.pc"
  sqlstm.offset = (unsigned int  )227;
#line 592 "offon_entry.pc"
  sqlstm.cud = sqlcud0;
#line 592 "offon_entry.pc"
  sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 592 "offon_entry.pc"
  sqlstm.sqlety = (unsigned short)4352;
#line 592 "offon_entry.pc"
  sqlstm.occurs = (unsigned int  )0;
#line 592 "offon_entry.pc"
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 592 "offon_entry.pc"
}

#line 592 "offon_entry.pc"


        if(rec_count == 0){
            fprintf(logfp,"no datas,sleep 1s...[%ld]\n",t);
            fflush(logfp);
    		sleep(1);

            if(++count>3)
            {
                count=0;

                if(send_req(connid,DECRYPT,TX_END,CONN_KEEP,1,0)<0)
                {
                    fprintf(logfp,"send_req(%d) failed!\n",connid);
                    fclose(logfp);
                    exit(1);
                }

                readlen=readnet(connid,(char *)&tcpbuf,8);
                if(readlen!=8)
                {
                    fprintf(logfp,"readnet(%d,0,8)=%d failed\n",connid,readlen);
                    fclose(logfp);
                    exit(1);
                }

                if(strncmp(tcpbuf.flag,MSGFLAG,4))
                {
                    tcpbuf.msglen[0]=0;
                    fprintf(logfp,"MSGFLAG is incorrect [%s]!\n",tcpbuf.flag);
                    fclose(logfp);
                    exit(1);
                }

                msglen=get_length(tcpbuf.msglen,4);
                if(msglen<32)
                {
                    fprintf(logfp,"MSGLEN is incorrect[%04d]\n",msglen);
                    fclose(logfp);
                    exit(1);
                }

                readlen=readnet(connid,(char *)&tcpbuf+8,msglen-8);
                if(readlen!=msglen-8)
                {
                    fprintf(logfp,"readnet(%d,8,%d)=%d failed!\n",
                        connid,msglen-8,readlen);
                    fclose(logfp);
                    exit(1);
                }
                printf("RCV:%s[%d]\n",(char *)&tcpbuf,msglen);
            }
        }
	}

	fclose(logfp);

	return 0;
}
