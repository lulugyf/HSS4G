#include <stdio.h>


int splitc(char *string, char *fields[], int  nfields, char sep )
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

int main(int argc, char *argv[])
{
    char hlrcode[4];
    int hlrport;
    char mgr_user[20], mgr_pswd[20], mgr_term[10];
    char order_fname[256],reply_fname[256],query_fname[256];
    char srvip[30];
    int commport;

    char				gsmip[32];
    int					gsmport;
    char				gsmuser[32];
    char				gsmpswd[32];
    int i;


    strcpy(hlrcode, "hab");
    hlrport=5;
    i = get_hlr_cfg(hlrcode, hlrport, gsmip, &gsmport, gsmuser, gsmpswd,
            order_fname,reply_fname,query_fname, srvip, &commport, NULL,
            mgr_user, mgr_pswd, mgr_term);

    if(i == 0){
        printf("Hello, world  %s\n", mgr_user);
    }else{
        printf("failed, %d\n", i);
    }
    
    return 0;
}
