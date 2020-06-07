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



void start(char **argv)

{




//passwd = getpwuid ( getuid());
int status;
char **args;
char *line;


do {

  printprompt();

  lsh_read_line(); //Calling th function to read the line

  int j=0;
  while(sentences[j]!=NULL)
  {
    args = lsh_split_line(sentences[j]); // Split line
    status = lsh_execute(args); //Execute the commands
    j++;
  }

  //free(line);
  free(args);
} while (status);


return;
}
