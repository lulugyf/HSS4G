/*********************************************************
 *
 *  FileName:
 *      offon_ericsson.c
 *
 *  Version:
 *      1.00 2005.06.07
 *
 *  tabstop = 4
 *
 ********************************************************/

#include "offon.h"
#include "db_func.h"

extern int writenet();
extern int readnet();
extern int get_length();
extern int get_commport();
extern int db_logout();
extern int get_one_userinfo();
extern int get_cfg_file();

char hlrcode[8];
char hlrport[8];
FILE *logfp;
FILE *sndfp,*rcvfp;
int sndid,rcvid;

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

int test_flag = 0;

#define		STATION		10

extern int GenDaemon();
extern int testdir();
extern int db_login();
extern char *rlspace(char *);

void usage(char *s)
{
	printf("%s\n\nUsage:%s -Hhlrcode -Nhlrport [-Udbuser -Pdbpswd -Sdbname] [-Llogdir] [-V]\n\n\n",s,procname);
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
	if(test_flag == 1)
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
	if(test_flag == 1)
		_print_bin(&tcpbuf, msglen);
	return 0;
}

int login_server(int id,char *hlrcode,char *hlrport,
		char *mgr_user, char *mgr_pswd, char *mgr_term)
{
	int readlen,msglen,retn;

	loginreqptr=(struct login_req *)tcpbuf.data;
	strcpy(loginreqptr->username, mgr_user);
	strcpy(loginreqptr->userpswd, mgr_pswd);
	strcpy(loginreqptr->hlrcode, hlrcode);
	loginreqptr->hlrport = hlrport[0];
	loginreqptr->termtype = mgr_term[0];
	memcpy(loginreqptr->reserve, "  ", 2);

	if(send_req(id,DECRYPT,TX_END,SRV_LOGIN,1,LOGINREQLEN)<0)
	{
		fprintf(logfp,"send_req(%d) failed[%d]\n",id,errno);
		return -2;
	}

	readlen = read_ack(connid, &tcpbuf);
	if(readlen != 0)
	{
		fprintf(logfp,"readnet(%d,8,%d)=%d failed!\n",id,msglen-8,readlen);
		return -6;
	}
printf("RCV:%s~\n",(char *)&tcpbuf);

	loginackptr=(struct login_ack *)tcpbuf.data;
	retn=get_length(loginackptr->retn,4);
	encrypt_char=loginackptr->encrypt;
	memset(cipher,0x0,sizeof(cipher));
	strncpy(cipher,loginackptr->cipher,8);
printf("cipher===%s~\n",cipher);

	return retn;
}

void cusnd(char *s,int len)
{
	if(len==1)
		fprintf(logfp,"[SND:0x%02x]\n",*s);
	else
		fprintf(logfp,"[SND:%s][%d]\n",s,len);

	if(write(sndid,s,len)!=len)
	{
		fprintf(logfp,"write(%d,%02x) failed[%d]\n",sndid,*s,errno);
		fclose(logfp);
		exit(1);
	}

	return;
}

void RcvFrmFlag()
{
	char *ptr;
	int ret,i;

	timeout=0;
	fprintf(logfp,"\nBEG:\n");
	signal(SIGALRM,catchalm);

	ptr=replystr+STATION;

	for(;;)
	{
		alarm(60);
		ret=read(rcvid,ptr,8192);
		alarm(0);
		if(ret==0)
		{
			fprintf(logfp,"disconnect from HLR![%ld]\n",time(NULL));
			fclose(logfp);
			exit(1);
		}
		else if(ret<0)
		{
			if(timeout)
				fprintf(logfp,"Can't receive datas from HLR[%ld]\n",time(NULL));
			else
				fprintf(logfp,"disconnect from HLR[errno=%d]![%ld]\n",
					errno,time(NULL));

			fclose(logfp);
			exit(1);
		}

		for(i=0;i<ret;i++,ptr++)
		{
			if(*ptr==0x00) *ptr=0x20;
		}

		if((*(ptr-2)==0x03 && *(ptr-1)==0x3c) || *(ptr-1)==0x3a || *(ptr-2)==0x3a)
		{
			fflush(logfp);
			*ptr=0x0;

			break;
		}
	}

	signal(SIGALRM,SIG_IGN);
	fprintf(logfp,"%s\nEND!\n",replystr+STATION);
	fflush(logfp);

	return;
}

