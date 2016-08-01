/*********************************************************
 *
 *  FileName:
 *      offon_nokia_mml.c
 *
 *  Version:
 *      1.00 2008.7.7
 *
 *  tabstop = 4
 *
 ********************************************************/

#include "offon.h"
#include "db_func.h"
#include <sys/time.h>

extern int writenet();
extern int readnet();
extern int get_length();
extern int get_one_userinfo();
extern int get_cfg_file();
char *port_type = "nokia_mml";

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
    sndcmd(sndid, "\r\n< \08 ", "ZZZ;\r\n", 6,
                    "\r\nEND OF DIALOGUE SESSION", replystr, sizeof(replystr));

	exit(1);
}


void catchalm(int sig)
{
	fprintf(logfp,"TIMEOUT!!![%ld]\n",time(NULL));
	fflush(logfp);
	timeout=1;

	return;
}



/*执行类似telnet 命令的一次操作, 等待一个特定的串序列, 然后发送一个串, 然后读取输出
需要注意的是: 在执行命令的时候, 发送了一天指令后, 读取结果的时候应该会同时把提示符也读回来了,
    因此下一次执行指令的时候需要先在上次的buffer中找wait的串, 在发送指令之前先清空buffer

返回值为buffer中串的长度, 
*/
int sndcmd(int sockid, const char *wait, const char *cmd, const int cmdlen,
    const char *cmdend, char *reply_buf, const int reply_buflen)
{
    char *ptr;
    int ret, i, l;
    char buf[100];
    char *search_ptr;

    timeout = 0;
	signal(SIGALRM,catchalm);

    /*printf("wait[%s]\n", wait);
    printf("send[%s]\n", cmd);
    printf("reply_buf[%s]\n", reply_buf);*/

    ptr = strstr(reply_buf, wait);
    if(ptr == NULL){
        reply_buf[0] = 0;
        ptr = reply_buf;
        l = reply_buflen;
        i = -1;
        fprintf(logfp, "BEG1[");
        while( 1 ){
            alarm(20);
            ret = read(sockid, ptr, l);
            alarm(0);
            if(ret == 0){
                fprintf(logfp, "remote disconnected\n");
                break;
            }else if(ret < 0){
                fprintf(logfp, "socket read failed\n");
                break;
            }
            ptr[ret] = 0;
            fprintf(logfp, "%s", ptr);
            if(strstr(reply_buf, wait) != NULL){
                i = 1;
                break;
            }
            l -= ret;
            ptr += ret;
        }
        fprintf(logfp, "]\n");
        if(i != 1){
            fprintf(logfp, "can not found wait[%s] in session, read[%s]\n",
                wait, reply_buf);
            signal(SIGALRM,SIG_IGN);
            return -1;
        }
        fprintf(logfp, "%s", reply_buf);
    }
    /*reply_buf[0] = 0; */
    memset(reply_buf, 0, reply_buflen);
    search_ptr = reply_buf;
    alarm(20);
    ret = tcp_writen(sockid, cmd, cmdlen);
    fprintf(logfp, "CMD[%s]\n", cmd);
    alarm(0);
    if(ret != cmdlen){
        fprintf(logfp, "write cmd failed, wlen:%d, write:%d\n", cmdlen, ret);
        signal(SIGALRM,SIG_IGN);
        return -2;
    }
    ptr = reply_buf;
    l = reply_buflen;
    i = -1;
    fprintf(logfp, "BEG:【");
    while( 1 ){
        alarm(20);
        ret = read(sockid, ptr, l);
        alarm(0);
        if(ret == 0){
            fprintf(logfp, "remote disconnected\n");
            break;
        }else if(ret < 0){
            fprintf(logfp, "socket read failed\n");
            break;
        }
        ptr[ret] = 0;
        fprintf(logfp, "%s", ptr);
        if(strstr(reply_buf, cmdend) != NULL){
            i = 1;
            break;
        }

        /*对已知的交互过程进行处理*/
        if(strstr(search_ptr, "/*** SYNTAX ERROR ***/") != NULL){
            /*配置语法错误, 需要发送错误操作终止该指令执行*/
            strcpy(buf, "\b\b;\r");
            tcp_writen(sockid, buf, strlen(buf));
            search_ptr += strlen(search_ptr);
        }else if(strstr(search_ptr, "CONFIRM COMMAND EXECUTION: Y/N ? ") != NULL){
            /*需要确认指令执行, 发送 Y*/
            strcpy(buf, "Y\r");
            tcp_writen(sockid, buf, strlen(buf));
            search_ptr += strlen(search_ptr);
        }
        l -= ret;
        ptr += ret;
    }
    fprintf(logfp, "】\n");
    if(i != 1){
        fprintf(logfp, "can not found cmdend[%s], in[%s]\n",
            cmdend, reply_buf);
        signal(SIGALRM,SIG_IGN);
        return -3;
    }

    /*fprintf(logfp, "get_reply[%s]\n", reply_buf); */

    signal(SIGALRM,SIG_IGN);
    return strlen(reply_buf);
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

	return 0000;
}

