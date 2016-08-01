/*********************************************************
 *
 *  FileName:
 *      offon_hlr.c
 *
 *  Version:
 *      1.00 2006.10.20
 *
 *  tabstop = 4
 *
 *
 * offon_hlr：
 用于模拟联机指令进程，功能与offon_alcatel一致，从manager提取命令后，等候一段时间后返回一个成功。
 时间间隔可以设置。
 启动格式：offon_hlr -Hhlrcode -Nhlrport -sspeed
 格式说明：hlrcode 和 hlrport与offon_alcatel一致；
	  Speed表示命令处理速度，单位是条/分钟；如：-s1200表示每分钟处理1200条命令
 *
 *
 ********************************************************/

#include "offon.h"
#include "db_func.h"

#define VERSION "2.2.0"

char hlrcode[8];
char hlrport[8];
FILE *logfp;
FILE *sndfp,*rcvfp;
int sndid,rcvid;
int speed;

struct timeval t1,t2;
time_t ttv;

char *procname;

struct comm_data	tcpbuf;
struct login_req	*loginreqptr;
struct login_ack	*loginackptr;
struct cmd_data_req	*cmdreqptr;
struct cmd_data_ack	*cmdackptr;

char encrypt_char;
char cipher[16];
int waitnum;

struct user_info	userhead,*userptr;
struct mcfg_info	mcfghead,*mcfgptr;

struct order_info	orderhead,*orderptr,*ordertail;
struct reply_info	replyhead,*replyptr,*replytail;

int					connid;
int					commport;
struct sockaddr_in	srvaddr;

int					gsmid;
char				gsmip[32];
int					gsmport;
struct sockaddr_in	gsmaddr;
char				gsmuser[32];
char				gsmpswd[32];

int timeout;
char ctrla[2];
char enter[2];
char orderinfo[1024];
char replystr[65535];

#define		STATION		10

extern int GenDaemon();
extern int testdir();
extern int db_login();
extern char *rlspace(char *);


int test_flag = 0;


void usage(char *s)
{
	printf("%s\n\nUsage:%s -Hhlrcode -Nhlrport [-sspeed] [-Udbuser -Pdbpswd -Sdbname] [-Llogdir] [-v]\n\n\n",s,procname);
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

void catchalm(int sig)
{
	fprintf(logfp,"TIMEOUT!!![%ld]\n",time(NULL));
	fflush(logfp);
	timeout=1;

	return;
}

/*发送请求报文*/
int send_req(int id,char encrypt,int finished,int srvtype,int transid,int length)
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
	memcpy(tcpbuf.reserve,"     ",5);

printf("\nSND:%s[%d]\n",(char *)&tcpbuf,length);
	if(test_flag)
		_print_bin(&tcpbuf, length);
	return writenet(id,(char *)&tcpbuf,length);
}

/*接收应答报文*/
int read_ack(int sockid, struct comm_data *buf)
{
	int readlen;
	int msglen;

	readlen=readnet(sockid,(char *)buf, 8);
	if(readlen!=8)
	{
		fprintf(logfp,"readnet(%d,0,8)=%d failed\n",sockid, readlen);
		return -1;
	}

	if(memcmp(buf->flag, MSGFLAG, 4))
	{
		buf->msglen[0]=0;
		fprintf(logfp,"MSGFLAG is incorrect [%s]!\n",tcpbuf.flag);
		return -2;
	}

	msglen = get_length(buf->msglen,4);
	if(msglen<32)
	{
		fprintf(logfp,"MSGLEN is incorrect[%04d]\n",msglen);
		return -3;
	}

	readlen = readnet(sockid, ((char *)buf)+8, msglen-8);
	if(readlen != msglen-8)
	{
		fprintf(logfp,"readnet(%d,8,%d)=%d failed!\n",connid,msglen-8,readlen);
		return -4;
	}
	printf("\nRCV:%s[%d]\n",(char *)&tcpbuf,msglen);
	if(test_flag)
	    _print_bin(&tcpbuf, msglen);
	return 0;
}

