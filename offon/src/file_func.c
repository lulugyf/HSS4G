#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include "file_func.h"

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>


static long globle_sequence = 0L;
static char globle_hlrcode[4];

long cur_write_fileid = -1;
long cur_read_fileid = -1;

#define MAX_FILESIZE 1024*1024

/*设置hlrcode, 然后从目录中恢复文件序号
 * 文件的读取断点只由offon_fordb 处理, 将断点保存到文件中
 * */
void set_hlrcode(char *hlr_code){
	memcpy(globle_hlrcode, hlr_code, 3);
	globle_hlrcode[3] = 0;
}

char *get_cur_timestr(char *buf, int flag)
{
	time_t t;
	struct tm *tt;

	time(&t);
	tt = localtime(&t);
	if(flag == 1)
		sprintf(buf, "%04d%02d%02d",
				tt->tm_year+1900, tt->tm_mon+1, tt->tm_mday);
	else
		sprintf(buf, "%04d%02d%02d%02d%02d%02d",
				tt->tm_year+1900, tt->tm_mon+1, tt->tm_mday,
				tt->tm_hour, tt->tm_min, tt->tm_sec
				);
	return buf;
}

char *get_data_file(char *filename, char *hlr, char *date){
	if(date == NULL || date[0] == 0){
		char dd[9];
		get_cur_timestr(dd, 1);
		sprintf(filename, "%s/databuf/%s.%s", getenv("HOME"), hlr, dd);
		if(date != NULL)
			strcpy(date, dd);
		return filename;
	}
	sprintf(filename, "%s/databuf/%s.%s", getenv("HOME"), hlr, date);
	return filename;
}

char *get_data_file_dul(char *filename, char *hlr, char *date){
	if(date == NULL || date[0] == 0){
		char dd[9];
		get_cur_timestr(dd, 1);
		sprintf(filename, "%s/databuf/syn_%s.%s", getenv("HOME"), hlr, dd);
		if(date != NULL)
			strcpy(date, dd);
		return filename;
	}
	sprintf(filename, "%s/databuf/syn_%s.%s", getenv("HOME"), hlr, date);
	return filename;
}

/*从断点文件中读取断点*/
int get_file_pos(char *hlr_code, char *date, long *rpos)
{
	FILE *fp;
	char fname[128];

	sprintf(fname, "%s/databuf/.%s.pos1",
			getenv("HOME"), hlr_code);
	fp = fopen(fname, "r");
	if(fp == NULL){
		get_cur_timestr(date, 1);
		*rpos = 0;
		return 0;
	}
	fscanf(fp, "%s %ld", date, rpos);
	fclose(fp);
	return 0;
}

/*检查文件入库是否完成, 否则 offon_manager 需要等待, 避免指令丢失
已经完成则返回0, 否则返回 -1 */
int check_indb(){
	char filename[128], f1[128];
	struct stat st;
	long rfid, rpos;
	char date[9], date1[9];

	date1[0] = 0;
	get_data_file(filename, globle_hlrcode, date1);
	if(stat(filename, &st) == -1)
		return 0;
	if(get_file_pos(globle_hlrcode, date, &rpos) != 0)
		return -1;
	if(strcmp(date, date1) != 0 || st.st_size > rpos)
		return -1;
	else
		return 0;
}

/*产生 stream_id */
long seq_nextval(){
	if(globle_sequence == 0L){
		struct timeval tt1;
		printf("取初始sequence\n");
		gettimeofday(&tt1, NULL);
		globle_sequence = tt1.tv_sec * 1000L + tt1.tv_usec;
	}
	return ++globle_sequence;
}

int write_record_dul(char *hlr_code, char *buf, int len){
	FILE *wfp = NULL;
	char filename[128];

	get_data_file_dul(filename, hlr_code, NULL);
	wfp = fopen(filename, "a");
	fwrite(buf, len, 1, wfp);
	fclose(wfp);
	return 1;
}

int write_record(char *buf, int len, int dul){
	FILE *wfp = NULL;
	char filename[128];

	get_data_file(filename, globle_hlrcode, NULL);
	wfp = fopen(filename, "a");
	fwrite(buf, len, 1, wfp);
	fclose(wfp);
    //if(buf[0] != 'S' && dul == 1){
    //	write_record_dul(globle_hlrcode, buf, len);
    //}
	return 1;
}

/*保存数据到 soprcmdhbc */
long cmd_save_file(struct op_data_req *req, int retn)
{
	char	tmpsql[512];
	long	stream;
	int len;
	char    buf[1024];
	long id;

	stream = seq_nextval();
	sprintf(buf, "I%ld|%s|%s|%s|%s|%s|%c|%c|%s|%s|%s|%s|%04d|%s\n",
		stream, req->command_id, req->phone_no, req->imsi_no, req->op_code,
		req->command_code, req->cmd_status, req->phone_priority, req->ss_info1, req->ss_info2,
		 req->ss_info3, req->req_time, retn, req->login_no
		);

	len = strlen(buf);
	write_record(buf, len, 1);
	return stream;
}

/*HLR执行指令完成后将结果 update 到 soprcmdhbc 表*/
void reply_save_file(struct cmdwait *ptr,int parse,char *info)
{
	char buf[1024], replyinfo[200];
	int len;
	char timebuf[50];

	if(info && strlen(info))
		strcpy(replyinfo,info);
	else
		strcpy(replyinfo,"null");
	replyinfo[198]=0;
	sprintf(buf, "U%ld|%04d|%s|%s\n", ptr->streamid, parse, get_cur_timestr(timebuf, 0), replyinfo);
	len = strlen(buf);
	write_record(buf, len, 1);
	return;
}

/*insert sSubCmd */
void order_save_file(struct cmdwait *ptr, int retn, int parse)
{
	char buf[1024];
	int len;

	ptr->cmdid[20] = 0;
	ptr->msisdn[15] = 0;
	ptr->imsi[20] = 0;
	ptr->ss_info1[15] = 0;
	ptr->ss_info2[20] = 0;
	ptr->ss_info3[120] = 0;

	sprintf(buf, "S%ld|%s|%03d|%d|%d|%s|%s|%s|%s|%s|%04d|%04d\n",
		ptr->streamid, ptr->cmdid, ptr->cmdcode, ptr->orderhead.next->ordercode, ptr->orderhead.next->serial,
		ptr->msisdn, ptr->imsi, ptr->ss_info1, ptr->ss_info2,ptr->ss_info3,
		retn, parse);
	len = strlen(buf);
	write_record(buf, len, 1);
	return;
}
