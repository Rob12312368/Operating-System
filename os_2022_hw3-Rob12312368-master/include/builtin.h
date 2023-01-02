#ifndef BUILTIN_H
#define BUILTIN_H
#include <stdbool.h>
#include <ucontext.h>
int help(char **args);
int cd(char **args);
int echo(char **args);
int exit_shell(char **args);
int record(char **args);
int mypid(char **args);

int add(char **args);
int del(char **args);
int ps(char **args);
int start(char **args);

typedef struct mycontext
  {
	ucontext_t ucont;
	char name[20];
	int state; 
	int prior;
	int wait;
	int entercounter;
	int running;
	int waiting;
	int turnaround;
	int resourcecount;
	char resources[15];
  } mycont;

extern const char *builtin_str[];

extern const int (*builtin_func[]) (char **);

extern int num_builtins();

extern mycont tasks[100];

extern mycont* curtask;

extern ucontext_t back;

extern bool resourcearr[8];

#endif
