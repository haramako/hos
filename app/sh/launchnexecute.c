#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <pwd.h>
#include "shell.h"


//extern int no;
//Declaring the builtin functions and pointer to function
char *builtin_str[]= { "cd", "pwd", "echo", "quit", "jobs", "kjob" , "killallbg", "fg"};
int (*builtin_func[]) (char **) = { &lsh_cd, &lsh_pwd, &lsh_echo, &lsh_exit, &lsh_jobs, &lsh_kjob, &lsh_killallbg, &lsh_fg};

//Return the no. of builtin
int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}




/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{

  pid_t pid;
  int status;
  char ampersand[2]="&";



  int i=0;
  char * last = NULL;

  if(args[1]==NULL)
    goto LABEL;
  char * ka = args[1];
  while (1)
  {
    last = args[i];
      i++;
    ka = args[i];
    if (ka==NULL)
      break;
  }

if (last!=NULL && i!=0 && strncmp(last,ampersand, 1) ==0)
  args[i-1]=NULL;

LABEL:


 pid = fork();

  //Parent with & background
  if (pid!=0 && last!=NULL && strncmp(last,ampersand, 1) ==0){


    signal(SIGCHLD, sigh);
    no++;
    a[no] = (process *) malloc(sizeof(process));
    (a[no])->num = pid;
    strcpy((a[no])->arr, args[0]);
    return 1;
  }

  //Child Process
  else if (pid == 0) {

    //Call function to redirect
    args = redirect(args);
    int ret=execvp(args[0], args);
     if (ret== -1) {
      perror("SHELL:");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("SHELL:");
  }
    else {
      // Parent process
      do {
        waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}




/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    printf("\n");
    return 1;
  }

  char * w = args[0];
  int y =0;
  while(w!=NULL){
    if(strncmp(w,"|",1)==0){
      return piping(args);
    }
    y++;
    w = args[y];
  }


  int count = lsh_num_builtins();
  int len = strlen(args[0]);
  for (i = 0; i < count; i++) {
    if (strncmp(args[0], builtin_str[i], len) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}