void RcvFrmN()
{
	char *ptr;
	int ret,i;

	timeout=0;
	fprintf(logfp,"\nBEG:\n");
	signal(SIGALRM,catchalm);

	ptr=replystr+STATION;

	for(;;)
	{
		alarm(60);
		ret=read(rcvid,ptr,8192);
		alarm(0);
		if(ret==0)
		{
			fprintf(logfp,"disconnect from HLR![%ld]\n",time(NULL));
			fclose(logfp);
			exit(1);
		}
		else if(ret<0)
		{
			if(timeout)
				fprintf(logfp,"Can't receive datas from HLR[%ld]\n",time(NULL));
			else
				fprintf(logfp,"disconnect from HLR[errno=%d]![%ld]\n",
					errno,time(NULL));

			fclose(logfp);
			exit(1);
		}

		for(i=0;i<ret;i++,ptr++)
		{
			if(*ptr==0x00) *ptr=0x20;
/*
			fprintf(logfp,"%c",*ptr);
printf("RCV: %c		%02x\n",*ptr,*ptr);
*/
		}

		if(*(ptr-2)==0x03 && *(ptr-1)==0x3c)
		{
			fflush(logfp);
			*ptr=0x0;

			break;
		}
	}

	signal(SIGALRM,SIG_IGN);
	fprintf(logfp,"%s\nEND!\n",replystr+STATION);
	fflush(logfp);

	return;
}

