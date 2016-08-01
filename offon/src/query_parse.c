#include "offon.h"

#define		Q_VALUES		1
#define		Q_COMPARE		2
#define		Q_STRING		3

extern FILE *logfp;

struct query_para
{
  char		name[64];
  int		station;
  char		key[128];
  int		type;		/* VALUES & COMPARE */

  int		val_src_beg;
  int		val_src_len;

  char		def_flg;
  char		currect;

  struct query_para	*next;
};

struct query_para qhead,*q;

char *del_space(char *s)
{
  int i,len;
  char *ps,*pd;

  if(s==NULL)
    return NULL;

  len=strlen(s);
  if(len==0)
    return s;

  for(i=len-1;i>=0;i--)
  {
    if(s[i]=='%')
    {
      s[i]=0x0;
      break;
    }

    if(s[i]==' ' || s[i]=='	' || s[i]=='\r' || s[i]=='\n')
      s[i]=0x0;
    else
      break;
  }

  len=strlen(s);
  if(len==0)
    return s;

  for(i=0;i<len;i++)
  {
    if(s[i]==' ' || s[i]=='	' || s[i]=='\r' || s[i]=='\n')
      s[i]=0x0;
    else
      break;
  }

  ps=s+i;
  pd=s;

  while(*ps)
  {
    if(*ps=='\\')
    {
      ps++;
      switch(*ps)
      {
      case 'r':
        *ps='\r';
        break;
      case 'n':
        *ps='\n';
        break;
      case 't':
        *ps='\t';
        break;
      default:
        break;
      }
    }

    *pd++=*ps++;
  }

  *pd=0x0;

  return s;
}

int get_query_cfg(char *fname)
{
  FILE *fp;

  char str[256],*info,*p;
  int count,flag;

  fp=fopen(fname,"r");
  if(fp==NULL)
  {
    fprintf(logfp,"can't open %s for read[%d]\n",fname,errno);
	fflush(logfp);

    return -1;
  }

  memset(&qhead,0x0,sizeof(qhead));
  q=NULL;
  count=0;
  flag=0;
  memset(str,0x0,sizeof(str));
  while(fgets(str,sizeof(str),fp))
  {
    count++;

    info=del_space(str);
    if(info[0]=='#' || strlen(info)==0)
    {
      memset(str,0x0,sizeof(str));
      continue;
    }
    else if(info[0]=='[')
    {
      if(q!=NULL)
      {
        if(q->type==Q_VALUES)
        {
          if(q->val_src_len==0)
            break;
        }
        else if(q->type==Q_COMPARE)
        {
          if(q->currect==0x0)
            break;
        }
        else if(q->type!=Q_STRING)
          break;
      }

      p=strchr(info,']');
      if(p==NULL)
        break;

      *p=0x0;

      q=(struct query_para *)malloc(sizeof(struct query_para)); 
      if(q==NULL)
      {
        fprintf(logfp,"malloc(%d) failed[%d]\n",sizeof(struct query_para),errno);
		fflush(logfp);

        break;
      }

      memset(q,0x0,sizeof(struct query_para));
      strcpy(q->name,info+1);

      q->next=qhead.next;
      qhead.next=q;
    }
    else if(strncmp(info,"station=",8)==0)
    {
      if(q==NULL || q->station>0)
        break;

      q->station=atoi(info+8)+1;
    }
    else if(strncmp(info,"key=",4)==0)
    {
      if(q==NULL || q->station==0 || strlen(q->key)>0)
        break;

      strcpy(q->key,info+4);
    }
    else if(strncmp(info,"type=",5)==0)
    {
      if(q==NULL || strlen(q->key)==0 || q->type!=0)
        break;

      if(strcmp(info+5,"VALUES") && strcmp(info+5,"COMPARE") && strcmp(info+5,"STRING"))
        break;

      if(strcmp(info+5,"VALUES")==0)
        q->type=Q_VALUES;
      else if(strcmp(info+5,"COMPARE")==0)
        q->type=Q_COMPARE;
      else
        q->type=Q_STRING;
    }
    else if(strncmp(info,"src_beg=",8)==0)
    {
      if(q==NULL || q->type!=Q_VALUES || q->val_src_beg>0)
        break;

      q->val_src_beg=atoi(info+8);
    }
    else if(strncmp(info,"src_len=",8)==0)
    {
      if(q==NULL || q->type!=Q_VALUES || q->val_src_beg==0 || q->val_src_len>0)
        break;

      q->val_src_len=atoi(info+8);
    }
    else if(strncmp(info,"default=",8)==0)
    {
      if(q==NULL || q->type!=Q_COMPARE || q->def_flg!=0)
        break;

      q->def_flg=info[8];
    }
    else if(strncmp(info,"currect=",8)==0)
    {
      if(q==NULL || q->type!=Q_COMPARE || q->def_flg==0 || q->currect!=0)
        break;

      q->currect=info[8];
    }
    else
      break;

    memset(str,0x0,sizeof(str));
  }

  if(feof(fp))
    count=0;

  fclose(fp);

  return count;
}

void cal_query_cfg(char *s,char *d)
{
  char *p;
  int i;

  q=qhead.next;
  while(q)
  {
printf("KEY===%s[",q->key);
for(i=0;i<strlen(q->key);i++)
  printf("%02x ",q->key[i]);
printf("]\n");
printf("STATION===%d\n",q->station);

    if(q->type==Q_STRING)
    {
      memcpy(d+q->station-1,q->key,strlen(q->key));

      q=q->next;
      continue;
    }

    p=strstr(s,q->key);
    if(p)
    {
printf("OK\n");
      if(q->type==Q_VALUES)
      {
        memcpy(d+q->station-1,p+q->val_src_beg,q->val_src_len);
      }
      else if(q->type==Q_COMPARE)
      {
        d[q->station-1]=q->currect;
      }
    }
    else if(q->type==Q_COMPARE)
    {
printf("NO[%02x]\n",d[q->station-1]);
      if(d[q->station-1]==0x0 || d[q->station-1]==0x20)
        d[q->station-1]=q->def_flg;
    }

    q=q->next;
  }

  return;
}

void disp_query_cfg()
{
  int count;

  count=0;
  fprintf(logfp,"QUERY INFO:\n");
  q=qhead.next;
  while(q)
  {
    count++;
    fprintf(logfp,"  NODE_%02d:\n",count);
    fprintf(logfp,"    name=====%s~\n",q->name);
    fprintf(logfp,"    station==%d\n",q->station);
    fprintf(logfp,"    key======%s~\n",q->key);
    if(q->type==Q_VALUES)
    {
      fprintf(logfp,"    type=====values\n");
      fprintf(logfp,"    src_beg==%d\n",q->val_src_beg);
      fprintf(logfp,"    src_len==%d\n",q->val_src_len);
    }
    else if(q->type==Q_COMPARE)
    {
      fprintf(logfp,"    type=====compare\n");
      fprintf(logfp,"    default==%c\n",q->def_flg);
      fprintf(logfp,"    currect==%c\n",q->currect);
    }
    else if(q->type==Q_STRING)
	{
      fprintf(logfp,"    type=====string\n");
	}
	else
      fprintf(logfp,"    type=====unknow\n");

    q=q->next;
  }

  fprintf(logfp,"END\n");
  fflush(logfp);

  return;
}

/*****************************************************
main()
{
  printf("get_query_cfg==%d\n",get_query_cfg("a"));
  disp_query_cfg();
}
*****************************************************/
