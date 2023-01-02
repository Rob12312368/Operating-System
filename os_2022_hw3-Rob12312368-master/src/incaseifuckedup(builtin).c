#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <ucontext.h>
#include <sys/time.h>
#include <signal.h>
#include "../include/function.h"
#include "../include/builtin.h"
#include "../include/command.h"
#include "../include/shell.h"
#define PAUSE -1
#define TERMI 0
#define READY 1
#define STACK 8192*3

sigset_t set;                       /* process wide signal mask */
ucontext_t signal_context;          /* the interrupt context */
void *signal_stack;                 /* global interrupt stack */

bool finished = false;
bool resume = false;
bool resourcearr[8]={true,true,true,true,true,true,true,true};
mycont tasks[100]; //tasks[0] for shell tasks[1] for simulation
mycont* curtask;
int size = 1;
int curcontext = 1;
int tens = 0;
ucontext_t back;
mycont goidl;


int help(char **args)
{
	int i;
    printf("--------------------------------------------------\n");
  	printf("My Little Shell!!\n");
	printf("The following are built in:\n");
	for (i = 0; i < num_builtins(); i++) {
    	printf("%d: %s\n", i, builtin_str[i]);
  	}
	printf("%d: replay\n", i);
    printf("--------------------------------------------------\n");
	return 1;
}

int cd(char **args)
{
	if (args[1] == NULL) {
    	fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  	} else {
    	if (chdir(args[1]) != 0)
      		perror("lsh");
	}
	return 1;
}

int echo(char **args)
{
	bool newline = true;
	for (int i = 1; args[i]; ++i) {
		if (i == 1 && strcmp(args[i], "-n") == 0) {
			newline = false;
			continue;
		}
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
	}
	if (newline)
		printf("\n");

	return 1;
}

int exit_shell(char **args)
{
	return 0;
}

int record(char **args)
{
	if (history_count < MAX_RECORD_NUM) {
		for (int i = 0; i < history_count; ++i)
			printf("%2d: %s\n", i + 1, history[i]);
	} else {
		for (int i = history_count % MAX_RECORD_NUM; i < history_count % MAX_RECORD_NUM + MAX_RECORD_NUM; ++i)
			printf("%2d: %s\n", i - history_count % MAX_RECORD_NUM + 1, history[i % MAX_RECORD_NUM]);
	}
	return 1;
}

bool isnum(char *str)
{
	for (int i = 0; i < strlen(str); ++i) {
    	if(str[i] >= 48 && str[i] <= 57)
			continue;
        else
		    return false;
  	}
  	return true;
}

int mypid(char **args)
{
	char fname[BUF_SIZE];
	char buffer[BUF_SIZE];
	if(strcmp(args[1], "-i") == 0) {

	    pid_t pid = getpid();
	    printf("%d\n", pid);
	
	} else if (strcmp(args[1], "-p") == 0) {
	
		if (args[2] == NULL) {
      		printf("mypid -p: too few argument\n");
      		return 1;
    	}

    	sprintf(fname, "/proc/%s/stat", args[2]);
    	int fd = open(fname, O_RDONLY);
    	if(fd == -1) {
      		printf("mypid -p: process id not exist\n");
     		return 1;
    	}

    	read(fd, buffer, BUF_SIZE);
	    strtok(buffer, " ");
    	strtok(NULL, " ");
	    strtok(NULL, " ");
    	char *s_ppid = strtok(NULL, " ");
	    int ppid = strtol(s_ppid, NULL, 10);
    	printf("%d\n", ppid);
	    
		close(fd);

  	} else if (strcmp(args[1], "-c") == 0) {

		if (args[2] == NULL) {
      		printf("mypid -c: too few argument\n");
      		return 1;
    	}

    	DIR *dirp;
    	if ((dirp = opendir("/proc/")) == NULL){
      		printf("open directory error!\n");
      		return 1;
    	}

    	struct dirent *direntp;
    	while ((direntp = readdir(dirp)) != NULL) {
      		if (!isnum(direntp->d_name)) {
        		continue;
      		} else {
        		sprintf(fname, "/proc/%s/stat", direntp->d_name);
		        int fd = open(fname, O_RDONLY);
        		if (fd == -1) {
          			printf("mypid -p: process id not exist\n");
          			return 1;
        		}

        		read(fd, buffer, BUF_SIZE);
        		strtok(buffer, " ");
        		strtok(NULL, " ");
        		strtok(NULL, " ");
		        char *s_ppid = strtok(NULL, " ");
		        if(strcmp(s_ppid, args[2]) == 0)
		            printf("%s\n", direntp->d_name);

        		close(fd);
     		}
	   	}
    	
		closedir(dirp);
	
	} else {
    	printf("wrong type! Please type again!\n");
  	}
	
	return 1;
}
void whichfunc(ucontext_t* u,char* s)
{
	if(!strcmp(s,"task1"))
		makecontext(u, task1, 0);
	else if(!strcmp(s,"task2"))
		makecontext(u, task2, 0);
	else if(!strcmp(s,"task3"))
		makecontext(u, task3, 0);
	else if(!strcmp(s,"task4"))
		makecontext(u, task4, 0);
	else if(!strcmp(s,"task5"))
		makecontext(u, task5, 0);
	else if(!strcmp(s,"task6"))
		makecontext(u, task6, 0);
	else if(!strcmp(s,"task7"))
		makecontext(u, task7, 0);
	else if(!strcmp(s,"task8"))
		makecontext(u, task8, 0);
	else if(!strcmp(s,"task9"))
		makecontext(u, task9, 0);
	else if(!strcmp(s,"test_resource1"))
		makecontext(u, test_resource1, 0);
	else if(!strcmp(s,"test_resource2"))
		makecontext(u, test_resource2, 0);	
	else if(!strcmp(s,"test_exit"))
		makecontext(u, test_exit, 0);
	else if(!strcmp(s,"test_sleep"))
		makecontext(u, test_sleep, 0);
	return;
}
void donothing()
{
	printf("doing nothing\n");
}
int add(char **args)
{
	finished = false;
	//printf("%d",size);
	getcontext(&tasks[size].ucont);
	tasks[size].ucont.uc_stack.ss_sp = malloc(STACK);
	tasks[size].ucont.uc_stack.ss_size = STACK;
	tasks[size].ucont.uc_stack.ss_flags = 0;
	tasks[size].ucont.uc_link = &back;
	strcpy(tasks[size].name,args[1]); //set new task's name
	tasks[size].state = READY;
	tasks[size].prior = atoi(args[3]);
	tasks[size].wait = 0;
	tasks[size].entercounter = -1;

	/*tasks[size].running = 0;
	tasks[size].waiting = 0;
	tasks[size].turnaround = 0;
	tasks[size].resourcecount = 0;
	memset(tasks[size].resources,0,15);*/
	//makecontext(&tasks[size].ucont,donothing,0);
	whichfunc(&tasks[size].ucont,args[2]);
	printf("Task %s is ready.\n",tasks[size].name);

	
	//swapcontext(&tasks[0].ucont,&tasks[size].ucont);
	size++;
	return 1;
}