int parse_queryinfo(char *s)
{
	/*************************
	char *imsip,*msisdnp,*p;
	struct query_info *ptr;
	int i;

	ptr=(struct query_info *)s;
	*************************/

	if(strstr(replystr+STATION,"NOT CONNECTED"))
		return 2001;

	if(strstr(replystr+STATION,"NOT ACCEPTED"))
		return 2003;

	if(strstr(replystr+STATION,"FORMAT ERROR"))
		return 2004;

	if(strstr(replystr+STATION,"UNREASONABLE VALUE"))
		return 2005;

	if(strstr(replystr+STATION,"FUNCTION BUSY"))
		return 2006;

	if(strstr(replystr+STATION,"DURING DUMP"))
		return 2007;

	/*******************************************
	msisdnp=strstr(replystr+STATION,"8613");
	if(msisdnp)
		memcpy(ptr->msisdn,msisdnp+2,11);

	imsip=strstr(replystr+STATION,"  4600");
	if(imsip)
	{
		memcpy(ptr->imsi,imsip+2,15);
	}

	if(strstr(replystr+STATION,"BOIC    ACTIVE-OP"))
	{
		ptr->toll='2';
	}
	else if(strstr(replystr+STATION,"OBO-2"))
	{
		ptr->toll='2';
	}
	else if(strstr(replystr+STATION,"OBO-4"))
	{
		ptr->toll='2';
	}
	else if(strstr(replystr+STATION,"OBO-1"))
	{
		ptr->toll='2';
	}
	else
	{
		ptr->toll='1';
	}

	if(strstr(replystr+STATION,"OBR-2"))
	{
		ptr->roam='2';
	}
	else if(strstr(replystr+STATION,"OBR-1"))
	{
		ptr->roam='3';
	}
	else
	{
		ptr->roam='1';
	}

	if(strstr(replystr+STATION,"NAM\r\n0"))
		ptr->gprs='y';
	else
		ptr->gprs='n';

	if(strstr(replystr+STATION,"TS22-1"))
		ptr->sms_snd='y';
	else
		ptr->sms_snd='n';

	if(strstr(replystr+STATION,"TS21-1"))
		ptr->sms_rcv='y';
	else
		ptr->sms_rcv='n';

	if(strstr(replystr+STATION,"CFU-1"))
		ptr->cfu='y';
	else
		ptr->cfu='n';

	if(strstr(replystr+STATION,"CFB-1"))
		ptr->cfbusy='y';
	else
		ptr->cfbusy='n';

	if(strstr(replystr+STATION,"CFNRY-1"))
		ptr->cfnreply='y';
	else
		ptr->cfnreply='n';

	if(strstr(replystr+STATION,"CFNRC-1"))
		ptr->cfnreach='y';
	else
		ptr->cfnreach='n';

	if(strstr(replystr+STATION,"CAW-1"))
		ptr->cw='y';
	else
		ptr->cw='n';

	if(strstr(replystr+STATION,"HOLD-1"))
		ptr->ch='y';
	else
		ptr->ch='n';

	if(strstr(replystr+STATION,"MPTY-1"))
		ptr->mpty='y';
	else
		ptr->mpty='n';

	if(strstr(replystr+STATION,"BAOC    ACTIVE-OP"))
		ptr->cboc='y';
	else
		ptr->cboc='n';

	if(strstr(replystr+STATION,"BAIC    ACTIVE-OP"))
		ptr->cbic='y';
	else
		ptr->cbic='n';

	if(strstr(replystr+STATION,"TS61-1"))
		ptr->fax='y';
	else
		ptr->fax='n';

	if(strstr(replystr+STATION,"BS26-1"))
		ptr->bs='y';
	else
		ptr->bs='n';

	if(strstr(replystr+STATION,"CLIP-1"))
		ptr->clip='y';
	else
		ptr->clip='n';

	if(strstr(replystr+STATION,"SOCLIP-1"))
		ptr->soclip='y';
	else
		ptr->soclip='n';

	if(strstr(replystr+STATION,"MRBT-1"))
		ptr->mrbt='y';
	else
		ptr->mrbt='n';

	if(strstr(replystr+STATION,"ICI-1"))
		ptr->ici='y';
	else
		ptr->ici='n';

	if(strstr(replystr+STATION,"CLIR-1"))
		ptr->clir='y';
	else
		ptr->clir='n';

	if(strstr(replystr+STATION,"SOCLIR-1"))
		ptr->soclir='y';
	else
		ptr->soclir='n';

	if(strstr(replystr+STATION,"OBO-1"))
		ptr->odboc='y';
	else
		ptr->odboc='n';

	if(strstr(replystr+STATION,"OBI-1"))
		ptr->odbic='y';
	else
		ptr->odbic='n';

	if(strstr(replystr+STATION,"BS31-1"))
		ptr->bs31='y';
	else
		ptr->bs31='n';

	if(strstr(replystr+STATION,"TCSI-1"))
		ptr->tcsi='y';
	else
		ptr->tcsi='n';

	if(strstr(replystr+STATION,"OCSI-1"))
		ptr->ocsi='y';
	else
		ptr->ocsi='n';
	**********************************************/

	return 0000;
}

int reply_parse()
{
	replyptr=replyhead.next;
	while(replyptr)
	{
		if(strstr(replystr+STATION,replyptr->replyinfo))
			return replyptr->replycode;

		replyptr=replyptr->next;
	}

	return 3998;
}

