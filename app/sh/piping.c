#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include "shell.h"



int piping (char ** args){

  int status;
  int p[2];

  //pipe(p);
  int q[2];

int i=0;
int j =0;
int k =0;
  int flag1;
  int end=0;
  int fd1, fd2, fd3;
  char *sym[100];

  int m = 0;
  int count = 0;

  char * r = args[0];
  pid_t pid;
  //Count the no. of command line arguments in piping
  while(r!=NULL){
    if (strncmp(args[m],"|",1)==0){
    count++;
    }
    m++;
    r = args[m];
  }
  count++;

  int last =count -1 ;
//  printf("%d\n",count );

//The main loop for piping
while(args[j]!=NULL &&  end!=1){

  k = 0;

  char * d = args[j];

  while((strncmp(d,"|",1)!=0)){
    //if(strncmp(d,"|",1)!=0)
    sym[k]=d;
    //printf("%s\n",sym[k] );
    j++;
    d = args[j];

    if(d == NULL){
      end = 1;
      k++;
      break;
    }
    k++;
  }

sym[k] = NULL;
j++;

//char ** sym1;
//sym1 = redirect(sym);

  if (i%2 == 0)
    pipe(q);
  else
    pipe(p);

  pid = fork();

  //Error on being unable to fork the process
  if(pid < 0)
    perror("Unable to fork the process");


  //In the child process
  if (pid == 0){

    char ** sym1;
    sym1 = redirect(sym);


      if (i==0){
        dup2(q[1],1);
      //  close(q[0]);
      }
      else if (i==last){
        if(count%2==0){
          dup2(q[0],0);
          //close(q[1]);
        }
        else{
           dup2(p[0],0);
          //close(p[1]);
        }
      }

      else{
        if(i%2==0){
          dup2(p[0],0);
          dup2(q[1],1);
        }
        else{
          dup2(q[0],0);
          dup2(p[1],1);
        }
      }
      if (execvp(sym1[0],sym1)==-1){
        kill(getpid(),SIGTERM);
      }
    }

      //In the parent process to close all file descriptors;


          if(i==0){
            close(q[1]);
          }

          else if(i==last){
              if(i%2==0){
                close(q[0]);
              }
              else{
                close(p[0]);
              }
          }

          else{
            if(i%2==0){
              close(p[0]);
              close(q[1]);
            }
            else{
              close(q[0]);
              close(p[1]);
            }
          }

        waitpid(pid,NULL,0);
        i++;


      }
      return 1;
    }