int login_server(int id,char *hlrcode,char *hlrport, struct comm_data *buf)
{
	int readlen,msglen,retn;

	/************ GET USER INFO ************************/
	memset(&userhead,0x0,sizeof(userhead));
	if(get_one_userinfo(hlrcode,hlrport,&userhead)<=0)
	{
		fprintf(logfp,"get_one_userinfo() failed!\n");
		return -1;
	}

	loginreqptr=(struct login_req *)buf->data;
	strcpy(loginreqptr->username,userhead.user);
	strcpy(loginreqptr->userpswd,userhead.pswd);
	strcpy(loginreqptr->hlrcode,userhead.hlrcode);
	loginreqptr->hlrport=userhead.hlrport;
	loginreqptr->termtype=userhead.type;
	/*memcpy(loginreqptr->reserve, "er", 2);*/

	if(send_req(id,DECRYPT,TX_END,SRV_LOGIN,1,LOGINREQLEN)<0)
	{
		fprintf(logfp,"send_req(%d) failed[%d]\n",id,errno);
		return -2;
	}

	retn = read_ack(id, buf);
	if(retn != 0){
		printf("in login_server: read_ack faild:%d\n", retn);
		return -1;
	}
	printf("RCV:%s~\n",(char *)buf);

	loginackptr=(struct login_ack *)buf->data;
	retn=get_length(loginackptr->retn,4);
	encrypt_char=loginackptr->encrypt;
	memset(cipher,0x0,sizeof(cipher));
	strncpy(cipher,loginackptr->cipher,8);

	return retn;
}

int exec_cmd(char *data)
{
	char s[32];
	int retn;
	char msisdn[16],stream[16],ocode[8];
	int ordercode;
	char *src,*dest,*para;
	int beg,len;
	char *ptr;

	cmdackptr=(struct cmd_data_ack *)data;

	/* printf("get_length(cmdackptr->retn,4)====%d\n",get_length(cmdackptr->retn,4)); */
	switch(get_length(cmdackptr->retn,4))
	{
	case 3001:
		if(++waitnum>30)
		{
			fprintf(logfp,"Now no datas[%ld][%d]\n",time(NULL),waitnum);
			waitnum=0;
		}

		fflush(logfp);
		sleep(3);

		cmdreqptr = (struct cmd_data_req *)data;
		memcpy(cmdreqptr->retn,"0000",4);
		cmdreqptr->type=ONLY_GET;

		return 0;
	case 0001:
		waitnum=0;
		cmdackptr=(struct cmd_data_ack *)data;
		cmdreqptr=(struct cmd_data_req *)data;

		memcpy(msisdn,cmdackptr->phone_no,MSISDNLEN);
		memcpy(stream,cmdackptr->stream_id,16);

		ordercode=get_length(cmdackptr->ordercode,4);

		/*usleep(60000000/speed);*/
        if(speed > 0)
		    usleep(speed);

		sprintf(cmdreqptr->retn,"%04d",0);
		cmdreqptr->type=REPLY_GET;
		memcpy(cmdreqptr->stream_id,stream,16);
		memcpy(cmdreqptr->phone_no,msisdn,MSISDNLEN);
		sprintf(ocode,"%04d",ordercode);
		memcpy(cmdreqptr->ordercode,ocode,4);

		fprintf(logfp, "phone_no=%s order=%04d\n", msisdn, ordercode);
		fflush(logfp);

		return 1;
	default:
		fprintf(logfp,"UNKNOW INFO[%04d]!\n",retn);
		fflush(logfp);
		return -1;
	}
}

