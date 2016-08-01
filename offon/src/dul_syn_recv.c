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

#include "file_func.h"

#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <errno.h>

char L[40];

#include "db_func.h"

#include "dul_util.h"

char hlrcode[8];
FILE *logfp;
char dbloginfile[128];

void usage(char *s)
{
	printf("%s\n\nUsage:dul_syn_recv -Hhlrcode [-V]\n\n\n", s);
	exit(1);
}

void LogOut(int sig)
{
    local_item->rcv_q_len = 0;
	/*printf("LogOut signal==%d\n",sig); */
	if(logfp)
	{
		fprintf(logfp,"LogOut signal==%d\n",sig);
		fclose(logfp);
	}

	exit(1);
}

int write_buf(int sockid, char *buf, int len)
{
    char temp[2048];
    int i, ret;
    struct pollfd *pp;

    len = strlen(buf);
    sprintf(temp, "%08d", len);
    memcpy(temp+8, buf, len);
    ret = tcp_writen(sockid, temp, len+8);
    printf("write_buf[%s], return %d\n", buf, ret);
    return ret;
}

int read_buf(int sockid, char *buf)
{
    char tmp[10];
    int i, j;
    i = tcp_readn(sockid, tmp, 8);
    if(i != 8)
        return -1;
    tmp[8] = 0;
    j = atoi(tmp);
    i = tcp_readn(sockid, buf, j);
    if(i != j)
        return -2;
    buf[i] = 0;
    printf("read_buf[%s]\n", buf);
    return i;
}

void *thread_tcp_recv_data(void *param)
{
    int servsock = -1;
    int sockid = -1;
    char clip[35];
    int ret, clport;
    char buf[64000];

    servsock = tcp_listen(NULL, local_item->rpt);
    if(servsock < 0){
        file_log(L, "can not listen on %d, hlrcode:%s", local_item->rpt, hlrcode);
        exit(1);
    }
    set_hlrcode(hlrcode);
    while(1){
        if(sockid < 0){
            sockid = tcp_accept(servsock, clip, &clport);
            if(sockid < 0){
                file_log(L, "tcp_accept failed, return %d, errno:%d", sockid, errno);
                exit(2);
            }
        }
        while(1){
            ret = read_buf(sockid, buf);
            if(ret <= 0){
                tcp_close(sockid);
                sockid = -1;
                break;
            }
            write_record(buf, ret, 0); /*写到offon_fordb 的文件中, 由offon_fordb负责入库*/
            write_buf(sockid, "ok", 2);
        }
        sleep(1);
    }
    return NULL;
}

/*从断点文件中读取断点*/
int dul_get_pos(char *hlr_code, char *date, long *rpos)
{
	FILE *fp;
	char fname[128];
	int wfid;

	*rpos = -1;
	sprintf(fname, "%s/databuf/.%s.pos.syn1",
			getenv("HOME"), hlr_code);
	fp = fopen(fname, "r");
	if(fp == NULL){
		char dd[9];
		get_cur_timestr(date, 1);
		*rpos = 0;
		return 0;
	}
	fscanf(fp, "%s %ld", date, rpos);
	fclose(fp);
	return 0;
}

int dul_sav_pos(char *hlr_code, char *date, long rpos){
	FILE *fp;
	char fname[128];

	sprintf(fname, "%s/databuf/.%s.pos.syn1",
			getenv("HOME"), hlr_code);
	fp = fopen(fname, "w");
	if(fp == NULL)
		return -1;
	fprintf(fp, "%s %ld\n", date, rpos);
	fclose(fp);
	return 0;
}