int del(char **args)
{
	
	for(int i=0;i<size;i++)
	{
		if(!strcmp(tasks[i].name,args[1]))
		{
			tasks[i].state = TERMI;
			printf("Task %s is killed.\n",tasks[i].name);
			break;
		}
	}
	return 1;
}

int ps(char **args)
{

	return 1;
}
void scheduler()//change task
{
	//printf("schedule!\n");
	bool idl = true;
	for(int i=1;i<size;i++)
	{
		if(tasks[i].wait>0)
			tasks[i].wait -= 1;
		if(tasks[i].wait == 0)
		{
			if(tasks[i].state == PAUSE)
			{
				tasks[i].state = READY;
			}				
		}
		if(tasks[i].state == READY)
			idl = false;
		/*if(tasks[i].running>0 && tasks[i].state!=TERMI)
			tasks[i].turnaround++;*/
	}
	if(idl)
	{
		if(tens == 3)
			tens = 0;
		printf("CPU idle.\n");
		//tasks[curcontext].waiting+=1;
		curtask = &goidl;
		setcontext(&curtask->ucont);
	}
	
	if(!strcmp(algor,"RR"))
	{
		if(tens==3)
		{
			tens = 0;
			/*if(tasks[curcontext].state==RUNNING)
				tasks[curcontext].state = READY;*/
			//printf("schedule out %d\n",curcontext);
			curcontext = (curcontext+1) % size; //RR
			if(curcontext == 0)
				curcontext = 1;
			
			while(tasks[curcontext].state != READY)
			{
				curcontext = (curcontext+1) % size;
			}
			if(curcontext == 0)
				curcontext = 1;

			curtask = &tasks[curcontext];
			curtask->entercounter = curtask->entercounter * -1;
			printf("Task %s is running.\n",tasks[curcontext].name);
			/*tasks[curcontext].state = RUNNING;
			tasks[curcontext].running += 1;*/
			setcontext(&curtask->ucont);
		}
	}
	else if(!strcmp(algor,"FCFS"))
	{
		//printf("schedule out %d\n",curcontext);
				/*if(tasks[curcontext].state==RUNNING)
			tasks[curcontext].state = READY;*/
		while(tasks[curcontext].state==TERMI)
		{
			curcontext++;
		}
		curtask = &tasks[curcontext];
		curtask->entercounter = curtask->entercounter * -1;
		printf("Task %s is running.\n",tasks[curcontext].name);
				/*tasks[curcontext].state = RUNNING;
		tasks[curcontext].running += 1;*/
		setcontext(&curtask->ucont);
	}
	else if(!strcmp(algor,"PP"))
	{
		//printf("schedule out %d\n",curcontext);
				/*if(tasks[curcontext].state==RUNNING)
			tasks[curcontext].state = READY;*/
		int smallest = 1000000;
		for(int i=1;i<size;i++)
		{
			if(tasks[i].state != TERMI && tasks[i].prior<smallest)
			{
				curcontext = i;
				smallest = tasks[i].prior;
			}
		}

		curtask = &tasks[curcontext];
		curtask->entercounter = curtask->entercounter * -1;
		printf("Task %s is running.\n",tasks[curcontext].name);
				/*tasks[curcontext].state = RUNNING;
		tasks[curcontext].running += 1;*/
		setcontext(&curtask->ucont);
	}
	

	
}
void timer_interrupt(int j, siginfo_t *si, void *old_context)
{
	//printf("interrupt!");
	tens++;
    /* Create new scheduler context */
    getcontext(&signal_context);
    signal_context.uc_stack.ss_sp = signal_stack;
    signal_context.uc_stack.ss_size = STACK;
    signal_context.uc_stack.ss_flags = 0;
    sigemptyset(&signal_context.uc_sigmask);
    makecontext(&signal_context, scheduler, 1);
	if(!strcmp(algor,"RR"))
	{
		//printf("tens %d\n",tens);
		if(tens == 3)
		{
			
			/* save running thread, jump to scheduler */
			swapcontext(&curtask->ucont,&signal_context);
		}
	}
	else
	{
		/* save running thread, jump to scheduler */
		swapcontext(&curtask->ucont,&signal_context);
	}


}
void setup_signals(void)
{
    struct sigaction act;

    act.sa_sigaction = timer_interrupt;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART | SA_SIGINFO;

    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);//SIGALRM

    if(sigaction(SIGVTALRM, &act, NULL) != 0) {//SIGALRM
        perror("Signal handler");
    }
}
void myhandler(int sig_num)
{
	signal(SIGTSTP,myhandler);
	resume = true;
	swapcontext(&curtask->ucont,&tasks[0].ucont);
}
int start(char **args)
{
	printf("Start simulation.\n");
	//swapcontext(&tasks[0].ucont,&tasks[1].ucont);
	
	signal_stack = malloc(STACK);
	setup_signals();

	signal(SIGTSTP,myhandler);
	getcontext(&goidl.ucont);
	goidl.ucont.uc_stack.ss_sp = malloc(STACK);
	goidl.ucont.uc_stack.ss_size = STACK;
	goidl.ucont.uc_stack.ss_flags = 0;
	goidl.ucont.uc_link = NULL;
	makecontext(&goidl.ucont,idle,0);


	struct itimerval value2;
    value2.it_value.tv_sec = 0;
    value2.it_value.tv_usec = 10*1000;
    value2.it_interval.tv_sec = 0;
    value2.it_interval.tv_usec = 10*1000;
    setitimer(ITIMER_VIRTUAL, &value2, NULL);

	while(!finished)
	{	
		bool idl = true;
		for(int i=1;i<size;i++)
		{

			if(tasks[i].wait == 0)
			{
				if(tasks[i].state == PAUSE)
				{
					tasks[i].state = READY;
				}				
			}
			if(tasks[i].state == READY)
				idl = false;
		}

	
		finished = true;
		if(!resume)
		{
			if(!strcmp(algor,"PP"))
			{
				int smallest = 1000000;
				for(int i=1;i<size;i++)
				{
					if(tasks[i].state != TERMI && tasks[i].prior<smallest)
					{
						smallest = tasks[i].prior;
						curcontext = i;
						finished = false;
					}
				}
			}
			else{

				for(int i=0;i<size;i++)
				{
					if(tasks[(curcontext+i)%size].state!=TERMI)
					{
						finished = false;
						if(tasks[(curcontext+i)%size].state==READY)
						{
							curcontext = (curcontext+i)%size;
							if(curcontext == 0)
								curcontext = 1;
							break;
						}				
					}
				}
				
			}
		}
		else
		{
			resume = false;
			finished = false;
		}
		if(finished)
		{
			printf("Simulation over.\n");
		}
		else{
			if(!idl)
			{
				printf("Task %s is running.\n",tasks[curcontext].name);
				curtask = &tasks[curcontext];
				tasks[curcontext].running++;
				swapcontext(&back,&curtask->ucont);
			}
			else{
				printf("CPU idle.\n");
				curtask = &goidl;
				setcontext(&curtask->ucont);
			}
			//printf("schedule out thread %d\n",curcontext);
		}
	}
	//printf("back");

	return 1;
}

const char *builtin_str[] = {
 	"help",
 	"cd",
	"echo",
 	"exit",
 	"record",
	"mypid",
	"add",
	"del",
	"ps",
	"start"
};

const int (*builtin_func[]) (char **) = {
	&help,
	&cd,
	&echo,
	&exit_shell,
  	&record,
	&mypid,
	&add,
	&del,
	&ps,
	&start
};

int num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}
