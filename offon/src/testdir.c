#include "h.h"

int
testdir(pathname)
char *pathname;
{
	struct stat	buf;

	memset(&buf,0x0,sizeof(buf));

	if(lstat(pathname,&buf)<0)
	{
		printf("lstat() failed[%d]!\n",errno);
		return(-1);
	}

	if(S_ISDIR(buf.st_mode))
	{
		printf("%s is DIR.\n",pathname);

		if(access(pathname,R_OK)<0)
		{
			printf("can't read it\n");
			return(2);
		}
		else
			printf("read OK!\n");

		if(access(pathname,W_OK)<0)
		{
			printf("can't write it\n");
			return(3);
		}
		else
			printf("write OK!\n");

		return(0);
	}
	else
	{
		printf("%s is not DIR.\n",pathname);

		return(10);
	}
}
