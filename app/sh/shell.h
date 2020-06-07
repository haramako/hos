#ifndef HEADER
#define HEADER


#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define LSH_RL_BUFSIZE 1024


void start(char **argv);
void signalhandler(int sig);
void printprompt();

char curpath[1000]; //Home directory, globally declared
char *sentences[25];
char *fargv[100];//Storing strings delimited by ;



extern int no;
typedef struct process{
  int num;
  char arr[60];
}process;
process * a[200];

/*
  List of builtin commands, followed by their corresponding functions.
 */
/*
  Function Declarations for builtin shell commands:
 */




int lsh_cd(char **args);
int lsh_pwd(char **args);
int lsh_echo(char ** args);
int lsh_exit(char **args);
int lsh_jobs(char ** args);
int lsh_kjob(char **args);
int lsh_killallbg(char **args);
int lsh_fg(char **args);

void lsh_read_line();
int piping (char **args);
char ** redirect(char** args);
int lsh_launch(char **args);
char **lsh_split_line(char *line);
int lsh_execute(char **args);
int lsh_num_builtins();
void sigh(int signum);

#endif
