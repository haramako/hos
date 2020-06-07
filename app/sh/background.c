#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include "shell.h"



void sigh(int signum)
{
	pid_t wpid;
  int procstatus;
	int status;
	wpid=waitpid(-1,&status,WNOHANG);
	if(wpid>0	&& (WIFEXITED(status)==0))
	{
			printf("Process with pid %d exited normally\n",wpid);
			printprompt();
	}
	if(wpid >0 && WIFSIGNALED(status)==0)
		{
			printf("Process with pid %d exited due to a user-defined signal\n",wpid);
			printprompt();
		}

}
