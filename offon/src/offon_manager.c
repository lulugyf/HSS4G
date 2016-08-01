/*********************************************************
 *
 *  FileName:
 *      offon_manager.c
 *
 *  Version:
 *      1.00 2004.11.09
 *
 *  tabstop = 4
 *
 ********************************************************/

#include "offon.h"
#include "file_func.h"

#define __ORDER_SAVE__

/*是否使用数据库的标志, 0: 不使用  1: 使用
暂时只适用于 Insert sOprCmd 和 Update sOprCmd 两个操作
以文件方式替代这两个数据库操作, 然后另外起一个进程从文件中取记录进行数据库操作
可以减少复制过程中的时间
规则:
	采用小文件方式,
	每个文件超过100K产生新文件, 文件名规则 hbc.00000001, 数字累加
	使用的当前文件名记录在控制文件 hbc.ctl 中
	sequence 的产生在manager进程中进行, 初始值从当前数据文件中的最后一个insert记录中取stream+1
		然后在内存中累加, 各 HLR分开计算
	处理数据文件的进程在 hbc.ctl 文件中记录当前处理的文件名和在文件中的位置, 读取数据文件的方式为
		readline, 每完成一条记录 hbc.ctl
	hbc.ctl 的文件格式 [000000020000000100002312], 其中 0-7 byte 是产生数据文件进程记录的当前
		文件序号, 8-15 byte 是处理进程记录的当前处理文件序号, 16-31 byte 是处理进程记录的当前
		文件位置
	hbc.00000001文件格式:
		insert: 以字母 "I" 开始:
			根据 soprcmdhbc 表结构, 各字段间以 | 分隔, 时间格式: yyyymmddhh24miss
				可以为空的字段为空时, | 之间无字符
			 STREAM_ID                                 NOT NULL NUMBER(16)
			 COMMAND_ID                                NOT NULL NUMBER(16)
			 TERM_ID                                   NOT NULL CHAR(6)
			 MSISDN                                    NOT NULL CHAR(15)
			 IMSI_NO                                   NOT NULL CHAR(20)
			 OP_CODE                                   NOT NULL CHAR(4)
			 COMMAND_CODE                              NOT NULL CHAR(4)
			 CMD_STATUS                                NOT NULL CHAR(1)
			 PHONE_PRIORITY                            NOT NULL CHAR(1)
			 REQUEST_TIME                              NOT NULL DATE
			 SS_INFO1                                  NOT NULL CHAR(15)
			 SS_INFO2                                  NOT NULL CHAR(20)
			 SS_INFO3                                           VARCHAR2(120)
			 REPLY_CODE                                NOT NULL CHAR(4)
			 REPLY_TIME                                NOT NULL DATE
			 REPLY_INFO                                         VARCHAR2(200)
			 BAK_STATUS                                NOT NULL CHAR(1)
			 BAK_TIME                                  NOT NULL DATE
			 LOGIN_NO                                  NOT NULL CHAR(6)
		update: 以字母 "U" 开始
			UPDATE sOprCmd%s SET cmd_status=\'1\',reply_code=:v1,reply_time=sysdate,
				reply_info=:v2 WHERE stream_id=:v3",hlr);
				则格式为 STREAM_ID | REPLY_CODE | REPLY_INFO

*/
int usedb_flag = 0;
int max_queue_len = -1;

int queue_len; /*指令队列长度*/

extern int writenet();
extern int readnet();
extern int get_length();
extern int comp_int();

extern int usrdata_revise();
extern int usrdata_check();
extern int get_ki_data();
extern int usrdata_save_db();
extern int get_userinfo();
extern int get_cmdtoorder();
extern int get_orderreduce();
extern int get_orderresend();
extern int get_replyparse();
extern int get_queryparse();
extern int get_queryrevise();
extern int get_usrdatamgr();
extern int get_pheadhlr();
extern int get_opcodepri();
extern int get_commport();
extern int get_cmdwait();

char hlrcode[8];
FILE *logfp;

struct timeval tt1,tt2,ttt1,ttt2;

char mgr_flag;
int timeout;
char *procname;
struct comm_data		tcpbuf;
struct user_info		userhead,*userptr,*usertail;
struct comm_info		commhead,*commptr,*commtail,commfree;
struct cmdtoorder		cmdhead,*cmdptr,*cmdtail;
struct order_reduce		reducehead,*reduceptr,*reducetail;
struct order_resend		resendhead,*resendptr,*resendtail;
struct reply_parse		parsehead,*parseptr,*parsetail;
struct query_parse		queryhead,*queryptr,*querytail;
struct query_revise		revisehead,*reviseptr,*revisetail;
struct usrdata_mgr		mgrhead,*mgrptr,*mgrtail;
struct priorityidx		prihead,*priptr,*pritail,prifree;
struct phoneidx			pidxfree,*pidxptr,*pidxtail;

struct login_req		*loginreqptr;
struct login_ack		*loginackptr;

struct cmd_data_req		*cmdreqptr;
struct cmd_data_ack		*cmdackptr;

int pheadnum;
struct phonehead	pheadinfo[MAXPHEADNUM];
int opcodenum;
struct opcode_pri	opcodeinfo[MAXOPCODENUM],opcodehead,*opcodeptr;

struct phead_hlr	phlrhead,*phlrptr;
struct phonewait	*pwaitptr,*pwaittail;
struct cmdwait		cwaithead,cwaitfree,*cwaitptr,*cwaittail;
struct orderwait	owaitfree,*owaitptr,*owaittail,*owaittmp;

extern int GenDaemon();
extern int testdir();
extern int db_login();

struct
{
	time_t rectime;
	char rectype[20];
	char hlrcode[6];
	int rc4enq;
	int tm4enq;
	int rc4gto;
	int tm4gto;
	int rc4dlr;
	int tm4dlr;
} yy_time;