FILE * open_logfp(time_t t, FILE *fp1, char *hlrcode, char *hlrport)
{
	struct tm *tv;
	char fname[128];

	static int curt = 0;

	tv=localtime(&t);
	if(tv->tm_mday != curt)
	{
		sprintf(fname,"%s/hlr_%04d%02d%02d.%s.%s",
				getenv("LOGDIR"), tv->tm_year+1900, tv->tm_mon+1,
				tv->tm_mday, hlrcode, hlrport);
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

int tt[10];
int main(int argc,char **argv,char **envp)
{
	long	command_id;
	char	hlr_code[8];
	char	phone_no[20];
	char	command_code[8];
	char	op_code[8];
	char	imsi_no[32];
	char	new_phone[20];
	char	new_imsi[32];
	char	business_status[8];

	int ret;
	char inputstr[2048],ss[64];
	int i,status,today;
	char srvip[32];
	int srvport;
	char dbuser[32],dbpswd[32],dbname[32],tname[64];
	char logfilename[256],logdir[256];
	time_t t;
	struct tm *tv;

	char chhlr;
	int msglen,readlen,headlen,tcplen,bodylen;
	int transid;

	char order_fname[256],reply_fname[256],cfginfo[1072],*p,*d;
	int line;
	FILE *cfgfp;
	char cfgstr[256];
	struct timeval t1, t2, t3, t4;

	procname=argv[0];
	logfp=NULL;

	memset(inputstr,0x0,sizeof(inputstr));
	for(i=0;i<argc;i++)
	{
		strcat(inputstr,argv[i]);
		strcat(inputstr," ");
	}

	if(argc<3)
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
	memset(hlrport,0x0,sizeof(hlrport));
	memset(dbuser,0x0,sizeof(dbuser));
	memset(dbpswd,0x0,sizeof(dbpswd));
	memset(dbname,0x0,sizeof(dbname));
	memset(logdir,0x0,sizeof(logdir));
	speed=120;

	while(--argc>0)
	{
		if(argv[argc][0]!='-')
			usage("Incorrect Parameter!");

		switch(argv[argc][1])
		{
		case 'H':
			strncpy(hlrcode,argv[argc]+2,4);
			break;
		case 'N':
			strncpy(hlrport,argv[argc]+2,4);
			break;
		case 's':
			speed=atoi(argv[argc]+2);
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
		case 'V':
		case 'v':
			printf("\n\nversion : %s\n\n",VERSION);
			exit(0);
		case 'T':
		case 't':
			test_flag = 1;
			break;
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
	logfp = open_logfp(t, NULL, hlrcode, hlrport);
	memset(&tt, 0, sizeof(tt));

	tv=localtime(&t);
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

		cfgfp=fopen(cfgstr, "r");
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

	/************ INIT COMM_PORT ***********************/
	commport=get_commport(hlrcode,srvip);
	if(commport<1024 || commport>65535)
	{
		fprintf(logfp,"get_commport(%s) == %d failed[%d]\n",commport,errno);
		fclose(logfp);
		exit(1);
	}

	memset(dbuser,0x0,sizeof(dbuser));
	memset(dbpswd,0x0,sizeof(dbpswd));
	memset(tname,0x0,sizeof(tname));

	connid = tcp_connect(srvip, commport);
	if(connid < 0){
		fprintf(logfp, "connect %s:%d failed\n", srvip, commport);
		fclose(logfp);
		exit(1);
	}

	if(login_server(connid, hlrcode, hlrport, &tcpbuf))
	{
		fprintf(logfp,"login_server() failed!\n");
		fclose(logfp);
		exit(1);
	}
	fprintf(logfp,"login_server(%s) success!\n",hlrcode);

	db_logout();

	fprintf(logfp,"LOGIN HLR,SUCCESS![%ld]\n",time(NULL));
	fflush(logfp);

	/************** GET AN ORDER AND DEAL ********************/
	waitnum=0;
	transid=2;
	cmdreqptr=(struct cmd_data_req *)tcpbuf.data;
	cmdreqptr->type=ONLY_GET;

	gettimeofday(&t3, NULL);
	while(1)
	{
		gettimeofday(&t1, NULL);
		if(send_req(connid,DECRYPT,TX_END,GETORDER,transid++,CMDREQLEN)<0)
		{
			fprintf(logfp,"send_req(%d) failed[%d]\n",connid,errno);
			break;
		}

		readlen = read_ack(connid, &tcpbuf);
		if(readlen != 0){
			printf("read_ack failed: %d\n", readlen);
			break;
		}
		gettimeofday(&t2, NULL);
		tt[1] += time_diff1(t1, t2);
/*printf("RCV:%s~\n",(char *)&tcpbuf); */

		ret = exec_cmd(tcpbuf.data);
		gettimeofday(&t1, NULL);
		tt[2] += time_diff1(t2, t1);
		if(ret == 1){
			tt[0] ++;
		}
printf("EXEC_CMD===========%d\n",ret);
		if(ret<0)
		{
			fprintf(logfp,"exec_cmd() failed!\n");
			break;
		}

		if(ret == 0 && tt[0] > 0){
			for(i=0; i<10; i++)
				printf("tt[%d]=%d\n", i, tt[i]);
			gettimeofday(&t4, NULL);
			printf("whole time: %d\n", time_diff1(t3, t4));
			fflush(stdout);
			memset(&tt, 0, sizeof(tt));
		}
		time(&t);
		logfp = open_logfp(t, logfp, hlrcode, hlrport);
		gettimeofday(&t2, NULL);
		tt[3] += time_diff1(t1, t2);
	}

	fclose(logfp);
	close(connid);

	return 0;
}

