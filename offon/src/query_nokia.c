
/*********************************************************
 *
 *  FileName:
 *      query_ericsson.c
 *
 *  Version:
 *      1.00 2006.06.03
 *
 *  tabstop = 4
 *
 ********************************************************/
#include <sqlca.h>
#include <sqlda.h>
#include <h.h>
#include <db_func.h>
#include <dmcs.h>

#define VERSION "1.0.0"

char hlrcode[8];
FILE *logfp;
FILE *sndfp,*rcvfp;
int sndid,rcvid;

char *procname;

int                                     listenid;
int                                     connid;
int                                     commport;
struct sockaddr_in      srvaddr;
int                                     optinfo,optlen;

char                            gsmip[32];
int                                     gsmport;
int                             gsmid;
char                            gsmuser[32];
char                            gsmpswd[32];

char                            tcpbuf[256];struct gsm_info_sc{    long                idno;    char                msisdn[16];    char                imsi[24];    char                hlrcode[8];    char                obo;    char                obi;    char                toll;    char                roam;    char                voice;    char                gprs;    char                smssnd;    char                smsrcv;    char                fax;    char                data;    char                wlan;    char                cfu;    char                cfb;    char                cfnry;    char                cfnrc;    char                caw;    char                hold;    char                mpty;    char                baoc;    char                baic;    char                boic;    char                clip;    char                clir;    char                rbt;    char                wap;    char                ussd;    char                prov;    char                bs3g; /**added by yanyt 090511 视频通话**********/    char                ocsiaddr[12];    char                ocsiskey[4];    char                tcsiaddr[12];    char                tcsiskey[4];    char                scp[16];    char                vpmn;    char                grpno[16];    char                vpnno[16];    char                power[8];    char                pkg[16];    char                tariff; /**added by hepan 20120508 彩铃付费*/    char                colorset[4];/** added by xulin 20120816 彩铃套餐*/    int                 status;    char                runcode;    struct gsm_info_sc      *next;};
struct gsm_info_sc      uptr;

int timeout;
char enter[2];
char orderinfo[1024];
char replystr[32767];
char replystr_total[131072];

#define         STATION         10

extern int GenDaemon();
extern int get_hlr_cfg();