int exec_cmd()
{
	char s[32];
	int retn;
	char msisdn[16],stream[16],ocode[8];
	int ordercode;
	char *src,*dest,*para;
	int beg,len;
	char *ptr, tmp[50];
    struct timeval tt1,tt2;

	cmdackptr=(struct cmd_data_ack *)tcpbuf.data;

printf("get_length(cmdackptr->retn,4)====%d\n",get_length(cmdackptr->retn,4));
	switch(get_length(cmdackptr->retn,4))
	{
	case 3001:
		if(++waitnum>30)
		{
			fprintf(logfp,"Now no datas[%ld][%d]\n",time(NULL),waitnum);
			waitnum=0;
			cusnd(enter,1);
			RcvFrmN();
		}

		fflush(logfp);
		sleep(3);

		cmdreqptr=(struct cmd_data_req *)tcpbuf.data;
		memcpy(cmdreqptr->retn,"0000",4);
		cmdreqptr->type=ONLY_GET;

		return 0;
	case 0001:
		fprintf(logfp,"GET_DATAS:\n");
		waitnum=0;
		cmdackptr=(struct cmd_data_ack *)tcpbuf.data;
		cmdreqptr=(struct cmd_data_req *)tcpbuf.data;

		memcpy(msisdn,cmdackptr->phone_no,MSISDNLEN);
		memcpy(stream,cmdackptr->stream_id,16);

		ordercode=get_length(cmdackptr->ordercode,4);

		fprintf(logfp,"--retn========%04d~\n",get_length(cmdackptr->retn,4));
		fprintf(logfp,"--stream_id===%s~\n",cmdackptr->stream_id);
		fprintf(logfp,"--ordercode===%04d~\n",ordercode);
		fprintf(logfp,"--phone_no====%s~\n",cmdackptr->phone_no);
		fprintf(logfp,"--imsi_no=====%s~\n",cmdackptr->imsi_no);
		fprintf(logfp,"--ss_info1====%s~\n",cmdackptr->ss_info1);
		fprintf(logfp,"--ss_info2====%s~\n",cmdackptr->ss_info2);
		fprintf(logfp,"--ss_info3====%s~\n",cmdackptr->ss_info3);
		fprintf(logfp,"--time========%ld~\n",time(NULL));

		/*** ORDER SND TO HLR ***/
		orderptr=orderhead.next;
		while(orderptr)
		{
			if(orderptr->ordercode==ordercode)
				break;
			else
				orderptr=orderptr->next;
		}

		if(orderptr)
		{
			src=orderptr->orderinfo;
			dest=orderinfo;

			for(;*src;)
			{
				switch(*src)
				{
				case '$':
					src++;
					switch(*src)
					{
					case '1':
						para=cmdackptr->phone_no;
						break;
					case '2':
						para=cmdackptr->imsi_no;
						break;
					case '3':
						para=cmdackptr->ss_info1;
						break;
					case '4':
						para=cmdackptr->ss_info2;
						break;
					default:
						para=cmdackptr->ss_info3;
						break;
					}

					src++;
					beg=get_length(src,2);
					src+=2;
					len=get_length(src,2);
					if(len==0)
						len=strlen(para+beg);

					memcpy(dest,para+beg,len);
					src+=2;
					dest+=len;

					break;
				default:
					*dest=*src;
					src++;
					dest++;

					break;
				}
			}
			*dest='\r';
			dest++;
			*dest=0x0;

gettimeofday(&tt1, NULL);
			cusnd(orderinfo,strlen(orderinfo));
			RcvFrmN();
gettimeofday(&tt2, NULL);
sprintf(tmp, "order_exec %d ", ordercode);
cal_time1(tmp, tt1, tt2);

			if(strstr(replystr+STATION,"DURING DUMP"))
			{
				fprintf(logfp,"END!!!\n");
				fclose(logfp);
				exit(1);
			}

			retn=reply_parse();
			fprintf(logfp,"RETN:%04d\n",retn);

			if(retn==0 && ordercode>9000)
			{
				memset(cmdreqptr->info,' ',sizeof(cmdreqptr->info));
				cal_query_cfg(replystr+STATION,cmdreqptr->info);
				fprintf(logfp,"INFO:%s~\n",cmdreqptr->info);
			}
		}
		else
		{
			fprintf(logfp,"UNKNOW ORDER_CODE!\n");
			retn=21;
		}

		sprintf(cmdreqptr->retn,"%04d",retn);
		cmdreqptr->type=REPLY_GET;
		memcpy(cmdreqptr->stream_id,stream,16);
		memcpy(cmdreqptr->phone_no,msisdn,MSISDNLEN);
		sprintf(ocode,"%04d",ordercode);
		memcpy(cmdreqptr->ordercode,ocode,4);

		return 1;
	default:
		fprintf(logfp,"UNKNOW INFO[%04d]!\n",retn);
		fflush(logfp);
		return -1;
	}
}