int reply_parse()
{
    char *p0, *p1;
    char *s;

    s = replystr + STATION;

	replyptr=replyhead.next;
	while(replyptr)
	{
		if(strstr(s, replyptr->replyinfo))
			return replyptr->replycode;

		replyptr=replyptr->next;
	}

    if( (p0 = strstr(s, "/*** DX ERROR: ")) != NULL){
        p0 += 15;
        p1 = strstr(p0, " ***/");
        if(p1 != NULL){
            /*p1[0] = 0; */
            return atoi(p0) % 10000;
        }
    }

	return 3998;
}

int exec_cmd(int *o_ordercode, int *etm)
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
			/*cusnd(enter,1);
			RcvFrmN(); */
            /*连接空闲, 发送回车来保持连接*/
            retn = sndcmd(sndid, "\r\n< ", "\r\n", 2,
                    "\r\n< ", replystr, sizeof(replystr));
		}

		fflush(logfp);
		sleep(5);

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
		*o_ordercode = ordercode;

		fprintf(logfp,"--retn========%04d~\n",get_length(cmdackptr->retn,4));
		fprintf(logfp,"--order=%s|%04d|%s|%s|%s|%s|%s|%ld\n",
            cmdackptr->stream_id, ordercode, cmdackptr->phone_no,
            cmdackptr->imsi_no, cmdackptr->ss_info1, cmdackptr->ss_info2, cmdackptr->ss_info3,
            time(NULL)
            );

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
printf("beg==%d\n",beg);
printf("len==%d\n",len);
printf("para=%s~\n",para+beg);

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
			*dest='\r'; dest++;
			/*  *dest='\n'; dest++; */
			*dest=0x0;

            if( (dest = strstr(orderinfo, ":KI=")) != NULL && strstr(orderinfo, "ZMAC:IMSI=") != NULL)
            {
                /*对于加载KI的指令, nokia需要明文的, 所以先解密
                ZMAC:IMSI=$20000:KI=$30014$40018,A3V=1,A8V=1:;
                */
                char tmp[33];
                des_11(dest+4, tmp);
                memcpy(dest+4, tmp, 32);
            }
            if( (dest = strstr(orderinfo, ",OPC=")) != NULL && strstr(orderinfo, "ZMAC:IMSI=") != NULL)
            {
                /*对于加载OPC的指令, nokia需要明文的, 所以先解密
                ZMAC:IMSI=$20000:KI=$30014$40018,UV=99,OPC=$50032;
                */
                char tmp[33];
                des_11(dest+5, tmp);
                memcpy(dest+5, tmp, 32);
            }

gettimeofday(&tt1, NULL);
            retn = sndcmd(sndid, "\r\n< \08 ", orderinfo, strlen(orderinfo),
                    "_>\r\n< \08 ", replystr, sizeof(replystr));
