#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include "shell.h"




void printprompt(){

  struct passwd * myname=NULL;
  myname = getpwuid(getuid());

  //Get the hostname
  char hostname[30];
  gethostname(hostname, 29);

  //Getting the current working dir
  char * home;
  char * homenext;
  char prespath[1000];
  home = getcwd(curpath, sizeof(curpath));
  int len = strlen(curpath);
  //printf("len is %d\n",len );
  //printf("curpat is %s\n",curpath );




//Print type
//printf("\n" );
printf("<");
printf("%s", myname->pw_name);
printf("@");
printf("%s", hostname);
printf(":");
homenext= getcwd(prespath, sizeof(prespath));
//printf("prespath is %s\n", prespath);
 if(strlen(prespath)<(size_t)len)
    printf("%s",prespath );

else if (strncmp(curpath,prespath,strlen(prespath))!=0)
  {
	  if(strlen(prespath)>(size_t)len)
    {
      printf("~");
      printf ("%s", prespath+ len);
    }
    else
    {
      printf("%s",prespath );
    }
  }
else
  printf("~");
printf(">");

return ;
}