int Init_For_Switch(char *hlrname,int gsmport,char **envp)
{
	char pscmd[256],cmdstr[256];
	int in[2],ou[2],childpid,Length;
	char hlrp[8];

	if (sndfp!=NULL&&rcvfp!=NULL){
		printf("send ~.\n");
		fclose(sndfp);
		fclose(rcvfp);
	}
	printf("Begin cusnd:\n");

	memset(pscmd,0x0,sizeof(pscmd));
	sprintf(pscmd,"ps -ef|grep telnet|grep %s|grep -v grep|grep -v agent|awk \'{print $2}\'|xargs kill -9",hlrname);
	system(pscmd);
	system(pscmd);

	pipe(in);
	pipe(ou);

	if((childpid=fork())<0)
	{
		perror("Err:3890  fork");
		return(-1);
	}

	if(childpid==0)
	{
		if(close(0)==-1||close(1)==-1)
		{
			perror("Err:3880  close");
			exit(1);
		}
		if(dup(ou[0])!=0||dup(in[1])!=1)
		{
			perror("Err:3880  dup");
			exit(1);
		}
		if(close(in[1])==-1||close(ou[0])==-1||	close(in[0])==-1||close(ou[1])==-1)
		{
			perror("Err:3880  close");
			exit(1);
		}

		if(gsmport==5000)
		{
			if(execle("/usr/bin/telnet","telnet",hlrname,"5000",NULL,envp)>0) {
				printf("execle telnet -e\n");
			}
		}
		else
		{
			if(execle("/usr/bin/telnet","telnet",hlrname,NULL,envp)>0) {
				printf("execle telnet -e\n");
			}
		}
	}
	if(close(in[1])==-1||close(ou[0])==-1) {
		printf("Err:3880  fdopen\n");
		return(-1);
	}
	if((sndfp=fdopen(ou[1],"w"))==NULL) {
		printf("Err:3880  fdopen\n");
		return(-1);
	}
	if((rcvfp=fdopen(in[0],"r"))==NULL) {
		printf("Err:3880  fdopen\n");
		fclose(sndfp);
		return(-1);
	}

	return 0;
}


struct
{
	char hlrcode[6];
	char hlrport[5];
	time_t rec_time;
	int rows;
	int tm4get;
	int tm4exc;
} yy_time;

