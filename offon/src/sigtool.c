#include <stdio.h>
#include <signal.h>

void
IgnoreAllSignal()
{
	struct sigaction act;

	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	sigaction(SIGHUP,&act,NULL);
	sigaction(SIGCHLD,&act,NULL); 
	sigaction(SIGQUIT,&act,NULL);
	
	return ;
}

	
