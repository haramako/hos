//Author : Bakhtiyar Syed
//Institution : IIIT Hyderabad

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




int no=0;


/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{

  
  //Signal handler for Ctrl-C
  signal(SIGINT,signalhandler);

//Starts the shell
  start(argv);

  return EXIT_SUCCESS;
}
