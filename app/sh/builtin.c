#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include "shell.h"


pid_t pid3;
int status;



//  Builtin function implementations.
/**
   @brief Builtin command: pwd. Returns the present working directory.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_pwd(char ** args)
{
    if (args[0]== NULL)
      return 1;

    else
    {
      char * tem;
      char array[1000];
      if ((tem = getcwd(array, sizeof(array)))!=NULL)
        printf("%s\n",array );
      else
        fprintf(stderr, "Error: Unable to access directory.\n" );
    }
    return 1;
}


/**
   @brief Builtin command: echo
   @param args List of args.  Given by user
   @return Always returns 1, to continue executing.
 */
int lsh_echo(char **args)
{
  //Empty argument prints new line
  if(args[1]==NULL)
    {
      printf("\n" );
      return 1;
    }

    pid3 = fork();
    //Child process
    if(pid3 <0)
     {
       perror("SHELL");
     }
    else if(pid3==0)
    {
      args = redirect(args);
      int i;
      i=1;
      while (args[i]!=NULL){
        write(1, args[i], strlen(args[i]));

        i++;
        }
      write(1,"\n", 1);

    exit(1) ;
    }

      waitpid(pid3,NULL,0);



  return 1;
}


/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    chdir(curpath);
  } else {

  if (strncmp(args[1],"~", 1)==0)
  {     //printf("yo found!\n" );
        strcpy(args[1],curpath);
        //printf("args[1] is %s\n",args[1] );
      }

     int dir = chdir(args[1]);
      if (dir!=0) {
      perror("lsh");
    }
  }
  return 1;
}


/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  if (strcmp(args[0],"quit")==0)
    return 0;
  else
    return 1;
}




int lsh_jobs(char **args)
{
  int loop=1;
  //printf("no is %d\n",no );
  while(loop<=no){

    if(a[loop]!=NULL){
    printf("[%d] ", loop );
    printf("%s ", (a[loop])->arr );
    printf("[%d]\n", (a[loop])->num );
    //printf("\n");
  }
    loop++;
}

return 1;
}



int lsh_kjob(char **args)
{
  if (args[1]==NULL || args[2]==NULL)
    perror("Usage: kjob [process no.] [signal no.]");

  int signo = atoi (args[2]);
  int procno =atoi  (args[1]);

  //Fix segmentation fault
  if(a[procno]==NULL){
    printf("The process does not exist.\n" );
    return 1;
  }
  int id = (a[procno])->num;
  a[procno]=NULL;
  //printf("procno is %d\n",procno );
  //printf("signo is %d\n",signo );
kill(id, signo);


  return 1;
}



int lsh_killallbg(char **args)
{

  int loop=1;
  while(loop<=no)
  {
    if(a[loop]!=NULL)
    {
      kill((a[loop])->num,9);
      a[loop]=NULL;
    }
    loop++;
  }

  return 1;

}

int lsh_fg(char **args)
{
  int loop;

  if(args[1]==NULL){
    printf("Usage: fg [process no.]\n");
  }
  int procno =atoi(args[1]);
  if(a[procno]==NULL){
    printf("ERROR: No such background process number exists.\n");
    return 1;
  }


  kill((a[procno]->num),18);
  a[procno]=NULL;
  return 1;
}
