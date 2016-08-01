#include "offon.h"

struct insert_soprcmd_data
{
	long streamid;
	char command_id[16];
	char phone_no[MSISDNLEN+1];
	char imsi_no[IMSILEN+1];
	char op_code[6];
	char command_code[6];
	char cmd_status;
	char phone_priority;
	char ss_info1[SSINFO1LEN+1];
	char ss_info2[SSINFO2LEN+1];
	char ss_info3[SSINFO3LEN+1];
	char req_time[16];
	char login_no[8];
	int  retn;
};

struct update_soprcmd_data
{
	long streamid;
	int  reply_code;
	char reply_info[200];
};

void list_all();
void fid_reset(char *hlrcode, long wfid, long rfid, long rpos);


int write_record(char *buf, int len, int dul);

void set_hlrcode(char *hlr_code);

long cmd_save_file(struct op_data_req *req, int retn);

void reply_save_file(struct cmdwait *ptr, int parse, char *info);

void order_save_file(struct cmdwait *ptr,int retn,int parse);

/*从数据文件中读取一条记录
返回值: 1 insert 记录, 结果保存在 struct op_data_req *req 中
		2 update 记录, 结果保存在 struct cmdwait *ptr 中
		0 无记录
*/
//int read_record_file(char *buf, int buflen, char *rp[][], int *rc, long *fpos);

/*检查文件入库是否完成, 否则offon_manager 需要等待, 避免指令丢失
已经完成则返回0, 否则返回 -1 */
int check_dbin();