void usage(char *s)
{
        printf("%s\n\nUsage:%s -Hhlrcode [-V]\n\n\n",s,procname);
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

int delespace(char *s)
{
  int len;
  char *src,*dst;

  for(len=0,src=dst=s;*src;src++)
  {
    if(*src!=0x09 && *src!=0x0a && *src!=0x0d && *src!=0x20)
      len++,*dst++=*src;
  }

  *dst=0x0;

  return len;
}


/***
void cal_time(struct timeval *beg,struct timeval *end,char *s)
{
        time_t t;

        if(beg->tv_sec<end->tv_sec)
                t=(end->tv_sec-beg->tv_sec)*1000000+end->tv_usec-beg->tv_usec;
        else
                t=end->tv_usec-beg->tv_usec;

        fprintf(logfp," %s_VAL_TIME:%06ld\n",s,t);

        return;
}
***/

void catchalm(int sig)
{
        fprintf(logfp,"TIMEOUT!!![%ld]\n",time(NULL));
        fflush(logfp);
        timeout=1;

        return;
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
/**
                        fprintf(logfp,"%c",*ptr);
printf("RCV: %c         %02x    %02x\n",*ptr,*ptr,*(ptr-1));
**/
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
printf("RCV: %c         %02x\n",*ptr,*ptr);
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

void parse_queryinfo()
{
        char *msisdnp,*imsip;

        msisdnp=strstr(replystr_total+STATION,"MOBILE STATION ISDN NUMBER ................. 861");
        if(msisdnp)
                memcpy(uptr.msisdn,msisdnp+47,11);

        imsip=strstr(replystr_total+STATION,"INTERNATIONAL MOBILE SUBSCRIBER IDENTITY ... 46");
        if(msisdnp)
                memcpy(uptr.imsi,imsip+45,15);

        if(strstr(replystr_total+STATION,"BOIC ... BARRING OF OUTGOING INTERNATIONAL CALLS ............ Y"))
        {
                uptr.toll='2';
        }
        else
        {
                uptr.toll='1';
        }

        if(strstr(replystr_total+STATION,"ROAMING PROFILE INDEX ...................... 22"))
        {
                uptr.roam='4';
        }
        else if(strstr(replystr_total+STATION,"ROAMING PROFILE INDEX ...................... 21"))
        {
                uptr.roam='3';
        }
        else if(strstr(replystr_total+STATION,"SERVICE AREA OF MSISDN ..................... ALL"))
        {
                uptr.roam='1';
        }
        else
        {
                uptr.roam='2';
        }

        if(strstr(replystr_total+STATION,"PRIMARY BASIC SERVICE CODE ................. T11"))
                uptr.voice='1';
        else
                uptr.voice='0';

        if(strstr(replystr_total+STATION,"NETWORK ACCESS .............. BOTH"))
                uptr.gprs='1';
        else
                uptr.gprs='0';

        if(strstr(replystr_total+STATION,"BASIC SERVICE .............................. T22,000"))
                uptr.smssnd='1';
        else
                uptr.smssnd='0';

        if(strstr(replystr_total+STATION,"BASIC SERVICE .............................. T21,000"))
                uptr.smsrcv='1';
        else
                uptr.smsrcv='0';

        if(strstr(replystr_total+STATION,"BASIC SERVICE .............................. T62,000"))
                uptr.fax='1';
        else
                uptr.fax='0';

        if(strstr(replystr_total+STATION,"BASIC SERVICE .............................. B16,000"))
                uptr.data='1';
        else
                uptr.data='0';

        if(strstr(replystr_total+STATION,"CFU...CALL FWD UNCONDITIONAL ......... Y"))
                uptr.cfu='1';
        else
                uptr.cfu='0';

        if(strstr(replystr_total+STATION,"CFB...CALL FWD ON SUBSCRIBER BUSY .... Y"))
                uptr.cfb='1';
        else
                uptr.cfb='0';

        if(strstr(replystr_total+STATION,"CFNA..CALL FWD ON NO REPLY ........... Y"))
                uptr.cfnry='1';
        else
                uptr.cfnry='0';

        if(strstr(replystr_total+STATION,"CFNR..CALL FWD ON SUBS. NOT REACHABLE  Y"))
                uptr.cfnrc='1';
        else
                uptr.cfnrc='0';

        if(strstr(replystr_total+STATION,"CW....CALL WAITING ................................... Y"))
                uptr.caw='1';
        else
                uptr.caw='0';

        if(strstr(replystr_total+STATION,"HOLD...CALL HOLD ........................... Y"))
                uptr.hold='1';
        else
                uptr.hold='0';

        if(strstr(replystr_total+STATION,"MPTY...MULTI PARTY SERVICE ................. Y"))
                uptr.mpty='1';
        else
                uptr.mpty='0';

        if(strstr(replystr_total+STATION,"BAOC ... BARRING OF ALL OUTGOING CALLS ...................... Y"))
    {
                uptr.obo='0';
    }
        else
    {
                uptr.obo='1';
    }

        if(strstr(replystr_total+STATION,"BAIC ... BARRING OF ALL INCOMING CALLS ...................... Y"))
                uptr.obi='0';
        else
                uptr.obi='1';


        if(strstr(replystr_total+STATION,"CLIP...CALLING LINE ID PRESENTATION ........ Y"))
                uptr.clip='1';
        else
                uptr.clip='0';

        if(strstr(replystr_total+STATION,"CALLING LINE ID RESTRICTION ......... PCBN"))
                uptr.clir='2';
        else if(strstr(replystr_total+STATION,"SOCLIR-1"))
                uptr.clir='2';
        else
                uptr.clir='0';

        if(strstr(replystr_total+STATION,"USSDB..USSD BARRING......................... Y"))
                uptr.ussd='1';
        else
                uptr.ussd='0';

        if(strstr(replystr_total+STATION,"MOBILE STATION CATEGORY .................... PR"))
                uptr.prov='1';
        else
                uptr.prov='0';

        if(strstr(replystr_total+STATION,"SRBT...SELECTIVE RINGBACK TONE ............. Y"))
                uptr.rbt='1';
        else
                uptr.rbt='0';

        if(strstr(replystr_total+STATION,"SCP....SERVICE CONTROL POINT ADDRESS........86"))
                uptr.vpmn='1';
        else
                uptr.vpmn='0';

        return;
}

int exec_cmd(int flag)
{
        char s[32];
        int retn;
        char msisdn[16],stream[16],ocode[8];
        int ordercode;
        char *src,*dest,*para;
        int beg,len;
        char *ptr;
    char imsi[16];
    char *p0;

        switch(flag)
        {
        case 0:
                fprintf(logfp,"Now no datas[%ld]\n",time(NULL));
        retn = sndcmd(sndid, "\r\n< ", "\r\n", 2,
                    "\r\n< ", replystr, sizeof(replystr));
                fflush(logfp);

                return 0;
        case 1:
                /*** ORDER SND TO HLR ***/
                memset(orderinfo,0x0,sizeof(orderinfo));
                sprintf(orderinfo,"ZMIO:MSISDN=86%s;\r",tcpbuf+7);

        retn = sndcmd(sndid, "\r\n< \08 ", orderinfo, strlen(orderinfo),
                    "_>\r\n< \08 ", replystr, sizeof(replystr));
        if(retn <= 0)
            {
          fprintf(logfp, "sndcmd failed, return:%d\n", retn);
          return -1;
        }
  
                memset(tcpbuf,0x0,sizeof(tcpbuf));
                if(strstr(replystr+STATION,"/*** DX ERROR: "))
                {
                        uptr.idno=0;
                        strcpy(uptr.hlrcode,hlrcode);

                        fprintf(logfp,"NOT FOUND!\n");
                        fflush(logfp);

                        return 1;
                }

        if((p0=strstr(replystr+STATION,"INTERNATIONAL MOBILE SUBSCRIBER IDENTITY ..."))==NULL)
        {
          uptr.idno=0;
          strcpy(uptr.hlrcode,hlrcode);

          fprintf(logfp,"IMSI NOT FOUND!\n");
          fflush(logfp);

          return 1;
        }

        memset(imsi,0x0,sizeof(imsi));
            memcpy(imsi,p0+45,15);
        delespace(imsi);

            printf("imsi:------%s\n",imsi);
        fprintf(logfp,"imsi:------%s\n",imsi);

        if(strlen(imsi)!=15)
        {
          strcpy(uptr.imsi,imsi);
          uptr.idno=0;
          strcpy(uptr.hlrcode,hlrcode);

          fprintf(logfp,"IMSI ERROR -----%s!\n",imsi);
          fflush(logfp);

          return 1;
        }
        
        memset(replystr_total,0x0,sizeof(replystr_total));
        strcpy(replystr_total,replystr+STATION);
  
            memset(orderinfo,0x0,sizeof(orderinfo));
        sprintf(orderinfo,"ZMBO:IMSI=%s;\r",imsi);
        retn = sndcmd(sndid, "\r\n< \08 ", orderinfo, strlen(orderinfo),
                    "_>\r\n< \08 ", replystr, sizeof(replystr));
        if(retn <= 0)
        {
          fprintf(logfp, "sndcmd failed, return:%d\n", retn);
          return -1;
        }
        strcat(replystr_total,replystr+STATION);

        memset(orderinfo,0x0,sizeof(orderinfo));
        sprintf(orderinfo,"ZMSO:IMSI=%s;\r",imsi);
        retn = sndcmd(sndid, "\r\n< \08 ", orderinfo, strlen(orderinfo),
                    "_>\r\n< \08 ", replystr, sizeof(replystr));
        if(retn <= 0)
        {
          fprintf(logfp, "sndcmd failed, return:%d\n", retn);
          return -1;
        }
        strcat(replystr_total,replystr+STATION);

        memset(orderinfo,0x0,sizeof(orderinfo));
        sprintf(orderinfo,"ZMGO:IMSI=%s;\r",imsi);
        retn = sndcmd(sndid, "\r\n< \08 ", orderinfo, strlen(orderinfo),
                    "_>\r\n< \08 ", replystr, sizeof(replystr));
        if(retn <= 0)
        {
          fprintf(logfp, "sndcmd failed, return:%d\n", retn);
          return -1;
        }
        strcat(replystr_total,replystr+STATION);

        memset(orderinfo,0x0,sizeof(orderinfo));
        sprintf(orderinfo,"ZMNO:IMSI=%s;\r",imsi);
        retn = sndcmd(sndid, "\r\n< \08 ", orderinfo, strlen(orderinfo),
                    "_>\r\n< \08 ", replystr, sizeof(replystr));
        if(retn <= 0)
        {
          fprintf(logfp, "sndcmd failed, return:%d\n", retn);
          return -1;
        }
        strcat(replystr_total,replystr+STATION);

        memset(orderinfo,0x0,sizeof(orderinfo));
        sprintf(orderinfo,"ZMQO:IMSI=%s,:DISP=CA;\r",imsi);
        retn = sndcmd(sndid, "\r\n< \08 ", orderinfo, strlen(orderinfo),
                    "_>\r\n< \08 ", replystr, sizeof(replystr));
        if(retn <= 0)
        {
          fprintf(logfp, "sndcmd failed, return:%d\n", retn);
          return -1;
        }
        strcat(replystr_total,replystr+STATION);

                printf("replytotal:\n-------------\n%s\n---------\n",replystr_total);

                strcpy(uptr.hlrcode,hlrcode);
                uptr.idno=1;
                parse_queryinfo();

                fprintf(logfp,"SUCCESS\n");
                fflush(logfp);

                return 1;
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
                if(close(in[1])==-1||close(ou[0])==-1||\
                        close(in[0])==-1||close(ou[1])==-1)
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

int main(int argc,char **argv,char **envp)
{
        fd_set rset;
        int fdret;
        struct timeval tval;

        int today,ret;
        char logfilename[256],logdir[256];
        time_t t;
        struct tm *tv;
    char ss[64];

        int readlen;

        FILE *cfgfp;

        procname=argv[0];
        logfp=NULL;

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
                case 'V':
                        printf("\n\n    version = %s\n\n",VERSION);
                        exit(0);
                default:
                        usage("Unknow Parameter!");
                        break;
                }
        }

printf("hlrcode===%s~\n",hlrcode);
        /*if(strlen(hlrcode)!=1)
                usage("Incorrect parameter [hlrcode]!\n");*/

        time(&t);
        tv=localtime(&t);
        memset(logfilename,0x0,sizeof(logfilename));
        sprintf(logfilename,"%s/query_nokia_%04d%02d%02d.%s",getenv("LOGDIR"),
                tv->tm_year+1900,tv->tm_mon+1,tv->tm_mday,hlrcode);

        logfp=fopen(logfilename,"a");
        if(logfp==NULL)
        {
                printf("can't open %s for LOG!\n",logfilename);
                exit(1);
        }

        fprintf(logfp,"INIT %04d/%02d/%02d %02d:%02d:%02d\n",
                tv->tm_year+1900,tv->tm_mon+1,tv->tm_mday,
                tv->tm_hour,tv->tm_min,tv->tm_sec); 
        fflush(logfp);
        today=tv->tm_mday;

        /************ INIT CFG FILENAME ********************/
        commport=0;
        memset(gsmip,0x0,sizeof(gsmip));
        gsmport=0;
        memset(gsmuser,0x0,sizeof(gsmuser));
        memset(gsmpswd,0x0,sizeof(gsmpswd));

        if(get_hlr_cfg(hlrcode,&commport,gsmip,&gsmport,gsmuser,gsmpswd)<=0)
        {
                fprintf(logfp,"get_cfg_filename(%s) failed!\n",hlrcode);
                fclose(logfp);
                exit(1);
        }

printf("commport=====%d\n",commport);
printf("gsmip========%s~\n",gsmip);
printf("gsmport======%d\n",gsmport);
printf("username=====%s~\n",gsmuser);
printf("userpswd=====%s~\n",gsmpswd);

        /************ INIT COMM_PORT ***********************/
        memset(&srvaddr,0x0,sizeof(srvaddr));
        srvaddr.sin_family=AF_INET;
        srvaddr.sin_addr.s_addr=0;
        srvaddr.sin_port=htons(commport);

        listenid=socket(AF_INET,SOCK_STREAM,0);
        if(listenid<0)
        {
                fprintf(logfp,"socket() failed[%d]\n",errno);
                fclose(logfp);
                exit(1);
        }

        optinfo=1;
        optlen=sizeof(optinfo);
        setsockopt(listenid,SOL_SOCKET,SO_REUSEADDR,(void *)&optinfo,optlen);

        if(bind(listenid,(struct sockaddr *)&srvaddr,sizeof(srvaddr))<0)
        {
                fprintf(logfp,"bind() failed[%d]\n",errno);
                fclose(logfp);
                exit(1);
        }

        if(listen(listenid,5)<0)
        {
                fprintf(logfp,"listen() failed[%d]\n",errno);
                fclose(logfp);
                exit(1);
        }

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
    memset(ss,0x0,sizeof(ss));
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

        /************** GET AN ORDER AND DEAL ********************/
        while(1)
        {
                /*add by lizw for initialize strcut 20100119 */
                memset(&uptr,0x0,sizeof(struct gsm_info_sc));
                
                FD_ZERO(&rset);
                FD_SET(listenid,&rset);
                tval.tv_sec=30;
                tval.tv_usec=0;

                fdret=select(listenid+1,&rset,NULL,NULL,&tval);
                if(fdret<0)
                {
                        fprintf(logfp,"select(%d) failed[%d]\n",listenid,errno);
                        break;
                }
                if(fdret==0)
                {
                        ret=exec_cmd(0);
printf("EXEC_CMD===========%d\n",ret);
                        if(ret<0)
                        {
                                fprintf(logfp,"exec_cmd(0)==%d failed!\n",ret);
                                break;
                        }

                        continue;
                }

                connid=accept(listenid,NULL,NULL);
                if(connid<0)
                {
                        fprintf(logfp,"accept(%d) failed[%d]\n",listenid);
                        fflush(logfp);
                        break;
                }

                memset(tcpbuf,0x0,sizeof(tcpbuf));
                readlen=read(connid,tcpbuf,18);
                if(readlen!=18)
                {
                        fprintf(logfp,"read(%d)=%d failed[%d]\n",connid,readlen,errno);
                        fflush(logfp);

                        break;
                }

                if(strncmp(tcpbuf,"MSISDN:",7))
                {
                        fprintf(logfp,"MSGFLAG is incorrect [%s]!\n",tcpbuf);
                        break;
                }

                ret=exec_cmd(1);
printf("EXEC_CMD+++++++++++%d\n",ret);
                if(ret<0)
                {
                        fprintf(logfp,"exec_cmd(1)=%d failed!\n",ret);
                        break;
                }

                if(write(connid,&uptr,sizeof(uptr))!=sizeof(uptr))
                {
                        fprintf(logfp,"wriet(%d) failed[%d]\n",connid,errno);
                        break;
                }

                time(&t);
                tv=localtime(&t);

                if(today!=tv->tm_mday)
                {
                        fclose(logfp);

                        memset(logfilename,0x0,sizeof(logfilename));
                        sprintf(logfilename,"%s/day_erc_%04d%02d%02d.%s",logdir,
                                tv->tm_year+1900,tv->tm_mon+1,tv->tm_mday,hlrcode);

                        logfp=fopen(logfilename,"a");
                        if(logfp==NULL)
                        {
                                printf("can't open %s for LOG!\n",logfilename);
                                exit(1);
                        }

                        today=tv->tm_mday;
                }

                close(connid);
        }

        fclose(logfp);
        close(listenid);

        return 0;
}
