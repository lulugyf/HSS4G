#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>

/***
void IgnoreAllSignal()
{
  struct sigaction act;

  act.sa_handler = SIG_IGN;
  sigemptyset(&act.sa_mask);
  act.sa_flags=0;
  sigaction(SIGHUP, &act,NULL);
  sigaction(SIGCHLD,&act,NULL); 
  sigaction(SIGQUIT,&act,NULL);
  
  return ;
}
***/

int  GenDaemon() {
        int childpid,fd;
        int flag;

        if (getppid()==1) {
                umask(0);
                return 0;
        }
		signal(SIGALRM, SIG_IGN);
        if ((childpid = fork())<0 ) {
                exit(-1);
        }else{
			if (childpid > 0)
				exit(0);
        }
        /*for (fd=1; fd<=3;fd++)
                close (fd);
	
        chdir("/");*/
        umask(0);

		/*IgnoreAllSignal();*/
        return 0;
}

char *delespace(char *source)
{
  int i,n;

  i=strlen(source);
  while(i > 0)
  {
	i--;
    if(source[i]==' ' || source[i]=='	')
      source[i]=0x0;
	else
		break;
  }
  return source;
}


void checkpid(int pid){
        const int pidcount = 50; /*最大允许连接数*/
        static int cpid[50], init=0;
        int i, ret;
        int flag = 0, ccount = 0;

        if(init == 0){
                memset(cpid, 0, sizeof(cpid));
                init = 1;
        }

        while(flag == 0){
                for(i=0; i<pidcount; i++){
                        if(cpid[i] == 0){
                                if(flag == 0){
                                        cpid[i] = pid;
                                        flag = 1;
                                }
                                continue;
                        }
                        ret=waitpid(cpid[i], NULL, WNOHANG);
                        if ((ret==cpid[i])||(ret==-1&&errno==10)){
                                cpid[i] = 0;
                        }
                }
                if(flag == 0){ /*已经达到最大数量, 休息一下, 等待释放*/
                        sleep(5);
                }
        }

}