gettimeofday(&tt2, NULL);
*etm = time_diff(tt1, tt2);
            if(retn <= 0){
                fprintf(logfp, "sndcmd failed, return:%d\n", retn);
                return -1;
            }


			retn=reply_parse();
			fprintf(logfp,"RETN:%04d\n",retn);

			if(retn==0 && ordercode>9000)
			{
				memset(cmdreqptr->info,' ',sizeof(cmdreqptr->info));
                /*memcpy(cmdreqptr->info, cmdackptr->queryinfo, QUERYINFOLEN); */
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
        struct timeval tt1, tt2;

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

	char order_fname[256],reply_fname[256],query_fname[256],cfginfo[1024],*p,*d;
	int line;
	int ordercode, etm;
	FILE *cfgfp;
	char cfgstr[256];
    char mgr_user[20], mgr_pswd[20], mgr_term[10];

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
	memset(dbuser,0x0,sizeof(dbuser));
	memset(dbpswd,0x0,sizeof(dbpswd));
	memset(dbname,0x0,sizeof(dbname));
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
			printf("\n\n	version = %s\n\n",VERSION);
			exit(0);
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
	sprintf(logfilename,"%s/%s%04d%02d%02d.%s.%s",logdir, port_type,
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

	/*fprintf(logfp,"ORDER INFO:\n");
	orderptr=orderhead.next;
	while(orderptr)
	{
		fprintf(logfp,"%06d = [%s]\n",orderptr->ordercode,orderptr->orderinfo);
		orderptr=orderptr->next;
	}
	fflush(logfp);*/

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

		/*fprintf(logfp,"%s",cfginfo); */

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

	/*disp_query_cfg(); */

    connid = tcp_connect(srvip, commport);
	if(connid < 0)
	{
		fprintf(logfp,"Incorrect IPADDR in mcfghead[%s]\n",srvip);
		fclose(logfp);
		exit(1);
	}

	memset(dbuser,0x0,sizeof(dbuser));
	memset(dbpswd,0x0,sizeof(dbpswd));
	memset(tname,0x0,sizeof(tname));

	if(login_server(connid,hlrcode,hlrport, mgr_user, mgr_pswd, mgr_term))
	{
		fprintf(logfp,"login_server() failed!\n");
		fclose(logfp);
		exit(1);
	}
	fprintf(logfp,"login_server(%s) success!\n",hlrcode);

	/*** CONNECT TO HLR ** */
    gsmid = tcp_connect(gsmip, gsmport);
    if(gsmid < 0){
        fprintf(logfp, "connect(%s:%d) failed, errno:%d\n", gsmip, gsmport, errno);
        fclose(logfp);
        exit(1);
    }
    sndid=gsmid;
    rcvid=gsmid;

	/*** LOGIN HLR ***/
    sprintf(ss, "%s\r\n", gsmuser);
    ret = sndcmd(sndid, "\nENTER USERNAME < \08 ", ss, strlen(ss),
            "\nENTER PASSWORD < \08 ", replystr, sizeof(replystr));
    if(ret <= 0){
        fprintf(logfp, "sndcmd username failed, return:%d\n", ret);
        fclose(logfp);
        return 2;
    }
    sprintf(ss, "%s\r\n", gsmpswd);
    ret = sndcmd(sndid, "\nENTER PASSWORD < \08 ", ss, strlen(ss),
            "\nMAIN LEVEL COMMAND <___>", replystr, sizeof(replystr));
    if(ret <= 0){
        fprintf(logfp, "sndcmd password failed, return:%d\n", ret);
        fclose(logfp);
        return 3;
    }

	fprintf(logfp,"LOGIN HLR,SUCCESS![%ld]\n",time(NULL));
	fflush(logfp);

    /*if(1) exit(0); */

	/************** GET AN ORDER AND DEAL ********************/
	waitnum=0;
	transid=2;
	cmdreqptr=(struct cmd_data_req *)tcpbuf.data;
	cmdreqptr->type=ONLY_GET;

	while(1)
	{
		if(send_req(connid,DECRYPT,TX_END,GETORDER,transid++,CMDREQLEN)<0)
		{
			fprintf(logfp,"send_req(%d) failed[%d]\n",connid,errno);
			break;
		}

		readlen=readnet(connid,(char *)&tcpbuf,8);
		if(readlen!=8)
		{
			fprintf(logfp,"readnet(%d,0,8)=%d failed\n",connid,readlen);
			break;
		}

		if(strncmp(tcpbuf.flag,MSGFLAG,4))
		{
			tcpbuf.msglen[0]=0;
			fprintf(logfp,"MSGFLAG is incorrect [%s]!\n",tcpbuf.flag);
			break;
		}

		msglen=get_length(tcpbuf.msglen,4);
		if(msglen<32)
		{
			fprintf(logfp,"MSGLEN is incorrect[%04d]\n",msglen);
			break;
		}

		readlen=readnet(connid,(char *)&tcpbuf+8,msglen-8);
		if(readlen!=msglen-8)
		{
			fprintf(logfp,"readnet(%d,8,%d)=%d failed!\n",connid,msglen-8,readlen);
			break;
		}
printf("RCV:%s~\n",(char *)&tcpbuf);

                ordercode = 0; etm = 0; 
		ret = exec_cmd( &ordercode, &etm);
                fprintf(logfp, "EXEC_CMD===========%d ordercode=%04d tm=%dms\n",ret, ordercode, etm);
		if(ret<0)
		{
			fprintf(logfp,"exec_cmd() failed!\n");
			break;
		}

		time(&t);
		tv=localtime(&t);

		if(today!=tv->tm_mday)
		{
			fclose(logfp);

			memset(logfilename,0x0,sizeof(logfilename));
			sprintf(logfilename,"%s/%s%04d%02d%02d.%s.%s",logdir, port_type, 
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