void yy_time_log(time_t t)
{
	if(t-yy_time.rec_time >= 300){
		file_log("perf",
				"rectime=%ld,rectype=hlrerc,hlrcode=%s,hlrport=%s,"
				"rows=%d,tm4get=%d,tm4exc=%d",
				t, yy_time.hlrcode, yy_time.hlrport, yy_time.rows,
				yy_time.tm4get/1000, yy_time.tm4exc/1000);
		yy_time.rows = 0;
		yy_time.tm4exc = 0;
		yy_time.tm4get = 0;
		yy_time.rec_time = t;
	}
}


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
	char mgr_user[20], mgr_pswd[20], mgr_term[10];

	int ret;
	char inputstr[2048],ss[64];
	int i,status,today;
	char srvip[32];
	int srvport;
	char logfilename[256],logdir[256];
	time_t t;
	struct tm *tv;

	char chhlr;
	int msglen,readlen,headlen,tcplen,bodylen;
	int transid;

	char order_fname[256],reply_fname[256],query_fname[256],cfginfo[1024],*p,*d;
	int line;
	FILE *cfgfp;
	char cfgstr[256];

	procname=argv[0];
	logfp=NULL;

	memset(inputstr,0x0,sizeof(inputstr));
	for(i=0;i<argc;i++)
	{
		strcat(inputstr,argv[i]);
		strcat(inputstr," ");
	}

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
	memset(logdir,0x0,sizeof(logdir));

	while(--argc>0)
	{
		if(argv[argc][0]!='-')
			usage("Incorrect Parameter!");

printf("CMD:%c\n",argv[argc][1]);
		switch(argv[argc][1])
		{
		case 'H':
			strncpy(hlrcode,argv[argc]+2,4);
			break;
		case 'N':
			strncpy(hlrport,argv[argc]+2,4);
			break;
		case 'L':
			strncpy(logdir,argv[argc]+2,240);
			break;
		case 'V':
			printf("\n\n	version = %s\n\n",VERSION);
			exit(0);
		case 't':
		case 'T':
			test_flag = 1;
			break;
		default:
			usage("Unknow Parameter!");
			break;
		}
	}