void usage(char *s)
{
	printf("%s\n\nUsage:%s -Hhlrcode [-Udbuser -Pdbpswd -Sdbname] [-Llogdir] [-Dusedb] [-Mmaxqueuelen] [-V]\n\n\n",s,procname);
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

void free_comm(struct comm_info *ptr)
{
	struct comm_info *tmp;

	close(ptr->next->sockid);

	tmp=ptr->next;
	ptr->next=tmp->next;

	tmp->next=commfree.next;
	commfree.next=tmp;

	return;
}

void print_cwait(const char *sig, FILE *fout, struct cmdwait *ptr)
{
	struct cmdwait *p1;
	struct orderwait *p2;
	p1 = ptr->next;
	fprintf(fout, "cmdlist: %s %s\n", sig, (p1==NULL ? "none":""));
	while(p1){
		fprintf(fout, "%ld %03d pri=%d %s[",
				p1->streamid, p1->cmdcode, p1->priority, p1->msisdn);
		p2 = p1->orderhead.next;
		while(p2){
			fprintf(fout, "%04d ", p2->ordercode);
			p2 = p2->next;
		}
		fprintf(fout, "]\n");
		p1 = p1->next;
	}
}

void print_pri(FILE *fout)
{
	struct priorityidx *p1;
	struct phoneidx *p2;
	p1 = prihead.next;

	while(p1)
	{
		fprintf(fout, "====pri %d,num=%d:[", p1->priority, p1->num);
		p2 = p1->phonehead.next;
		while(p2){
			fprintf(fout, " %d%04d", p2->phead, p2->ptail);
			p2 = p2->next;
		}
		fprintf(fout, "]\n");
		p1 = p1->next;
	}
}

int send_ack(int id,int encrypt,int finished,int srvtype,int transid,int length)
{
	char chlen[16];

	length+=32;
	memcpy(tcpbuf.flag,MSGFLAG,4);
	sprintf(chlen,"%04d",length);
	memcpy(tcpbuf.msglen,chlen,4);
	sprintf(tcpbuf.srvtype,"%08d",srvtype);
	sprintf(tcpbuf.transid,"%08d",transid);
	tcpbuf.finished=finished;
	tcpbuf.msgtype=ACK_TYPE;
	tcpbuf.encrypt=encrypt;
	memcpy(tcpbuf.reserve,"     ",5);

/*printf("SND:%s[%d]\n",(char *)&tcpbuf,length); */
	return writenet(id,(char *)&tcpbuf,length);
}

int general_ack(struct comm_info *ptr,int srv,int transid,int code,char *desc)
{
	struct general_ack *ack;

	if(code==3001)
		fprintf(logfp,"#");
	else
		fprintf(logfp,"GENERAL_ACK:%04d,%s\n",code,desc);

	ack=(struct general_ack *)tcpbuf.data;

	sprintf(ack->retn,"%04d",code);
	strcpy(ack->desc,desc);

	if(send_ack(ptr->next->sockid,DECRYPT,TX_END,srv,transid,sizeof(struct general_ack))<0)
	{
		fprintf(logfp,"send_ack() failed[%d]\n");
		free_comm(ptr);
	}

	return 0;
}

struct phoneidx * get_pidxnode()
{
	struct phoneidx *ptr;

	if(pidxfree.next)
	{
		ptr=pidxfree.next;
		pidxfree.next=ptr->next;
	}
	else
	{
		ptr=(struct phoneidx *)malloc(sizeof(struct phoneidx));
		if(ptr==NULL)
		{
			fprintf(logfp,"malloc(%d) failed[%d]\n",sizeof(struct phoneidx));
			fclose(logfp);
			exit(1);
		}
		/*fprintf(logfp,"malloc(%d) in get_pidxnode()\n",sizeof(struct phoneidx)); */
	}

	memset(ptr,0x0,sizeof(struct phoneidx));

	return ptr;
}

struct priorityidx * get_prinode()
{
	struct priorityidx *ptr;

	if(prifree.next)
	{
		ptr=prifree.next;
		prifree.next=ptr->next;
	}
	else
	{
		ptr=(struct priorityidx *)malloc(sizeof(struct priorityidx));
		if(ptr==NULL)
		{
			fprintf(logfp,"malloc(%d) failed[%d]\n",sizeof(struct priorityidx));
			fclose(logfp);
			exit(1);
		}
		/*fprintf(logfp,"malloc(%d) in get_prinode()\n",sizeof(struct priorityidx)); */
	}
	memset(ptr,0x0,sizeof(struct priorityidx));
	return ptr;
}

struct orderwait * get_owaitnode()
{
	if(owaitfree.next)
	{
		owaittail=owaitfree.next;
		owaitfree.next=owaitfree.next->next;
	}
	else
	{
		owaittail=(struct orderwait *)malloc(sizeof(struct orderwait));
		if(owaittail==NULL)
		{
			fprintf(logfp,"malloc(%d) failed[%d]\n",sizeof(struct orderwait));
			fclose(logfp);
			exit(1);
		}
		/*fprintf(logfp,"malloc(%d) in get_owaitnode()\n",sizeof(struct orderwait)); */
	}

	memset(owaittail,0x0,sizeof(struct orderwait));

	return owaittail;
}

struct cmdwait * get_cwaitnode(struct phonewait *pwait)
{
	cwaittail=&pwait->cmdhead;
	while(cwaittail->next)
		cwaittail=cwaittail->next;

	if(cwaitfree.next)
	{
		cwaittail->next=cwaitfree.next;
		cwaitfree.next=cwaitfree.next->next;
	}
	else
	{
		cwaittail->next=(struct cmdwait *)malloc(sizeof(struct cmdwait));
		if(cwaittail==NULL)
		{
			fprintf(logfp,"malloc(%d) failed[%d]\n",sizeof(struct cmdwait));
			fclose(logfp);
			exit(1);
		}
		fprintf(logfp,"malloc(%d) in get_cwaitnode()\n",sizeof(struct cmdwait));
	}

	memset(cwaittail->next,0x0,sizeof(struct cmdwait));

	return cwaittail->next;
}

struct phonewait * get_pwaitnode(char *phone,int ph,int pt,int type)
{
	int plen,phead,ptail;
	int beg,end,cur,val;

	if(phone==NULL)
	{
		phead=ph;
		ptail=pt;
	}
	else
	{
		plen=strlen(phone);
		if(plen>4)
		{
			phead=get_length(phone,plen-4);
			ptail=get_length(phone+plen-4,4);
		}
		else
		{
			phead=0;
			ptail=atoi(phone);
		}
	}

	beg=0;
	end=pheadnum;
	cur=(end-beg)/2 + beg;

	for(;;)
	{
		val=pheadinfo[cur].phead-phead;
		if(val==0)
		{
			if(type==1)
				return pheadinfo[cur].ptail[ptail];

			if(pheadinfo[cur].ptail[ptail]==NULL)
			{
				pwaittail=(struct phonewait *)malloc(sizeof(struct phonewait));
				if(pwaittail==NULL)
				{
					fprintf(logfp,"malloc(%d) failed[%d]\n",sizeof(struct phonewait));
					fclose(logfp);
					exit(1);
				}
				/*fprintf(logfp,"malloc(%d) in get_pwaitnode()\n",sizeof(struct phonewait)); */

				memset(pwaittail,0x0,sizeof(struct phonewait));
				pheadinfo[cur].ptail[ptail]=pwaittail;
			}

			return pheadinfo[cur].ptail[ptail];
		}
		else if(val>0)
		{
			if(end==cur)
				break;

			end=cur;
			cur=(end-beg)/2 + beg;
		}
		else
		{
			if(beg==cur)
				break;

			beg=cur;
			cur=(end-beg)/2 + beg;
		}
	}

	return NULL;
}

int get_priority(char *code)
{
	int cur,beg,end,val;

	if(code==NULL)
		return NULL;

	beg=0;
	end=opcodenum;
	cur=(end-beg)/2 + beg;

	for(;;)
	{
		val=strcmp(opcodeinfo[cur].op_code,code);
		if(val==0)
		{
			return opcodeinfo[cur].priority;
		}
		else if(val>0)
		{
			if(cur==end)
				break;

			end=cur;
			cur=(end-beg)/2 + beg;
		}
		else
		{
			if(cur==beg)
				break;

			beg=cur;
			cur=(end-beg)/2 + beg;
		}
	}

	return 3000;
}

int compare_order(struct orderwait *optr,struct orderwait *otmp)
{
	/*fprintf(stderr, "into compare_order..\n");*/
	if(optr==otmp)
		return 0;

	if(optr->status==REDUCE_CMD || otmp->status==REDUCE_CMD){
        fprintf(logfp, "order reduced %d, return\n", otmp->ordercode);
		return 0;
    }

	reduceptr=reducehead.next;

	while(reduceptr)
	{
		if(optr->ordercode==reduceptr->old_order_code && otmp->ordercode==reduceptr->new_order_code)
		{
			switch (reduceptr->operate)
			{
			case '1':
				optr->status=REDUCE_CMD;
				fprintf(logfp, "reduce old %d by %d\n", optr->ordercode, otmp->ordercode);
				break;
			case '2':
				otmp->status=REDUCE_CMD;
				fprintf(logfp, "reduce new %d by %d\n", otmp->ordercode, optr->ordercode);
				break;
			case '3':
				optr->status=REDUCE_CMD;
				otmp->status=REDUCE_CMD;
                fprintf(logfp, "reduce both old:%d new:%d\n",
                    reduceptr->old_order_code, reduceptr->new_order_code);
				break;
			default:
				break;
			}
		}

		reduceptr=reduceptr->next;
	}

	return 0;
}

struct cmdtoorder *get_ordernode(int code)
{
	cmdtail=cmdhead.next;
	while(cmdtail)
	{
		if(cmdtail->cmdcode==code)
			return cmdtail;

		cmdtail=cmdtail->next;
	}

	return NULL;
}

void queryinfo_parse(struct cmdwait *ptr,int cmd,int code,char *info)
{
	char *src,*dest;
	int i,len;

	queryptr=queryhead.next;
	while(queryptr)
	{
		if(comp_int(queryptr->cmd_code,cmd) && comp_int(queryptr->order_code,code))
		{
			switch(queryptr->dest_station)
			{
			case 1:
				if(queryptr->dest_beg+queryptr->data_len>MSISDNLEN)
					dest=NULL;
				else
					dest=ptr->next->msisdn;
				break;
			case 2:
				if(queryptr->dest_beg+queryptr->data_len>IMSILEN)
					dest=NULL;
				else
					dest=ptr->next->imsi;
				break;
			case 3:
				if(queryptr->dest_beg+queryptr->data_len>SSINFO1LEN)
					dest=NULL;
				else
					dest=ptr->next->ss_info1;
				break;
			case 4:
				if(queryptr->dest_beg+queryptr->data_len>SSINFO2LEN)
					dest=NULL;
				else
					dest=ptr->next->ss_info2;
				break;
			case 5:
				if(queryptr->dest_beg+queryptr->data_len>SSINFO3LEN)
					dest=NULL;
				else
					dest=ptr->next->ss_info3;
				break;
			case 6:
				if(queryptr->dest_beg+queryptr->data_len>QUERYINFOLEN)
					dest=NULL;
				else
					dest=ptr->next->queryinfo;
				break;
			defalut:
				dest=NULL;
				break;
			}

			if(dest)
			{
				memcpy(dest+queryptr->dest_beg,info+queryptr->src_beg,
					queryptr->data_len);

				/*无主停机如果查询失败没有结果, 导致放到停机指令的imsi号位置为空格, 使进程挂死 by guanyf 2006-12-21*/
				if(memcmp(dest+queryptr->dest_beg, " ", 1) == 0)
					(dest+queryptr->dest_beg)[0] = '1';
			}
		}

		queryptr=queryptr->next;
	}

	return;
}

int tt[10];

/*** 应答信息处理 ***/
int reply_info_deal(struct comm_info *ptr,struct cmd_data_req *req)
{
	int retncode,parsecode,cmdcode,ordercode,revise,cmdtype;

	/*** 取得该号码的命令存储地址 ***/
	pwaitptr=get_pwaitnode(req->phone_no,0,0,1);
	if(pwaitptr==NULL)
	{
		fprintf(logfp,"can't find addr of %s!\n",req->phone_no);
		return 3002;
	}

	cwaitptr=&pwaitptr->cmdhead;
	if(cwaitptr->next->streamid!=atol(req->stream_id) || cwaitptr->next->orderhead.next->ordercode!=get_length(req->ordercode,4))
	{
		fprintf(logfp,"can't find order of %s[%s:%ld][%d:%d]\n",
			req->phone_no,req->stream_id,cwaitptr->next->streamid,
			cwaitptr->next->orderhead.next->ordercode,
			get_length(req->ordercode));
		return 3003;
	}

	/*** 释放号码状态 ***/
	pwaitptr->status=0;

	/*** 分析命令处理结果 ***/
	revise=0;
	cmdcode=cwaitptr->next->cmdcode;
	ordercode=get_length(req->ordercode,4);
	retncode=get_length(req->retn,4);
	parsecode=retncode;
	owaitptr=&cwaitptr->next->orderhead;

	/*** 查询结果分析 ***/
	if(ordercode>=9000 && retncode==0000)
	{
		queryinfo_parse(cwaitptr,cmdcode,ordercode,req->info);
	}

	/*** 用户资料校验 ***/
	if(ordercode>=9000 && (mgr_flag=='y' || mgr_flag=='Y'))
	{
		if(usrdata_revise(revisehead.next,cwaitptr->next,retncode,req->info,hlrcode)<0)
		{
			fprintf(logfp,"usrdata_revise() failed!\n");
			fclose(logfp);
			exit(1);
		}
	}

	if(retncode && (owaitptr->next->status==GENERAL_CMD
			|| owaitptr->next->status==REDUCE_CMD)
	)
	{
		parseptr=parsehead.next;
		while(parseptr)
		{
			if(comp_int(parseptr->cmd_code,cmdcode) && comp_int(parseptr->order_code,ordercode) && parseptr->reply_code==retncode)
			{
				if(parseptr->revise=='R' || parseptr->revise=='r')
				{
					return 3009;
				}
				else if(parseptr->revise=='Y' || parseptr->revise=='y')
				{
					owaitptr->next->status = (parseptr->revise=='Y') ? REDUCE_CMD : REVISE_CMD;
					/*owaitptr->next->status = REVISE_CMD;*/

					if(revise==0){
						order_save_file(cwaitptr->next,retncode,parsecode);
					}
					revise++;

					/*** 向队列中增加错误校正命令 ***/
					owaittmp=get_owaitnode();
					owaittmp->ordercode=parseptr->dest_order;
					owaittmp->type=parseptr->order_type;
					owaittmp->serial=(parseptr->serial+20)%100;
					owaittmp->status=REVISE_CMD;

					owaittmp->next=owaitptr->next;
					owaitptr->next=owaittmp;

                    while(owaittmp)
                    {
                        fprintf(logfp,"owaittmp->ordercode====%d\n",owaittmp->ordercode);
                        fprintf(logfp,"owaittmp->type=========%d\n",owaittmp->type);
                        fprintf(logfp,"owaittmp->serial=======%d\n",owaittmp->serial);
                        fprintf(logfp,"owaittmp->status=======%d\n\n\n",owaittmp->status);
                        owaittmp=owaittmp->next;
                    }
                    fprintf(logfp,"-----------------\n");
                    fflush(logfp);
				}
				else
				{
					parsecode=parseptr->parse_result;
					break;
				}
			}

			parseptr=parseptr->next;
		}
	}

	if(revise)
	{
		return 0000;
	}

	/*** 保存指令处理结果到数据库 ***/
	order_save_file(cwaitptr->next,retncode,parsecode);

	/*** 释放指令节点 ***/
	cmdtype=owaitptr->next->status;
	owaittail=owaitptr->next;
	owaitptr->next=owaittail->next;
	owaittail->next=owaitfree.next;
	owaitfree.next=owaittail;

	/* 释放后续指令节点 */
	if(parsecode && cmdtype!=REVISE_CMD)
	{
		while(owaitptr->next)
		{
			fprintf(logfp,"free one order[%d]\n",owaitptr->next->ordercode);
			owaittail=owaitptr->next;
			owaitptr->next=owaittail->next;
			owaittail->next=owaitfree.next;
			owaitfree.next=owaittail;
		}
	}
	fflush(logfp);

	if(owaitptr->next==NULL)
	{
		char tmp[30];
		/*** 保存业务命令结果 ***/
        reply_save_file(cwaitptr->next,parsecode, cwaitptr->next->queryinfo);

		queue_len --;

		/*** 释放业务命令节点 ***/
		cwaittail=cwaitptr->next;
		cwaitptr->next=cwaittail->next;
		cwaittail->next=cwaitfree.next;
		cwaitfree.next=cwaittail;

		/*
		sprintf(tmp, "rpl[%ld %s %04d qlen:%d]",
        		cwaitptr->next->streamid, cwaitptr->next->msisdn, parsecode, queue_len);
		print_cwait(tmp, logfp, cwaitptr);
		print_pri(logfp); */

		/*** 不释放号码节点 ***/
	}

	return 0000;
}

/*** 从队列中取得一条待发送指令 ***/
int get_one_order(struct comm_info *ptr,time_t cur_t)
{
	struct phoneidx *pidx1, *pidx2;
	struct timeval t1, t2;
	char tmp[40];
	int loop_limit = 5000;
	int loop_i = 0;

	switch(ptr->termtype)
	{
	case TERM_HLR:
	case TERM_AUC:
	case TERM_DATACENTER:
		pritail = &prihead;
		while(pritail->next)
		{
			priptr=pritail->next;
			pidx1 = &priptr->phonehead;
			while(pidx1->next)
			{ /*** 取得该号码的命令存储地址 ***/
				pidx2 = pidx1->next;
				loop_i ++;
				if(loop_i > loop_limit) {
					fprintf(logfp, "get_one_order too many loop count %d\n", loop_i);
					return 0;
				}
				pwaitptr=get_pwaitnode(NULL,pidx2->phead,
						pidx2->ptail, 1);
				if(pwaitptr==NULL || pwaitptr->cmdhead.next==NULL)
				{ /*** 该号码没有待发送命令 ***/
					priptr->num--;
					if(pidx2->next == NULL && priptr->num > 0){
						//如果是尾部, 但还有其它节点, 把尾指针指到上一个节点
						priptr->phonetail.next = pidx1;
					}
					pidx1->next=pidx2->next;

					pidx2->next = pidxfree.next;
					pidxfree.next = pidx2;

					// 如果删除的是尾指针, 而链表还有其它节点, 则导致尾指针悬空

					/*清空 尾指针*/
					if(priptr->num == 0){
						fprintf(logfp, "clear pri node %d\n", priptr->priority);
						priptr->phonetail.next = NULL;
					}
					continue;
				}

				if(pwaitptr->status>0 && ptr->termtype!=pwaitptr->type)
				{ /*** 该号码处于忙状态 ***/
					pidx1=pidx1->next;
					continue;
				}

				if(cur_t-pwaitptr->status<120)
				{ /*** 该号码处于忙状态 ***/
					pidx1=pidx1->next;
					continue;
				}

				cwaitptr=&pwaitptr->cmdhead;
				if(ptr->termtype != cwaitptr->next->orderhead.next->type)
				{ /*** 指令类型不匹配 ***/
					pidx1=pidx1->next;
					continue;
				}

				if(cwaitptr->next->orderhead.next->status==REDUCE_CMD)
				{
					/*** 剔重指令 ***/
					fprintf(logfp,"REDUCE:%ld	%s	%d\n",
						cwaitptr->streamid,cwaitptr->next->msisdn,
						cwaitptr->next->orderhead.next->ordercode);

                    order_save_file(cwaitptr->next,1000,0000);

					/*** 释放指令节点 ***/
					owaitptr=&cwaitptr->next->orderhead;
					owaittail=owaitptr->next;
					owaitptr->next=owaittail->next;
					owaittail->next=owaitfree.next;
					owaitfree.next=owaittail;

					if(owaitptr->next==NULL)
					{
						/*** 保存业务命令结果 ***/
						reply_save_file(cwaitptr->next,0000,"REDUCE");
						queue_len --;

						/*** 释放业务命令节点 ***/
						cwaittail=cwaitptr->next;
						cwaitptr->next=cwaittail->next;
						cwaittail->next=cwaitfree.next;
						cwaitfree.next=cwaittail;
						/*** 不释放号码节点 ***/
					}
					continue;
				}

				pwaitptr->status=cur_t;
				pwaitptr->type=ptr->termtype;

				cwaitptr=cwaitptr->next;
				cmdackptr=(struct cmd_data_ack *)tcpbuf.data;
				memcpy(cmdackptr->retn,"0001",4);
				sprintf(cmdackptr->stream_id,"%ld",cwaitptr->streamid);
				sprintf(cmdackptr->ordercode,"%04d",
					cwaitptr->orderhead.next->ordercode);
				memcpy(cmdackptr->phone_no,cwaitptr->msisdn,MSISDNLEN);
				memcpy(cmdackptr->imsi_no,cwaitptr->imsi,IMSILEN);
				memcpy(cmdackptr->ss_info1,cwaitptr->ss_info1,SSINFO1LEN);
				memcpy(cmdackptr->ss_info2,cwaitptr->ss_info2,SSINFO2LEN);
				memcpy(cmdackptr->ss_info3,cwaitptr->ss_info3,SSINFO3LEN);
                /*memcpy(cmdackptr->queryinfo,cwaitptr->queryinfo,QUERYINFOLEN); */

				/*sprintf(tmp, "gto[%ld %s %s %d]",
						cwaitptr->streamid, cwaitptr->cmdid, cwaitptr->msisdn, cwaitptr->cmdcode);
				print_cwait(tmp, logfp, pwaitptr->cmdhead.next); */

				return 1;
			}

			if(priptr->phonehead.next==NULL)
			{ /*** 该优先级下没有任何号码 */

				pritail->next=priptr->next;
				priptr->next=prifree.next;
				prifree.next=priptr;
			}
			else
			{
				pritail=pritail->next;
			}
		}

		return 0;
	default:
		fprintf(logfp,"UNKNOW TERM TYPE[%d]\n",ptr->termtype);
		return 0;
	}
}

/*读取数据中的相关配置*/
int init_cfg()
{
 	/* INIT USER INFO */
	memset(&userhead,0x0,sizeof(userhead));
	if(get_userinfo(hlrcode,&userhead))
	{
		fprintf(logfp,"get_userinfo() failed!\n");
		fclose(logfp);
		exit(1);
	}

	fprintf(logfp,"user info:\n");
	userptr=userhead.next;
	while(userptr)
	{
		fprintf(logfp,"%s[%d]	[%d]	%s	%c	%c	%c	%s[%d]\n",
			userptr->user,strlen(userptr->user),
			strlen(userptr->pswd),userptr->hlrcode,userptr->hlrport,
			userptr->type,userptr->encrypt,userptr->termid,
			strlen(userptr->termid));

		userptr=userptr->next;
	}

	/* INIT COMM INFO */
	memset(&commhead,0x0,sizeof(commhead));
	memset(&commfree,0x0,sizeof(commfree));
	commptr=NULL;
	commtail=NULL;

	/* INIT CMD TO ORDER */
	memset(&cmdhead,0x0,sizeof(cmdhead));
	cmdptr=NULL;
	cmdtail=NULL;

	if(get_cmdtoorder(hlrcode,&cmdhead))
	{
		fprintf(logfp,"get_cmdtoorder(%s) failed!\n",hlrcode);
		fclose(logfp);
		exit(1);
	}

	fprintf(logfp,"cmdtoorder:\n");
	cmdptr=cmdhead.next;
	while(cmdptr)
	{
		fprintf(logfp,"%d		%d		%d		%d\n",cmdptr->cmdcode,cmdptr->priority,
			cmdptr->ordercode,cmdptr->serial);

		cmdptr=cmdptr->next;
	}

	/* INIT ORDER REDUCE */
	memset(&reducehead,0x0,sizeof(reducehead));
	if(get_orderreduce(hlrcode,&reducehead))
	{
		fprintf(logfp,"get_orderreduce(%s) failed!\n",hlrcode);
		fclose(logfp);
		exit(1);
	}

	fprintf(logfp,"orderreduce:\n");
	reduceptr=reducehead.next;
	while(reduceptr)
	{
		fprintf(logfp,"%d		%d		%c\n",reduceptr->old_order_code,
			reduceptr->new_order_code,reduceptr->operate);

		reduceptr=reduceptr->next;
	}

	/* INIT ORDER RESEND */
	memset(&resendhead,0x0,sizeof(resendhead));
	if(get_orderresend(hlrcode,&resendhead))
	{
		fprintf(logfp,"get_orderresend(%s) failed!\n",hlrcode);
		fclose(logfp);
		exit(1);
	}

	fprintf(logfp,"orderresend:\n");
	resendptr=resendhead.next;
	while(resendptr)
	{
		fprintf(logfp,"%d	%s\n",resendptr->order_code,resendptr->func_code);

		resendptr=resendptr->next;
	}

	/* INIT ORDER REPLY PARSE */
	memset(&parsehead,0x0,sizeof(parsehead));
	if(get_replyparse(hlrcode,&parsehead))
	{
		fprintf(logfp,"get_replyparse(%s) failed!\n",hlrcode);
		fclose(logfp);
		exit(1);
	}

	fprintf(logfp,"replyparse:\n");
	parseptr=parsehead.next;
	while(parseptr)
	{
		fprintf(logfp,"%s	%d	%d	%d	%d	%d	%d	%c	%d\n",
			parseptr->hlrcode,parseptr->cmd_code,
			parseptr->order_code,parseptr->reply_code,
			parseptr->parse_result,parseptr->revise,parseptr->dest_order,
			parseptr->order_type,parseptr->serial);

		parseptr=parseptr->next;
	}

	/* INIT ORDER QUERY PARSE */
	memset(&queryhead,0x0,sizeof(queryhead));
	if(get_queryparse(hlrcode,&queryhead))
	{
		fprintf(logfp,"get_queryparse(%s) failed!\n",hlrcode);
		fclose(logfp);
		exit(1);
	}

	fprintf(logfp,"queryparse:\n");
	queryptr=queryhead.next;
	while(queryptr)
	{
		fprintf(logfp,"%s	%d	%d	%d	%d	%d\n",
			queryptr->hlrcode,queryptr->order_code,queryptr->src_beg,
			queryptr->data_len,queryptr->dest_station,queryptr->dest_beg);

		queryptr=queryptr->next;
	}

	/* INIT QUERY REVISE */
	memset(&revisehead,0x0,sizeof(revisehead));
	if(get_queryrevise(hlrcode,&revisehead))
	{
		fprintf(logfp,"get_queryrevise(%s) failed!\n",hlrcode);
		fclose(logfp);
		exit(1);
	}

	fprintf(logfp,"queryrevise:\n");
	reviseptr=revisehead.next;
	while(reviseptr)
	{
		fprintf(logfp,"%d	%d	%04d	%s	%c	%c	%d	%c	%d	%d\n",
			reviseptr->cmd_code,reviseptr->order_code,
			reviseptr->reply_code,reviseptr->func_code,
			reviseptr->status_flag,reviseptr->info_flag,
			reviseptr->status_addr,reviseptr->status_null,
			reviseptr->info_beg,reviseptr->info_len);

		reviseptr=reviseptr->next;
	}

	/* INIT USR DATA MGR */
	memset(&mgrhead,0x0,sizeof(mgrhead));
	if(get_usrdatamgr(hlrcode,&mgrhead))
	{
		fprintf(logfp,"get_usrdatamgr(%s) failed!\n",hlrcode);
		fclose(logfp);
		exit(1);
	}

	fprintf(logfp,"usrdatamgr:\n");
	mgrptr=mgrhead.next;
	while(mgrptr)
	{
		fprintf(logfp,"%d	%s	%c	%s\n",
			mgrptr->order_code,mgrptr->func_code,
			mgrptr->func_status,mgrptr->ss_info);

		mgrptr=mgrptr->next;
	}

	/* INIT PHONE TO HLR */
	memset(&phlrhead,0x0,sizeof(phlrhead));
	if(get_pheadhlr(hlrcode,&phlrhead))
	{
		fprintf(logfp,"get_pheadhlr() failed!\n");
		fclose(logfp);
		exit(1);
	}

	pheadnum=0;
	memset(&pheadinfo,0x0,sizeof(pheadinfo));

	fprintf(logfp,"pheadhlr:\n");
	while(phlrhead.next)
	{
		phlrptr=phlrhead.next;
		phlrhead.next=phlrptr->next;

		/*fprintf(logfp,"hlr=%s	phead=%d\n",phlrptr->hlrcode,phlrptr->phead); */
		if(strcmp(hlrcode,phlrptr->hlrcode)==0 && pheadnum<MAXPHEADNUM)
		{
			strcpy(pheadinfo[pheadnum].hlrcode,hlrcode);
			pheadinfo[pheadnum].phead=phlrptr->phead;

			pheadnum++;
		}
		free(phlrptr);
	}
	fflush(logfp);

	/* INIT OP_CODE TO PRIORITY */
	memset(&opcodehead,0x0,sizeof(opcodehead));
	if(get_opcodepri(&opcodehead))
	{
		fprintf(logfp,"get_opcodepri() failed!\n");
		fclose(logfp);
		exit(1);
	}

	opcodenum=0;
	memset(&opcodeinfo,0x0,sizeof(opcodeinfo));

	fprintf(logfp,"opcode_priority:\n");
	while(opcodehead.next)
	{
		opcodeptr=opcodehead.next;
		opcodehead.next=opcodeptr->next;

		fprintf(logfp,"opcode==%s	priority==%d\n",
			opcodeptr->op_code,opcodeptr->priority);
		if(opcodenum<MAXOPCODENUM)
		{
			strcpy(opcodeinfo[opcodenum].op_code,opcodeptr->op_code);
			opcodeinfo[opcodenum].priority=opcodeptr->priority;

			opcodenum++;
		}
		free(opcodeptr);
	}
	fflush(logfp);
    return 0;
}


/*向内存队列添加一条 CMD */
int q_add_one_cmd(struct cmdwait *cmd, struct cmdtoorder *cmdptr, struct phonewait *pwaitptr)
{
    struct cmdwait *cwaitptr;
    struct phoneidx *pi2;

    if(cmdptr == NULL){
		/* 查询对应的指令 */
		cmdptr = get_ordernode(cmd->cmdcode);
		if(cmdptr==NULL)
		{
			fprintf(logfp,"get_ordernode(%d) failed!\n",
				cmd->cmdcode);
			reply_save_file(cmd, 1002, "UNKNOW COMMAND");
			cmd->next=cwaitfree.next;
			cwaitfree.next=cmd;
			return 1002;
		}

		/* 查询号码位置 */
		pwaitptr = get_pwaitnode(cmd->msisdn,0,0,0);
		if(pwaitptr==NULL)
		{
			fprintf(logfp,"get_pwaitnode(%s,0,0,0) failed!\n",
					cmd->msisdn);
			reply_save_file(cmd,1001,"UNKNOW PHONE_HEAD");
			cmd->next=cwaitfree.next;
			cwaitfree.next=cmd;
			return 1001;
		}
		cwaittail=&pwaitptr->cmdhead;
		while(cwaittail->next)
			cwaittail=cwaittail->next;
    }

    /* 保存命令到命令队列 */
    cwaittail->next=cmd;
    cmd->next=NULL;

    cmd->priority=get_priority(cmd->opcode)+(cmd->priority-'0')*1000+cmdptr->priority;

    /* 保存指令到指令队列 */
    owaittmp=NULL;
    while(cmdptr)
    {
        if(cmdptr->cmdcode!=cmd->cmdcode)
            break;

        owaittmp=get_owaitnode();
        owaittmp->ordercode=cmdptr->ordercode;
        owaittmp->type=cmdptr->type;
        owaittmp->serial=cmdptr->serial;
        owaittmp->status=GENERAL_CMD;

        /* 保存指令到指令队列 */
        owaittmp->next=cmd->orderhead.next;
        cmd->orderhead.next=owaittmp;

        /* 指令剔重 */
        cwaittail=pwaitptr->cmdhead.next;
        while(cwaittail)
        {
            owaitptr=cwaittail->orderhead.next;
            while(owaitptr)
            {
                compare_order(owaitptr,owaittmp);
                owaitptr=owaitptr->next;
            }
            cwaittail=cwaittail->next;
        }
        cmdptr=cmdptr->next;
    }

    /* 保存号码到优先级队列 */
    priptr=&prihead;
    while(priptr->next)
    {
        if(priptr->next->priority<=cmd->priority)
            break;
        else
            priptr=priptr->next;
    }

    if(priptr->next==NULL || priptr->next->priority!=cmd->priority)
    {
        pritail=get_prinode();
        pritail->next=priptr->next;
        priptr->next=pritail;
        priptr->next->priority=cmd->priority;
    }

    /*%%%%%%%%%% 重点, 同一个优先级的节点太多, 每次找尾巴就很痛苦
     * 调整方式: 在结构中增加尾指针
     *  */
    pi2 = &priptr->next->phonetail;

    if(priptr->next->num == 0){
    	priptr->next->phonehead.next = pi2->next = get_pidxnode();
    }else{
    	pi2->next->next = get_pidxnode();
    	pi2->next = pi2->next->next;
    }

    pi2->next->phead=get_length(cmd->msisdn,7);
    pi2->next->ptail=get_length(cmd->msisdn+7,4);
    priptr->next->num++;
    queue_len ++;

    return 0;
}

void yy_time_log(time_t t)
{
	if(t-yy_time.rectime >= 300){
		file_log("perf",
				"rectime=%ld,rectype=mgr,hlrcode=%s,rc4enq=%d,"
				"tm4enq=%d,rc4gto=%d,tm4gto=%d,rc4dlr=%d,tm4dlr=%d",
				t, yy_time.hlrcode, yy_time.rc4enq,
				yy_time.tm4enq/1000, yy_time.rc4gto, yy_time.tm4gto/1000, yy_time.rc4dlr, yy_time.tm4dlr/1000);
		yy_time.rc4enq = 0;
		yy_time.tm4enq = 0;
		yy_time.rc4gto = 0;
		yy_time.tm4gto = 0;
		yy_time.rc4dlr = 0;
		yy_time.tm4dlr = 0;
		yy_time.rectime = t;
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
		sprintf(fname,"%s/mgr%04d%02d%02d.%s",
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
	char inputstr[2048];
	int i,status,ret;
	char dbuser[32],dbpswd[32],dbname[32];
	char loginuser[16],loginpswd[16];
	char logfilename[256],logdir[256];
	time_t t;
	struct tm *tv;
	int today;
	char fname[256];
	FILE *qryfp;
	FILE *cfgfp;
	char cfgstr[256];
	char ackinfo[256];

	int listenid,connid,comm_port,err_num;
	unsigned int clilen;
	int optinfo,optlen;
	struct sockaddr_in srv_addr,cli_addr;
	struct timeval tval, t1, t2;
	fd_set rset;
	int fdret,maxid,waitnum;

	int readlen,msglen,tcplen,bodylen;
	int srvtype,transid;

	long	stream_id;
	long	command_id;
	int		cmd_code;
	int bk_flag, er_flag = 0;

	struct op_data_req		*srvcode0001req;
	struct op_data_ack		*srvcode0001ack;

	struct query0001_req	*qry01req;
	struct query0001_ack	*qry01ack;
	struct cmdwait *wait11, *wait12;
	int x, y;

	procname=argv[0];
	logfp=NULL;

	queue_len = 0;

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
	memset(dbuser,0x0,sizeof(dbuser));
	memset(dbpswd,0x0,sizeof(dbpswd));
	memset(dbname,0x0,sizeof(dbname));
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
		case 'L':
			strncpy(logdir,argv[argc]+2,240);
			break;
		case 'D':
			usedb_flag = argv[argc][2] - '0';
			break;
		case 'M':
			max_queue_len = atoi(argv[argc]+2);
			break;
		case 'V':
			printf("\n\n	version = %s\n\n",VERSION);
			exit(0);
		default:
			usage("Unknow Parameter!");
			break;
		}
	}

	printf("hlrcode===%s~\n",hlrcode);
	if(strlen(hlrcode)!=3)
		usage("The length of hlrcode is 3 character");

	if(strlen(logdir)<1)
		strcpy(logdir,getenv("LOGDIR"));

	if(testdir(logdir))
		usage("Incorrect logdir!");

	time(&t);
	logfp = open_logfp(t, NULL, hlrcode);

	/*检查双通道主备状态, 如果是备用状态, 则等待并定期重新检查
    dul_find(hlrcode);
    i = 0;
    while( 1 ){
        if(local_item->status == '2'){
            fprintf(logfp, "b");
            if(i > 79){
                fprintf(logfp, "\n");
                i = 0;
            }
            fflush(logfp);
            i++;
            sleep(1);
        }else{
            break;
        }
    }*/

	/*使用文件方式处理 sOprCmd 表的操作*/
	if(usedb_flag == 0){
		set_hlrcode(hlrcode);
		while(check_indb() != 0){/*检查文件入库操作是否完成*/
			fprintf(logfp, "file to db not complete, sleep 5\n");
			fflush(logfp);
			sleep(2);
		}
	}
	tv = localtime(&t);
	fprintf(logfp,"INIT %04d/%02d/%02d %02d:%02d:%02d\n",
		tv->tm_year+1900,tv->tm_mon+1,tv->tm_mday,
		tv->tm_hour,tv->tm_min,tv->tm_sec);
	fprintf(logfp,"%s[%d]\n",inputstr,argc);
	fflush(logfp);

	today=tv->tm_mday;



	/*** 登陆数据库 ***/
    i = logindb("gdblogin.cfg");
	if(i != 0)
	{
		fprintf(logfp,"db_login(%s) failed!\n","dgblogin.cfg");
		fclose(logfp);
		exit(1);
	}
	fprintf(logfp,"db_login() success!\n");
	fflush(logfp);

    /*初始化数据库配置*/
    init_cfg();

	memset(&cwaitfree,0x0,sizeof(cwaitfree));
	memset(&owaitfree,0x0,sizeof(owaitfree));
	memset(&prihead,0x0,sizeof(prihead));
	memset(&prifree,0x0,sizeof(prifree));
	memset(&pidxfree,0x0,sizeof(pidxfree));

	memset(&yy_time, 0, sizeof(yy_time));
	strcpy(yy_time.hlrcode, hlrcode);
	yy_time.rectime = time(NULL);

	/*** 恢复数据库中没有执行的命令 ***/
	gettimeofday(&t1, NULL);
	fprintf(logfp,"RESUME CMD,waiting......%ld \n", time(0));
	if(get_cmdwait(hlrcode, &cwaithead))
	{
		fprintf(logfp,"get_cmdwait(%s) failed!\n",hlrcode);
		fclose(logfp);
		exit(1);
	}
	gettimeofday(&t2, NULL);
	fprintf(logfp, "RESUME CMD,read time: %d\n", time_diff(t1, t2));
	fflush(logfp);
	wait11 = cwaithead.next;
	ret = 0;
	memset(&tt, 0, sizeof(tt));
	while(wait11)
	{
		wait12 = wait11->next;
		i = q_add_one_cmd(wait11, NULL, NULL);
		wait11 = wait12;
		ret ++;
	}
	gettimeofday(&t1, NULL);
	fprintf(logfp,"RESUME CMD,add to q, OK %ld, time=%d, count=%d\n",
			time(0), time_diff(t2, t1), ret);
	fflush(logfp);


	/* 创建服务端口 */
	mgr_flag='n';
	comm_port=get_commport(hlrcode,NULL,&mgr_flag);
	if(comm_port<1024 || comm_port>65535)
	{
		fprintf(logfp,"get_commport(%s) failed!\n",hlrcode);
		fclose(logfp);
		exit(1);
	}
	fprintf(logfp,"comm_port == %d\n",comm_port);
	fflush(logfp);
	if((listenid = tcp_listen(NULL, comm_port))<0)
	{
		fprintf(logfp,"listen(%d) failed[%d]\n",listenid,errno);
		fclose(logfp);
		exit(1);
	}

	err_num=0;
	waitnum=0;

	i = time(NULL);
	printf("INIT OK %ld - %ld\n", i-t, i);

	for(;;)
	{
         /*双通道标记检查 如果是备用状态, 则不运行
        if(local_item->status == '2'){
          fprintf(logfp, "dul bak, exit, hlrcode[%s] status[%c]\n", hlrcode, local_item->status);
          fclose(logfp);
          exit(0);
        }*/

		FD_ZERO(&rset);

		FD_SET(listenid,&rset);
		maxid=listenid;

		commptr=commhead.next;
		while(commptr)
		{
			FD_SET(commptr->sockid,&rset);
			if(maxid<commptr->sockid) maxid=commptr->sockid;

			commptr=commptr->next;
		}

		maxid++;
		tval.tv_sec=1;
		tval.tv_usec=0;

		fdret=select(maxid,&rset,NULL,NULL,&tval);
		if(fdret<0)
		{
			fprintf(logfp,"select(%d) failed[%d][%ld]\n",maxid,errno,time(NULL));
			fclose(logfp);
			exit(1);
		}
		time(&t);

		if(fdret==0)
		{
			if(++waitnum>=30)
			{
				waitnum=0;
				fprintf(logfp,"\n");
				fflush(logfp);
			}
			fprintf(logfp,"*");
			fflush(logfp);

			continue;
		}

		if(FD_ISSET(listenid,&rset))
		{
			memset(&cli_addr,0x0,sizeof(cli_addr));
			clilen=sizeof(cli_addr);
			connid=accept(listenid,(struct sockaddr *)&cli_addr,&clilen);
			if(connid<0)
			{
				fprintf(logfp,"accept(%d) failed[%d]\n",listenid,errno);

				if(++err_num>1000)
				{
					fprintf(logfp,"over the mark 1000 times!\n");
					fclose(logfp);
					exit(1);
				}
				else
					continue;
			}
			err_num=0;
			fprintf(logfp,"connect from %s[id=%d][%ld]\n",
				(char *)inet_ntoa(cli_addr.sin_addr),connid,t);

			if(commfree.next)
			{
				commptr=commfree.next;
				commfree.next=commptr->next;
				memset(commptr,0x0,sizeof(struct comm_info));
			}
			else
			{
				commptr=(struct comm_info *)malloc(sizeof(struct comm_info));
				if(commptr==NULL)
				{
					fprintf(logfp,"malloc(%d) failed[%d]\n",sizeof(struct comm_info),errno);
					fclose(logfp);
					exit(1);
				}
				fprintf(logfp,"malloc(%d) at accept()\n",sizeof(struct comm_info),errno);
				memset(commptr,0x0,sizeof(struct comm_info));
			}

			commptr->sockid=connid;
			memcpy(&commptr->cliaddr,&cli_addr,sizeof(cli_addr));
			commptr->begintime=t;

			commptr->next=commhead.next;
			commhead.next=commptr;
		}

		commptr=&commhead;

		while(commptr->next)
		{
			bk_flag = 0;
			if(FD_ISSET(commptr->next->sockid,&rset))
			{
				readlen=readnet(commptr->next->sockid,(char *)&tcpbuf,8);
				if(readlen!=8)
				{
					if(readlen)
						fprintf(logfp,"readnet(%d,0,8)=%d failed[%d]\n",
							commptr->next->sockid,readlen,errno);

					free_comm(commptr);
					continue;
				}

				if(strncmp(tcpbuf.flag,MSGFLAG,4))
				{
					tcpbuf.msglen[0]=0;
					fprintf(logfp,"MSGFLAG is incorrect[%s]!\n",tcpbuf.flag);
					free_comm(commptr);
					continue;
				}

				msglen=get_length(tcpbuf.msglen,4);
				if(msglen<32)
				{
					fprintf(logfp,"MSGLEN is incorrect[%04d]!\n",msglen);
					free_comm(commptr);
					continue;
				}

				readlen=readnet(commptr->next->sockid,(char *)&tcpbuf+8,
					msglen-8);
				if(readlen!=msglen-8)
				{
					fprintf(logfp,"readnet(%d,8,%d)=%d failed[%d]\n",
						commptr->next->sockid,msglen-8,readlen,errno);

					free_comm(commptr);
					continue;
				}
/*printf("RCV:%s[%d]\n",(char *)&tcpbuf,msglen); */

				srvtype=get_length(tcpbuf.srvtype,8);
				transid=get_length(tcpbuf.transid,8);

				switch(srvtype)
				{
				case CONN_KEEP:
					if(send_ack(commptr->next->sockid,DECRYPT,TX_END,srvtype,transid,0)<0)
					{
						fprintf(logfp,"send_ack() failed[%d]\n");
						free_comm(commptr);
					}
					break;
				case SRV_LOGIN:
					loginreqptr=(struct login_req *)tcpbuf.data;
					fprintf(logfp,"LOGIN:hlrcode=%s,hlrport=%c,termtype=%c,time=%ld\n",
						loginreqptr->hlrcode,loginreqptr->hlrport,
						loginreqptr->termtype,t);

					userptr=userhead.next;
					while(userptr)
					{
						if(strncmp(userptr->hlrcode,loginreqptr->hlrcode,3)==0 && userptr->hlrport==loginreqptr->hlrport && userptr->type==loginreqptr->termtype)
						{
							memset(loginuser,0x0,sizeof(loginuser));
							memcpy(loginuser,loginreqptr->username,8);
							if(strcmp(userptr->user,loginuser))
							{
								general_ack(commptr,srvtype,transid,ACK0012);
								break;
							}

							memset(loginpswd,0x0,sizeof(loginpswd));
							memcpy(loginpswd,loginreqptr->userpswd,8);
							if(strcmp(userptr->pswd,loginpswd))
							{
								general_ack(commptr,srvtype,transid,ACK0013);
								break;
							}

							loginackptr=(struct login_ack *)tcpbuf.data;
							memcpy(loginackptr->retn,"0000",4);
							loginackptr->encrypt=userptr->encrypt;
							memcpy(loginackptr->cipher,"12345678",8);

							if(send_ack(commptr->next->sockid,DECRYPT,TX_END,srvtype,transid,LOGINACKLEN)<0)
							{
								fprintf(logfp,"send_ack() failed[%d]\n");
								free_comm(commptr);
							}

							commptr->next->login=LOGIN_REGISTER;
							commptr->next->termtype=userptr->type;
							commptr->next->encrypt=userptr->encrypt;
							memcpy(commptr->next->cipher,"12345678",8);
							commptr->next->begintime=t;
							memcpy(&commptr->next->userinfo,userptr,
								sizeof(struct user_info));

							fprintf(logfp,"LOGIN SUCC:type=%c,encrypt=%c,time=%ld\n",
								commptr->next->termtype,commptr->next->encrypt,t);

							break;
						}

						userptr=userptr->next;
					}

					if(userptr==NULL)
						general_ack(commptr,srvtype,transid,ACK0011);
					break;
				case QUERY0001:
					qry01req=(struct query0001_req *)&tcpbuf.data;
					fprintf(logfp,"QUERY0001:type=%c,datas=%s,time=%ld\n",
						qry01req->type,qry01req->datas,t);

					switch(qry01req->type)
					{
					case 'p':
						/* 查询号码位置 */
						pwaitptr=get_pwaitnode(qry01req->datas,0,0,1);
						if(pwaitptr==NULL)
						{
							fprintf(logfp,"can't find %s in MEM!\n",
								qry01req->datas);
							general_ack(commptr,srvtype,transid,ACK2002);
							break;
						}

						sprintf(fname,"%s/QRY%ld\0",logdir,t);
						sprintf(ackinfo,"QRY%ld\0",t);
						qryfp=fopen(fname,"w");
						if(qryfp==NULL)
						{
							fprintf(logfp,"fopen(%s) for QUERY failed[%d]\n",
								fname,errno);
							general_ack(commptr,srvtype,transid,ACK2003);
							break;
						}

						fprintf(qryfp,"QRY_TYPE:phone\nQRY_DATA:%s\n",
							qry01req->datas);
						fprintf(qryfp,"STATUS:%ld[%ld]\n",pwaitptr->status,t);

						cwaitptr=pwaitptr->cmdhead.next;
						while(cwaitptr)
						{
							fprintf(qryfp,"CMD INFO:\n");
							fprintf(qryfp," streamid:%ld\n",cwaitptr->streamid);
							fprintf(qryfp," cmdid   :%s\n",cwaitptr->cmdid);
							fprintf(qryfp," opcode  :%s\n",cwaitptr->opcode);
							fprintf(qryfp," cmdcode :%d\n",cwaitptr->cmdcode);
							fprintf(qryfp," status  :%c\n",cwaitptr->status);
							fprintf(qryfp," priority:%d\n",cwaitptr->priority);
							fprintf(qryfp," msisdn  :%s\n",cwaitptr->msisdn);
							fprintf(qryfp," imsi    :%s\n",cwaitptr->imsi);
							fprintf(qryfp," ss_info1:%s\n",cwaitptr->ss_info1);
							fprintf(qryfp," ss_info2:%s\n",cwaitptr->ss_info2);
							fprintf(qryfp," ss_info3:%s\n",cwaitptr->ss_info3);
							fprintf(qryfp," req_time:%s\n",cwaitptr->req_time);

							owaitptr=cwaitptr->orderhead.next;
							while(owaitptr)
							{
								fprintf(qryfp,"ORDER INFO:\n");
								fprintf(qryfp,"  ordercode=%d,type=%c,serial=%d,status=%d\n",owaitptr->ordercode,owaitptr->type,owaitptr->serial,owaitptr->status);
								owaitptr=owaitptr->next;
							}

							cwaitptr=cwaitptr->next;
						}
						fprintf(qryfp,"END\n");
						fclose(qryfp);

						general_ack(commptr,srvtype,transid,0,ackinfo);
						break;
					case 'i':
						/* 查询优先级索引 */
						sprintf(fname,"%s/QRY%ld\0",logdir,t);
						sprintf(ackinfo,"QRY%ld\0",t);

						qryfp=fopen(fname,"w");
						if(qryfp==NULL)
						{
							fprintf(logfp,"fopen(%s) for QUERY failed[%d]\n",
								fname,errno);
							general_ack(commptr,srvtype,transid,ACK2003);
							break;
						}

						fprintf(qryfp,"QRY_TYPE:priority\n");
						priptr=prihead.next;
						while(priptr)
						{
							fprintf(qryfp,"priority=%03d	num=%03d\n",
								priptr->priority,priptr->num);

							pidxptr=priptr->phonehead.next;
							while(pidxptr)
							{
								fprintf(qryfp,"    phone = %07d%04d\n",
									pidxptr->phead,pidxptr->ptail);

								pidxptr=pidxptr->next;
							}

							priptr=priptr->next;
						}

						fprintf(qryfp,"END\n");
						fclose(qryfp);

						general_ack(commptr,srvtype,transid,0,ackinfo);
						break;
					default:
						general_ack(commptr,srvtype,transid,ACK2001);
						break;
					}
					break;
				case SRVCODE0001:
					gettimeofday(&tt1, NULL);
					if(memcmp(tcpbuf.reserve, "VV", 2) == 0){
						y = get_length(tcpbuf.reserve+2, 2);
						fprintf(logfp, "VVVVVVVV=%d\n", y);
						fflush(logfp);
					}else
						y = 1;
					for(x=0; x<y; x++)
					{
						srvcode0001req=(struct op_data_req *)&tcpbuf.data;
						srvcode0001req += x;
						fprintf(logfp,"SRVCODE0001:%s %s %s %s %s %c %c %s %s %s %s %s",
							srvcode0001req->command_id,
							srvcode0001req->phone_no,
							srvcode0001req->imsi_no,
							srvcode0001req->op_code,
							srvcode0001req->command_code,
							srvcode0001req->cmd_status,
							srvcode0001req->phone_priority,
							srvcode0001req->ss_info1,
							srvcode0001req->ss_info2,
							srvcode0001req->ss_info3,
							srvcode0001req->req_time,
							srvcode0001req->login_no);

						cmd_code=atoi(srvcode0001req->command_code)*10+srvcode0001req->cmd_status-'0';
						/* 查询对应的指令 */
						cmdptr=get_ordernode(cmd_code);
						if(cmdptr==NULL)
						{
							fprintf(logfp,"[1002]\n");
							cmd_save_file(srvcode0001req, 1002);
							continue;
						}

						/* 查询号码位置 */
						pwaitptr=get_pwaitnode(srvcode0001req->phone_no,0,0,0);
						if(pwaitptr==NULL)
						{
							fprintf(logfp,"[1001]\n");
							cmd_save_file(srvcode0001req, 1001);
							continue;
						}
						fprintf(logfp, "[0000]\n");
						stream_id = cmd_save_file(srvcode0001req, 0000);

						/* 分配命令节点,保存信息 */
						cwaitptr=get_cwaitnode(pwaitptr);
						cwaitptr->streamid=stream_id;
						strcpy(cwaitptr->cmdid,srvcode0001req->command_id);
						strcpy(cwaitptr->opcode,srvcode0001req->op_code);
						cwaitptr->cmdcode=cmd_code;
						cwaitptr->status=srvcode0001req->cmd_status;
						cwaitptr->priority=get_priority(srvcode0001req->op_code)+(srvcode0001req->phone_priority-'0')*1000+cmdptr->priority;
						strcpy(cwaitptr->msisdn,srvcode0001req->phone_no);
						strcpy(cwaitptr->imsi,srvcode0001req->imsi_no);
						strcpy(cwaitptr->ss_info1,srvcode0001req->ss_info1);
						strcpy(cwaitptr->ss_info2,srvcode0001req->ss_info2);
						strcpy(cwaitptr->ss_info3,srvcode0001req->ss_info3);
						strcpy(cwaitptr->req_time,srvcode0001req->req_time);

						q_add_one_cmd(cwaitptr, cmdptr, pwaitptr);
						/*print_cwait("enq", logfp, &(pwaitptr->cmdhead)); */
					}
					general_ack(commptr,srvtype,transid,ACK0000);
					yy_time.rc4enq += y;
					gettimeofday(&tt2, NULL);
					yy_time.tm4enq += time_diff1(tt1, tt2);
					break;

				case GETORDER:
					if(commptr->next->login!=LOGIN_REGISTER)
					{
						general_ack(commptr,srvtype,transid,ACK0021);
						break;
					}

					cmdreqptr=(struct cmd_data_req *)tcpbuf.data;

					if(cmdreqptr->type==REPLY_GET)
					{
						gettimeofday(&ttt1,NULL);
						ret=reply_info_deal(commptr->next,cmdreqptr);
						gettimeofday(&ttt2,NULL);
						yy_time.rc4dlr ++;
						yy_time.tm4dlr += time_diff1(ttt1, ttt2);
						if(ret)
						{
							switch(ret)
							{
							case 3002:
								general_ack(commptr,srvtype,transid,ACK3002);
								break;
							case 3003:
								general_ack(commptr,srvtype,transid,ACK3003);
								break;
							case 3009:
								fprintf(logfp,"HLR_ERROR,reconnect to HLR!\n");
								free_comm(commptr);
								break;
							default:
								general_ack(commptr,srvtype,transid,ACK0101);
								break;
							}
							break;
						}
					}

					gettimeofday(&ttt1,NULL);
					ret = get_one_order(commptr->next, t);
					gettimeofday(&ttt2,NULL);
					yy_time.tm4gto += time_diff1(ttt1, ttt2);
					tt[0] += time_diff1(ttt1, ttt2);
					if(ret)
					{
						if(send_ack(commptr->next->sockid,DECRYPT,TX_END,srvtype,transid,CMDACKLEN)<0)
						{
							fprintf(logfp,"send_ack() failed[%d]\n");
							free_comm(commptr);
						}
						yy_time.rc4gto ++;
					}
					else
					{
						general_ack(commptr,srvtype,transid,ACK3001);
					}
					break;
				default:
					fprintf(logfp,"srvtype error[%d]\n",srvtype);
					general_ack(commptr,srvtype,transid,ACK0001);
					free_comm(commptr);
					continue;
				}
			}

			/*** ADD ON 20050804 ***/
			if(bk_flag == 1)
				break;
			if(commptr->next)
				commptr=commptr->next;
		}
		time(&t);
		/*判断日志文件翻转和记录性能日志*/
		logfp = open_logfp(t, logfp, hlrcode);
		yy_time_log(t);

	}

	fclose(logfp);
	return 0;
}


