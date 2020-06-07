
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



void signalhandler(int sig)
{
  //printf("\n" );
//signal(SIGINT,signalhandler);
  //printf("CAught you Ctrl-C\n" );
  //printprompt();

    //printprompt();


  printf("\nCtrl-C signal received\n");

   //signal(SIGINT, signalhandler);
   //return;
//  main();

}
