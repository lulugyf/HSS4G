#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_ITEM 300
#define MAX_MIDFILE_SIZE 2048000

#include "dul_util.h"

struct hlrdul{
	int item_count;
    struct hlrdul_item items[MAX_ITEM];
};

struct hlrdul *local_ptr = NULL;
struct hlrdul_item *local_item = NULL;

char *dul_getshm(int type){
	char filename[100];
	key_t key;
	int shmid = 0;
	char *ptr = NULL;
	char *pp;
	int *p1;
	struct hlrdul *hp;
    int create_flag = 0;

    if(local_ptr != NULL)
        return (char *)local_ptr;

	sprintf(filename, "%s/shmkey.ctl", getenv("ETCDIR"));
	key = ftok(filename, 9);
    if(key < 0){
        printf("can not get key from [%s], please check that file\n", filename);
        return NULL;
    }
        shmid = shmget(key, sizeof(struct hlrdul), 0);
    if(shmid < 0){
        if(type == 1){
            shmid = shmget(key, sizeof(struct hlrdul), IPC_CREAT|0666);
            create_flag = 1;
        }
        if(shmid < 0){
            printf("can not get shm id\n");
            return NULL;
        }
    }

	ptr = (char *)shmat(shmid, NULL, 0);
	if(ptr == NULL){
		printf("shmat failed %d\n", errno);
		exit(1);
	}

    if(create_flag == 1){
	    hp = (struct hlrdul *)ptr;
        memset(ptr, 0, sizeof(struct hlrdul));
        hp->item_count = 0;
    }

    local_ptr = (struct hlrdul *)ptr;
	return ptr;
}

int dul_add(char *hlr_code, int idx, char *dip, int dpt, int rpt, char status)
{
    struct hlrdul_item *p;

    dul_getshm(1);
    p = (struct hlrdul_item *)(&local_ptr->items[idx]);
    local_ptr->item_count = idx+1;

    strcpy(p->hlrcode, hlr_code);
    strcpy(p->dip, dip);
    p->dpt = dpt;
    p->rpt = rpt;
    p->status = status;
    return 0;
}

int dul_set(char *hlr_code, char *dip, int dpt, int rpt, char status)
{
    struct hlrdul_item *p1 = NULL;
    int i;
    char *ptr;

    ptr = dul_getshm(1);
    if(ptr == NULL){
        printf("can not get shm addr, %d\n", errno);
        return -1;
    }
    for(i=0; i<local_ptr->item_count; i++){
        p1 = (struct hlrdul_item *)&(local_ptr->items[i]);
        if(strcmp(p1->hlrcode, hlr_code) == 0)
            break;
        p1 = NULL;
    }
    if(p1 == NULL){
        p1 = (struct hlrdul_item *)&(local_ptr->items[local_ptr->item_count ++]);
        strcpy(p1->hlrcode, hlr_code);
    }

    strcpy(p1->dip, dip);
    p1->dpt = dpt;
    p1->rpt = rpt;
    p1->status = status;
    return 0;
}

int dul_find(const char *hlr_code)
{
    struct hlrdul_item *p = NULL;
    int i;

    dul_getshm(0);
    for(i=0; i<local_ptr->item_count; i++){
        p = (struct hlrdul_item *)(&local_ptr->items[i]);
        if(strcmp(p->hlrcode, hlr_code) == 0){
            local_item = p;
            return 1;
        }
    }
    return 0;
}

int dul_get(char *dip, int *dpt, int *rpt, char *status)
{
    if(local_item == NULL)
        return 0;
    if(dip != NULL)
        strcpy(dip, local_item->dip);
    if(dpt != NULL)
        *dpt = local_item->dpt;
    if(rpt != NULL)
        *rpt = local_item->rpt;
    if(status != NULL)
        *status = local_item->status;
    return 1;
}

int dul_get_rcv_q_len(int **t){
    if(local_item == NULL)
        return 0;
    *t = &(local_item->rcv_q_len);
    return 1;
}

int dul_get_status(char **t){
    if(local_item == NULL)
        return 0;
    *t = &(local_item->status);
    return 1;
}


int dul_setl(char *dip, int dpt, int rpt, char status)
{
    if(local_item == NULL)
        return 0;
    if(dip != NULL)
        strcpy(local_item->dip, dip);
    if(dpt != NULL)
        local_item->dpt =  dpt;
    if(rpt != NULL)
        local_item->rpt = rpt;
    if(status != NULL)
        local_item->status = status;
    return 1;
}

int dul_listall(FILE *fpo)
{
    struct hlrdul_item *p;
    int i;
    char *p1;

    p1 = dul_getshm(0);
    if(p1 == NULL){
        fprintf(fpo, "not initial\n");
        return 1;
    }
    fprintf(fpo, "begin print dul_cfg\n");
    fprintf(fpo, "hlr\tdul_ip\tdul_pt\tdul_rpt\tstatus\trcv_q_len\twfidx\trfidx\trfpos\n");
    for(i=0; i<local_ptr->item_count; i++){
        p = (struct hlrdul_item *)(&local_ptr->items[i]);
        fprintf(fpo, "%s\t%s\t%d\t%d\t%c\t%d\t%d\t%d\t%d\n",
            p->hlrcode, p->dip, p->dpt, p->rpt, p->status, p->rcv_q_len,
            p->wfile_idx, p->rfile_idx, p->rfile_pos);
    }
    fprintf(fpo, "end print dul_cfg\n");
    return 0;
}