printf("hlrcode===%s[%s]~\n",hlrcode,hlrport);
	if(strlen(hlrcode)!=3)
		usage("Incorrect parameter [hlrcode]!\n");

	if(strlen(hlrport)!=1)
		usage("Incorrect parameter [hlrport]!\n");

	if(strlen(logdir)<1)
		strcpy(logdir,getenv("LOGDIR"));

	if(testdir(logdir))
		usage("Incorrect parameter [logdir]!");

	time(&t);
	tv=localtime(&t);
	memset(logfilename,0x0,sizeof(logfilename));
	sprintf(logfilename,"%s/erc%04d%02d%02d.%s.%s",logdir,
		tv->tm_year+1900,tv->tm_mon+1,tv->tm_mday,hlrcode,hlrport);

	logfp=fopen(logfilename,"a");
	if(logfp==NULL)
	{
		printf("can't open %s for LOG!\n",logfilename);
		exit(1);
	}

	fprintf(logfp,"INIT %04d/%02d/%02d %02d:%02d:%02d\n",
		tv->tm_year+1900,tv->tm_mon+1,tv->tm_mday,
		tv->tm_hour,tv->tm_min,tv->tm_sec);
	fprintf(logfp,"%s[%d]\n",inputstr,argc);
	fflush(logfp);
	today=tv->tm_mday;

	/************ 获取连接配置信息 ********************/
	if(get_hlr_cfg(hlrcode, atoi(hlrport), gsmip, &gsmport, gsmuser, gsmpswd,
		order_fname,reply_fname,query_fname, srvip, &commport, NULL,
		mgr_user, mgr_pswd, mgr_term) != 0)
	{
		fprintf(logfp,"get_cfg_filename(%s) failed!\n",hlrcode);
		fclose(logfp);
		exit(1);
	}

	/************ GET ORDER INFO ***********************/
	cfgfp=fopen(order_fname,"r");
	if(cfgfp==NULL)
	{
		fprintf(logfp,"Can't open %s for read[%d]\n",order_fname,errno);
		fclose(logfp);
		exit(1);
	}

	line=0;
	memset(cfginfo,0x0,sizeof(cfginfo));
	while(fgets(cfginfo,sizeof(cfginfo),cfgfp))
	{
		line++;
		if(cfginfo[0]=='#' || cfginfo[0]=='\r' || cfginfo[0]=='\n')
		{
			memset(cfginfo,0x0,sizeof(cfginfo));
			continue;
		}

		if(strncmp(cfginfo,"OP",2) || (p=strchr(cfginfo,':'))==NULL)
		{
			fprintf(logfp,"Incorrect cfg_info[line=%d]\n",line);
			fclose(cfgfp);
			fclose(logfp);
			exit(1);
		}

		orderptr=(struct order_info *)malloc(sizeof(struct order_info));
		memset(orderptr,0x0,sizeof(struct order_info));

		orderptr->ordercode=atoi(cfginfo+2);
		strcpy(orderptr->orderinfo,rlspace(p+1));
		if(strlen(orderptr->orderinfo)==0 || strlen(orderptr->orderinfo)>1000)
		{
			fprintf(logfp,"Incorrect cfg_info[line=%d]\n",line);
			fclose(cfgfp);
			fclose(logfp);
			exit(1);
		}

		ordertail=&orderhead;
		while(ordertail->next)
		{
			if(ordertail->next->ordercode==orderptr->ordercode)
			{
				fprintf(logfp,"ORDER_CODE[%d] is exists[line=%d]\n",
					orderptr->ordercode,line);
				fclose(logfp);
				fclose(cfgfp);
				exit(1);
			}

			ordertail=ordertail->next;
		}

		ordertail->next=orderptr;

		memset(cfginfo,0x0,sizeof(cfginfo));
	}
	fclose(cfgfp);

	fprintf(logfp,"ORDER INFO:\n");
	orderptr=orderhead.next;
	while(orderptr)
	{
		fprintf(logfp,"%06d = [%s]\n",orderptr->ordercode,orderptr->orderinfo);
		orderptr=orderptr->next;
	}
	fflush(logfp);

	/************ GET REPLY INFO ***********************/
	cfgfp=fopen(reply_fname,"r");
	if(cfgfp==NULL)
	{
		fprintf(logfp,"Can't open %s for read[%d]\n",reply_fname,errno);
		fclose(logfp);
		exit(1);
	}

	fprintf(logfp,"REPLY INFO:\n");
	line=0;
	memset(cfginfo,0x0,sizeof(cfginfo));
	while(fgets(cfginfo,sizeof(cfginfo),cfgfp))
	{
		line++;
		if(cfginfo[0]=='#' || cfginfo[0]=='\r' || cfginfo[0]=='\n')
		{
			memset(cfginfo,0x0,sizeof(cfginfo));
			continue;
		}

		fprintf(logfp,"%s",cfginfo);

		if(strncmp(cfginfo,"RETN:",5) || cfginfo[9]!=':')
		{
			fprintf(logfp,"Incorrect cfg_info[line=%d]\n",line);
			fclose(cfgfp);
			fclose(logfp);
			exit(1);
		}

		replyptr=(struct reply_info *)malloc(sizeof(struct reply_info));
		memset(replyptr,0x0,sizeof(struct reply_info));

		replyptr->replycode=atoi(cfginfo+5);

		p=cfginfo+10;
		d=replyptr->replyinfo;

		if(*p!='[' || strchr(p,']')==NULL)
		{
			fprintf(logfp,"Incorrect cfg_info[line=%d]\n",line);
			fclose(cfgfp);
			fclose(logfp);
			exit(1);
		}

		while(*(++p)!=']')
		{
			if(*p=='\\')
			{
				p++;
				switch(*p)
				{
				case 'r':
					*d='\r';
					break;
				case 'n':
					*d='\n';
					break;
				case 't':
					*d='\t';
					break;
				default:
					*d=*p;
				}
			}
			else
				*d=*p;

			d++;
		}

		if(d==replyptr->replyinfo)
		{
			fprintf(logfp,"Incorrect cfg_info[line=%d]\n",line);
			fclose(cfgfp);
			fclose(logfp);
			exit(1);
		}

		replyptr->next=replyhead.next;
		replyhead.next=replyptr;

		memset(cfginfo,0x0,sizeof(cfginfo));
	}
	fclose(cfgfp);
	fflush(logfp);

	/************ GET QUERY INFO ***********************/
	ret=get_query_cfg(query_fname);
	if(ret)
	{
		fprintf(logfp,"get_query_cfg()===%d\n",ret);
		fclose(logfp);
		exit(1);
	}

	disp_query_cfg();

	/************ INIT COMM_PORT ***********************/


	connid=tcp_connect(srvip, commport);
	if(connid<0)
	{
		fprintf(logfp,"socket() failed[%d]\n",errno);
		fclose(logfp);
		exit(1);
	}

	if(login_server(connid, hlrcode, hlrport, mgr_user, mgr_pswd, mgr_term))
	{
		fprintf(logfp,"login_server() failed!\n");
		fclose(logfp);
		exit(1);
	}

	fprintf(logfp,"login_server(%s) success!\n",hlrcode);

	gsmid = tcp_connect(gsmip, gsmport);

	sndid=gsmid;
	rcvid=gsmid;


	/*** LOGIN HLR ***/
	sleep(1);

	enter[0]='\r';
	enter[1]=0x00;

	while(1)
	{
		RcvFrmFlag();

		if(strstr(replystr+STATION,"<"))
		{
			break;
		}

		if(strstr(replystr+STATION,"USERCODE"))
		{
			cusnd(gsmuser,strlen(gsmuser));
			cusnd(enter,1);
			continue;
		}

		if(strstr(replystr+STATION,"PASSWORD"))
		{
			cusnd(gsmpswd,strlen(gsmpswd));
			cusnd(enter,1);
			continue;
		}

		cusnd(enter,1);
	}

	fprintf(logfp,"LOGIN HLR,SUCCESS![%ld]\n",time(NULL));
	fflush(logfp);

	/************** GET AN ORDER AND DEAL ********************/
	waitnum=0;
	transid=2;
	cmdreqptr=(struct cmd_data_req *)tcpbuf.data;
	cmdreqptr->type=ONLY_GET;
	yy_time.rec_time = time(NULL);
	strcpy(yy_time.hlrcode, hlrcode);
	strcpy(yy_time.hlrport, hlrport);

	while(1)
	{
		int data_flag = 0;
		gettimeofday(&t1, NULL);
		if(send_req(connid,DECRYPT,TX_END,GETORDER,transid++,CMDREQLEN)<0)
		{
			fprintf(logfp,"send_req(%d) failed[%d]\n",connid,errno);
			break;
		}

		readlen = read_ack(connid, &tcpbuf);
		if(readlen != 0)
		{
			fprintf(logfp,"readnet(%d,8,%d)=%d failed!\n",connid,msglen-8,readlen);
			break;
		}
		gettimeofday(&t2, NULL);
		cmdackptr=(struct cmd_data_ack *)tcpbuf.data;
		data_flag = (get_length(cmdackptr->retn,4) == 0001);
		if(data_flag){
			yy_time.rows ++;
			yy_time.tm4get += time_diff1(t1, t2);
		}
printf("RCV:%s~\n",(char *)&tcpbuf);

		ret=exec_cmd();
		gettimeofday(&t1, NULL);
		if(data_flag){
			yy_time.tm4exc += time_diff1(t2, t1);
		}
printf("EXEC_CMD===========%d\n",ret);
		if(ret<0)
		{
			fprintf(logfp,"exec_cmd() failed!\n");
			break;
		}

		time(&t);
		yy_time_log(t);
		tv=localtime(&t);

		if(today!=tv->tm_mday)
		{
			fclose(logfp);

			memset(logfilename,0x0,sizeof(logfilename));
			sprintf(logfilename,"%s/erc%04d%02d%02d.%s.%s",logdir,
				tv->tm_year+1900,tv->tm_mon+1,tv->tm_mday,hlrcode,hlrport);

			logfp=fopen(logfilename,"a");
			if(logfp==NULL)
			{
				printf("can't open %s for LOG!\n",logfilename);
				exit(1);
			}

			today=tv->tm_mday;
		}

		fflush(logfp);
	}

	fclose(logfp);
	close(connid);

	return 0;
}

