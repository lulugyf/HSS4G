#include "offon.h"

extern FILE *logfp;

int timeout;

char *rlspace(char *s)
{
	int len;

	if(s==NULL)
		return NULL;

	for(len=strlen(s)-1;len>=0;len--)
	{
		if(s[len]==' ' || s[len]=='\t' || s[len]==0x0a || s[len]==0x0d)
			s[len]=0x0;
		else
			break;
	}

	for(;;)
	{
		if(*s==' ' || *s=='\t' || *s==0x0a || *s==0x0d)
			s++;
		else
			return s;
	}
}

int get_length(char *s,int len)
{
	int ret,i;

	for(i=0,ret=0;i<len;i++)
	{
		if(s[i]>='0' && s[i]<='9')
			ret=ret*10+s[i]-'0';
		else
			break;
	}

	return ret;
}

void sig_alarm(int sig)
{
	timeout=1;
	fprintf(logfp,"sig_alarm(%d) timeout!\n",sig);

	return;
}

int readnet(int id,char *buf,int len)
{
	int recvlen,leftlen,readlen;

	readlen=0;
	recvlen=0;
	leftlen=len;

	timeout=0;
	signal(SIGALRM,sig_alarm);

	while(leftlen)
	{
		timeout=0;
		alarm(15);

		readlen=read(id, buf+recvlen, leftlen);
		if(readlen==0)
		{
			fprintf(logfp,"client close[%d]!\n",id);
			recvlen=0;
			close(id);
			break;
		}
		if(readlen<0)
		{
			if(timeout)
			{
				fprintf(logfp,"read(%d,%d,%d) timeout!\n",id,recvlen,leftlen);
				recvlen=readlen;
				close(id);
				break;
			}
			else
			{
				fprintf(logfp,"read(%d,%d,%d) failed[%d]!\n",id,recvlen,leftlen,errno);
				recvlen=readlen;
				close(id);
				break;
			}
		}

		recvlen += readlen;
		leftlen -= readlen;
	}

	alarm(0);
	signal(SIGALRM,SIG_IGN);

	return recvlen;
}

int writenet(int id,char *buf,int len)
{
	int sendlen,leftlen,writelen;

	leftlen=len;
	sendlen=0;

	while(leftlen)
	{
		writelen=write(id,buf+sendlen,leftlen);
		if(writelen<0)
		{
			fprintf(logfp,"write(%d,%d,%d) failed[%d]\n",id,sendlen,leftlen,errno);
			close(id);
			return writelen;
		}

		sendlen += writelen;
		leftlen -= writelen;
	}

	return sendlen;
}

void get_valstr(char *d,char *s,char *p1,char *p2,char *p3,char *p4,char *p5,char *p6)
{
	int beg,len;
	char *p;

	while(*s)
	{
		if(*s=='$')
		{
			if(!isdigit(s[1]) || !isdigit(s[2]) || !isdigit(s[3]) || !isdigit(s[4]) || !isdigit(s[5]))
				break;

			switch(s[1])
			{
			case '1':
				p=p1;
				break;
			case '2':
				p=p2;
				break;
			case '3':
				p=p3;
				break;
			case '4':
				p=p4;
				break;
			case '5':
				p=p5;
				break;
			case '6':
				p=p6;
				break;
			default:
				p=NULL;
				break;
			}

			if(p==NULL)
				break;

			beg=(s[2]-'0')*10+(s[3]-'0');
			len=(s[4]-'0')*10+(s[5]-'0');
			if(len==0)
				len=strlen(p+beg);

			memcpy(d,p+beg,len);

			s+=6;
			d+=len;
		}
		else
		{
			*d=*s;
			s++;
			d++;
		}
	}

	return;
}

int comp_int(int val1,int val2)
{
	if(val1==val2 || val1==9999)
		return 1;
	else
		return 0;
}

void cal_time(char *s,struct timeval t1,struct timeval t2)
{
	if(t2.tv_usec<t1.tv_usec)
		fprintf(logfp,"CAL_TIME %s: %ld.%06ld  [%ld.%06ld - %ld.%06ld]\n",s,t2.tv_sec-t1.tv_sec-1,t2.tv_usec-t1.tv_usec+1000000,t1.tv_sec,t1.tv_usec,t2.tv_sec,t2.tv_usec);
	else
		fprintf(logfp,"CAL_TIME %s: %ld.%06ld  [%ld.%06ld - %ld.%06ld]\n",s,t2.tv_sec-t1.tv_sec,t2.tv_usec-t1.tv_usec,t1.tv_sec,t1.tv_usec,t2.tv_sec,t2.tv_usec);
	fflush(logfp);

	return;
}