int dul_read_bak(char *hlr_code, char *buf, int maxlen)
{
	static char cdate[10];
	static long rpos = -1;
	static FILE *fp = NULL;
	char filename[128], *ptr, *line;
	int llen, readlen;

	if(fp == NULL){
		dul_get_pos(hlr_code, cdate, &rpos);
        printf("dul_get_pos %s return cdate[%s] rpos[%ld]\n",
            hlr_code, cdate, rpos);
		sprintf(filename, "%s/databuf/syn_%s.%s", getenv("HOME"), hlr_code, cdate);
		fp = fopen(filename, "r");
		while(fp == NULL){
			sleep(1);
			get_cur_timestr(cdate, 1);
			sprintf(filename, "%s/databuf/syn_%s.%s", getenv("HOME"), hlr_code, cdate);
			rpos = 0;
			fp = fopen(filename, "r");
            printf("open file %s return %08x\n", filename, fp);
		}
		if(rpos > 0)
			fseek(fp, rpos, SEEK_SET);
	}
	readlen = 0;
	line = buf;
	while( (ptr = fgets(line, maxlen, fp)) != NULL)
	{
		llen = strlen(line);
		maxlen -= llen;
		line += llen;
		readlen += llen;
		if(maxlen <= 1024) break;
	}
	if(ptr == NULL){
		/*已经读到文件末尾了, 检查是否需要切换文件*/
		char date[10];
		get_cur_timestr(date, 1);
		sprintf(filename, "%s/databuf/syn_%s.%s", getenv("HOME"), hlr_code, date);
		if(strcmp(date, cdate) != 0){
			char oldfile[128], bakfile[128];
			fclose(fp);
			/*移动旧文件到bak目录*/
			sprintf(oldfile, "%s/databuf/syn_%s.%s", getenv("HOME"), hlr_code, cdate);
			sprintf(bakfile, "%s/databuf/bak/syn_%s.%s", getenv("HOME"), hlr_code, cdate);
			rename(oldfile, bakfile);
            printf("switch file [%s] to [%s]\n", oldfile, filename);
			rpos = 0;
			fp = fopen(filename, "r");
			strcpy(cdate, date);
            dul_sav_pos(hlr_code, cdate, rpos);
		}
	}
	if(fp != NULL){
		rpos = ftell(fp);
	}
	if(readlen > 0)
		dul_sav_pos(hlr_code, cdate, rpos);

	return readlen;
}

/*从文件中读取同步数据,然后向备用侧进行同步*/
void *thread_send_bak(void *parm)
{
    char dip[35];
    int  dpt;
    char status;
    int sockid = -1;
    char buf[64000], tmp[50];
    int i,j;
    int send_status;
    int data_flag;

    dul_find(hlrcode);

    data_flag = 0;
    while(1)
    {
        while(sockid == -1){
            sockid = tcp_connect(local_item->dip, local_item->dpt);
            if(sockid < 0){
                file_log(L, "can not connect bak server %s:%d", local_item->dip, local_item->dpt);
                sockid = -1;
                sleep(2);
            }
            /*设置sotimeout*/
            tcp_setrcvtimeo(sockid, 10000);
			file_log(L, "tcp_connect %s:%d succ %d",
					local_item->dip, local_item->dpt, sockid);
        }
        if(data_flag == 0){
            i = dul_read_bak(hlrcode, buf+8, sizeof(buf)-8);
            if(i <= 0){
                /* 没有数据 */
                file_log(L, "no data, sleep(1)");
                sleep(1);
                continue;
            }
        }
        data_flag = 1;
		sprintf(tmp, "%08d", i);
		memcpy(buf, tmp, 8);
        j = tcp_writen(sockid, buf, i+8); /*write data */
        if(j < i+8 ){
        	file_log(L, "tcp send data failed, %d", i);
            tcp_close(sockid);
            sockid = -1;
            sleep(1);
            continue;
        }
        read_buf(sockid, tmp);
        data_flag = 0;
    }
    if(sockid != -1)
        tcp_close(sockid);

    return NULL;
}


int main(int argc,char **argv)
{

	int ret,count;
	int i;
	FILE *cfgfp;
	char cfgstr[256];
	char logfilename[256],logdir[256];
	time_t t;
	char tempstr[1024];
	char buf[1024], *rp[30];
	char cmdcode[4];
	char cmd_status[2];
    long c1, c2;
    time_t t1, t2;
    pthread_t th1;

	if(argc<2)
		usage("Incorrect Parameter!");

    logfp = stdout;

	signal(SIGTERM,LogOut);
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
		default:
			usage("Unknow Parameter!");
			break;
		}
	}

	if(GenDaemon()<0)
	{
		printf("GenDaemon() failed!\n");
		exit(1);
	}

	if(strlen(hlrcode) != 3 )
		usage("Incorrect parameter [hlrcode]!\n");

    sprintf(L, "synr_%s", hlrcode);

	sprintf(dbloginfile, "%s/gdblogin.cfg", getenv("ETCDIR"));

    dul_find(hlrcode);

    /*启动从网络接收备份数据线程*/
    ret = pthread_create(&th1, NULL, thread_tcp_recv_data, NULL);

    thread_send_bak(NULL);

    return 0;
}