/*文件作为同步数据的中间媒介部分*/

/*只修改不为-1的项*/
int dul_set_fstat(const char *hlr_code, int widx, int ridx, int rpos)
{
    dul_find(hlr_code);
    if(local_item == NULL)
        return -1;
    if(widx != -1)
        local_item->wfile_idx = widx;
    if(ridx != -1)
        local_item->rfile_idx = ridx;
    if(rpos != -1)
        local_item->rfile_pos = rpos;
    return 0;
}

int dul_set_fstat_by_idx(const int idx, int widx, int ridx, int rpos)
{
    struct hlrdul_item *p;
    if( idx < 0 || idx > MAX_ITEM)
        return -2;

    p = &local_ptr->items[idx];

    if(widx != -1)
        p->wfile_idx = widx;
    if(ridx != -1)
        p->rfile_idx = ridx;
    if(rpos != -1)
        p->rfile_pos = rpos;
    return 0;
}


int dul_get_fstat(const char *hlr_code, int *widx, int *ridx, int *rpos)
{
    dul_find(hlr_code);
    if(local_item == NULL)
        return -1;
    if(widx != NULL)
        *widx = local_item->wfile_idx;
    if(ridx != NULL)
        *ridx = local_item->rfile_idx;
    if(rpos != NULL)
        *rpos = local_item->rfile_pos;
    return 0;
}


int dul_sem_getid(char *keyfile, int proj_id)
{
    key_t semkey;
    int myid;

    union semun {
        int val;
        struct semid_ds *buf;
        ushort * array;
    } argument;

    semkey = ftok(keyfile, proj_id);
    if(semkey == -1){
        printf("ftok failed [%s] errno[%d]\n", keyfile, errno);
        return -1;
    }

    argument.val = 1;
    myid = semget(semkey, 1, 0);
    if(myid < 0 && errno == 2)
        myid = semget(semkey, 1, IPC_CREAT|0660);
    if(myid == -1){
        printf("can not create sem for[%s], errno[%d]\n", keyfile, errno);
        return -2;
    }
    if( semctl(myid, 0, SETVAL, argument) < 0)
    {
        printf("Cannot set semaphore value. for [%s], errno[%d]\n", keyfile, errno);
    }
    else
    {
        printf("Semaphore %s initialized.\n", keyfile);
    }

    return myid;
}

/*信号量的锁定和解锁 sem*/
int dul_sem_lock(int op, int semid){
    static struct sembuf op_open={ 1,-1,0  };
    static struct sembuf op_lock={ 0,-1,0};
    static struct sembuf op_unlock={ 0,1,0};
    if(semid < 0)
        return -3;

    if(op == 1){ /*lock*/
        while( semop(semid, &op_lock, 1) < 0){
            if(errno==EINTR)
                continue;
            printf("sem op_lock failed! errno[%d]\n", errno);
            return -1;
        }
    }else if(op == 0){ /*unlock*/
        while( semop(semid, &op_unlock, 1) <0){
            if(errno==EINTR)
                continue;
            printf("sem op_unlock error! errno[%d]\n", errno);
            return -2;
        }
    }
    return 0;
}


int dul_check_files(char *hlrcode, int *pmsid, int *pmxid)
{
	/*文件号是重新计算的, 需要搜索目录中的文件, 确定上次的断点*/
	DIR *ddp;
	struct dirent *dp;
	char fname[128], fpre[30];
	int msid, mxid, x, len_fpre;
	FILE *fp;
	struct stat st;

	msid = -1;
	sprintf(fname, "%s/databuf", getenv("HOME"));
	ddp = opendir(fname);
	sprintf(fpre, "syn_%s.", hlrcode);
	len_fpre = strlen(fpre);
	while( (dp = readdir(ddp) ) != NULL){
		if(memcmp(dp->d_name, fpre, len_fpre) != 0)
			continue;
		x = atoi(dp->d_name+len_fpre);
		if( msid == -1)
			msid = mxid = x;
		if( x < msid ) msid = x;
		else if(x > mxid) mxid = x;
	}
	closedir(ddp);
	if( x >= 0 ){
		*pmxid = mxid;
		*pmsid = msid;
	}
}
/*保存备份数据*/
int dul_cur_rfid = -1;
int dul_cur_wfid = -1;
int dul_save_bak(const char *hlr_code, char *data)
{
    FILE *fp;
    char dfile[128];
    long fpos;
    int  cidx;

    if(dul_cur_wfid == -1){
    	dul_check_files(hlr_code, &dul_cur_rfid, &dul_cur_wfid);
    }

    sprintf(dfile, "%s/databuf/syn_%s.%d", getenv("HOME"), hlr_code, dul_cur_wfid);
    fp = fopen(dfile, "a");
    if(fp == NULL)
        return -3;
    if( (fpos = ftell(fp)) >= MAX_MIDFILE_SIZE){
    	fclose(fp);
    	sprintf(dfile, "%s/databuf/syn_%s.%d", getenv("HOME"), hlr_code, ++dul_cur_wfid);
    	fp = fopen(dfile, "a");
    }
    fprintf(fp, "%s", data);
    fclose(fp);
    return 0;
}