void cal_time1(char *s,struct timeval t1,struct timeval t2)
{
	if(t2.tv_usec<t1.tv_usec)
		fprintf(logfp,"CAL_TIME %s: %ld.%06ld\n",s,t2.tv_sec-t1.tv_sec-1,t2.tv_usec-t1.tv_usec+1000000);
	else
		fprintf(logfp,"CAL_TIME %s: %ld.%06ld\n",s,t2.tv_sec-t1.tv_sec,t2.tv_usec-t1.tv_usec);
	fflush(logfp);
	return;
}

int time_diff(struct timeval t1,struct timeval t2){
	return (t2.tv_sec-t1.tv_sec)*1000 + (t2.tv_usec-t1.tv_usec) / 1000;
}

int time_diff1(struct timeval t1,struct timeval t2){
	return (t2.tv_sec-t1.tv_sec)*1000000 + (t2.tv_usec-t1.tv_usec);
}


#include <stdarg.h>
void file_log(char *log_file, char *fmt, ...)
{
        va_list args;
        FILE    *fp;
        char filename[128];
        char tmpstr[256];
        long t;
        struct tm *ttt;

        time(&t);
        ttt = localtime(&t);

        sprintf(filename, "%s/log/%s.%04d%02d%02d",
                getenv("HOME"),
                log_file,
                ttt->tm_year+1900, ttt->tm_mon+1, ttt->tm_mday);
        fp=fopen(filename,"a");
        if(fp == NULL) return;
        fprintf(fp,"%02d:%02d:%02d[%08d]",
                ttt->tm_hour,ttt->tm_min,ttt->tm_sec, getpid());
        va_start(args, fmt);
        vfprintf(fp,fmt,args);
        va_end(args);
        fprintf(fp, "\n");
        fclose(fp);
        return;
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


/*
 *
 * select trim(a.hlr_code),to_char(b.hlr_port),trim(a.ip_addr),to_char(a.comm_port),
   trim(b.gsm_ip),to_char(b.gsm_port),trim(b.gsm_user),trim(b.gsm_pswd),
   trim(b.order_cfg),nvl(trim(b.reply_cfg),' '),nvl(trim(b.query_cfg),' ')
 from offon.cmaincfginfo a, offon.chlrcfginfo b
where a.hlr_code=b.hlr_code
 */
int get_hlr_cfg(const char *hlrcode, const int hlrport, char *gsmip,
		int *gsmport,char *gsmuser, char *gsmpswd,
		char *order_fname, char *reply_fname, char *query_fname,
		char *mgr_servip, int *mgr_servport, char *mgr_flag, char *mgr_user,
		char *mgr_pswd, char *mgr_term)
{
	FILE *fp;
	char line[1024];
	char fname[128];
	char *fs[15], *p;
	int fc, find_flag, i;

	sprintf(fname, "%s/hlr.cfg", getenv("ETCDIR"));
	fp = fopen(fname, "r");
	find_flag = 0;
	while( fgets(line, sizeof(line)-1, fp) != NULL){
		fc = splitc(line, fs, 14, '|');
		if(fc != 14){
            printf("format err %d\n", fc);
			continue;
        }
		if(strcmp(fs[0], hlrcode) == 0 && atoi(fs[1])==hlrport){
			find_flag = 1;
			break;
		}
	}
	fclose(fp);
	if(find_flag == 0)
		return -1;

    if( (p = strchr(fs[13], '\n') ) != NULL)
        p[0] = 0;

	strcpy(mgr_servip, fs[2]);
    /*for(i=0; i<fc; i++)
        printf("%d=%s\n", i, fs[i]); */
    fflush(stdout);
	*mgr_servport = atoi(fs[3]);
	if(mgr_flag != NULL)
		*mgr_flag = 'N';
	strcpy(gsmip, fs[4]);
	*gsmport = atoi(fs[5]);
	strcpy(gsmuser, fs[6]);
	strcpy(gsmpswd, fs[7]);

	strcpy(order_fname, fs[8]);
	strcpy(reply_fname, fs[9]);
	strcpy(query_fname, fs[10]);

	strcpy(mgr_user, fs[11]);
	strcpy(mgr_pswd, fs[12]);
	strcpy(mgr_term, fs[13]);

	return 0;
}



void _print_bin(void *buf, int len)
{
	int i, j, l;
	unsigned char *ptr, c;
	char hex[100], asc[30], cp[10];
	l = len / 16 + (len % 16 > 0 ? 1 : 0);

	ptr = (unsigned char *)buf;
	for(i=0; i<l; i++){
		memset(hex, 0, sizeof(hex));
		memset(asc, 0, sizeof(asc));
		for(j=0; j<16; j++){
			if(i*16+j > len){
				strcat(hex, "   ");
				strcat(asc, " ");
			}else{
				c = ptr[i*16+j];
				sprintf(cp, "%02X ", c);
				strcat(hex, cp);
				if(c < 33 || c > 125)
					strcpy(cp, ".");
				else
					sprintf(cp, "%c", c);
				strcat(asc, cp);
			}
		}
		printf("%04X  %s [%s]\n", i*16, hex, asc);
	}
}




