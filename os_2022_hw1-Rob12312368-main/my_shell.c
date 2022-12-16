#ifndef _POSIX_C_SOURCE
	#define  _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h> 
#define BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
char* currentdir;
int tokencount;
typedef struct node
{
	char str[64];
	struct node* next;

}node;
node* top = NULL;
node* cur = NULL;
char* read_line(void)
{
	int bufsize = BUFSIZ;
	int position = 0;
	char* buffer = (char*)malloc(sizeof(char)*bufsize);
	int c;

	if(!buffer)
	{
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		c = getchar();
		if(c == EOF || c == '\n')
		{
			buffer[position] = '\0';
			return buffer;
		}
		else
		{
			buffer[position] = c;
		}
		position++;

		if(position >= bufsize)
		{
			bufsize += BUFSIZ;
			buffer = realloc(buffer, bufsize);
			if(!buffer)
			{
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}

}
char** split_line(char* line)
{
	int bufsize = BUFSIZ;
	int position = 0;
	char** tokens = (char**)malloc(sizeof(char*)*bufsize);
	char* token;

	if(!tokens)
	{
		fprintf(stderr,"lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, TOK_DELIM);
	while(token != NULL)
	{
		tokens[position] = token;
		position++;

		if(position >= bufsize)
		{
			bufsize += TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if(!tokens)
			{
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, TOK_DELIM);
	}
	tokens[position] = NULL;
	tokencount = position;
	return tokens;

}
void catHeadTail(char* filename, int headtail, char* line)
{

    char *ptr;
    long num;
    num = strtol(line, &ptr, 10);
	num *= -1;
	char cwd[256];
	if(getcwd(cwd, sizeof(cwd)) == NULL)
	{
      perror("getcwd() error");
	}
    else
	{
	  strcat(cwd,"/");
	  strcat(cwd,filename);
  	}

	char lines[1000][256];
	int linecount = 0;
	FILE *input= fopen(cwd, "r");
	if(!input)
	{
		perror("lsh: file open error");
	}

	while( fgets (lines[linecount], 256, input)!=NULL ) {
		linecount++;
   	}
   	fclose(input);
	
	if(headtail== 1)
	{
		for(int i=0;i<num;i++)
		{
			printf("%s",lines[i]);
		}
	}
	else{
		for(int i=linecount-num;i<=linecount;i++)
		{
			printf("%s",lines[i]);
		}
	}




	  
}


int lsh_launch(char **args)
{
	pid_t pid;
	int background = 0;
	if(!strcmp(args[tokencount-1],"&"))
	{
		background = 1;
		args[tokencount-1] = '\0';
	}
	pid = fork();
	
	if(pid==0)
	{
		//child process
		setpgid(0,0);
		if(tokencount>=4 && !strcmp(args[2],"|")) //pipleline
		{
			if(!strcmp(args[0],"cat") && !strcmp(args[2],"|") )
			{
				if(!strcmp(args[3],"head"))
					catHeadTail(args[1],1,args[4]); //head
				else
					catHeadTail(args[1],0,args[4]); //tail
			}
		}
		else if(tokencount>=3 && !strcmp(args[1],"<"))
		{
			char *temp[3];
			temp[0] = args[0];
			temp[1] = args[2];
			temp[2] = NULL;
			if(execvp(temp[0],temp) == -1)
			{
				perror("lsh");
			}
		}
		else if(tokencount>=4 && !strcmp(args[2],">"))
		{
			char lines[1000][256];
			int linecount = 0;
			FILE *input= fopen(args[1], "r");
			FILE *output = fopen(args[3], "w");
			if(!input || !output)
			{
				perror("lsh: file open error");
			}

			while( fgets (lines[linecount], 256, input)!=NULL ) {
				linecount++;
			}

			for(int i=0;i<linecount;i++)
			{
				fputs(lines[i],output);
			}
		}
		else if(execvp(args[0],args) == -1)
		{
			perror("lsh");
		}
		exit(0);
	}
	else if(pid < 0)
	{
		perror("lsh");
	}
	else
	{
		//parent process
		if(!background)
		{
			wait(NULL);
		}
		else{
			printf("[PID]: %d\n",pid);
			wait(NULL);
		}
	}
	return 1;
}
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_echo(char **args);
int lsh_record(char **args);
int lsh_replay(char **args);
int lsh_mypid(char **args);
/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "echo",
  "record",
  "replay",
  "mypid"
};

/* all the buildin func*/
int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &lsh_echo,
  &lsh_record,
  &lsh_replay,
  &lsh_mypid
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

int lsh_help(char **args)
{
  printf("1: help: show all build-in function info\n"
  "2: cd: change directory\n"
  "3. echo: echo the strings to standard output\n"
  "4. record: show last 16 commands you typed in\n"
  "5: replay: re-execute the command showed in record\n"
  "6: mypid: find and print proocess-ids\n"
  "7: exit: exit shell\n");
  return 1;
}

int lsh_exit(char **args)
{
  return 0;
}
int lsh_echo(char **args)
{
	if(!strcmp(args[1],"-n"))
	{
		for(int i=2;i<tokencount;i++)
		{
			printf("%s ",args[i]);
		}
	}
	else
	{
		for(int i=1;i<tokencount;i++)
		{
			printf("%s ",args[i]);
		}
		printf("\n");
	}
	return 1;
}
int lsh_record(char **args)
{
	int i = 1;
	node* go = top;
	while(go!=NULL)
	{
		printf("%d: %s\n",i,go->str);
		go = go->next;
		i++;
	}
	return 1;
}
int lsh_replay(char **args)
{
	node* go;
	go = top;

	for(int i=0;i<atoi(args[1])-1;i++)
	{
		go = go->next;
	}
	node* add = (node*)malloc(sizeof(node));
	add -> next = NULL;
	strcpy(add->str, go->str);
	cur -> next = add;
	cur = cur -> next;

	char** command = split_line(go->str);


	int state;
	pid_t pd = fork();
	if (pd == 0)
	{
		if(execvp(command[0],command) == -1)
		{
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	}
	else if (pd < 0)
	{
		perror("lsh");
	}
	else
	{
		do {
		waitpid(pd, &state, WUNTRACED);
		} while (!WIFEXITED(state) && !WIFSIGNALED(state));
	}


	return 1;
}
int lsh_mypid(char **args)
{
	int pid = getpid();
	if(!strcmp(args[1],"-i"))
	{
		printf("%d",pid);
	}
	else if(!strcmp(args[1],"-p"))
	{		
		FILE *file;
		char path[30]="/proc/";
		char content[512];
		char *token;
		strcat(path,args[2]);
		strcat(path,"/stat");
		file = fopen(path,"r");
		if (file)
		{
			int i=1;
			fgets(content, 512, file);
			token = strtok(content, TOK_DELIM);
			while(i<4)
			{
				token = strtok(NULL, TOK_DELIM);
				i++;
			}
			fclose(file);
		}
		else{
			printf("not existed");
		}


		printf("%d",atoi(token));
	}
	else if(!strcmp(args[1],"-c"))
	{
		FILE *file;
		char path[30]="/proc/";
		char content[512];
		char *token;
		strcat(path,args[2]);
		strcat(path,"/task/");
		strcat(path,args[2]);
		strcat(path,"/children");
		file = fopen(path,"r");
		if (file)
		{
			fgets(content, 512, file);
			token = strtok(content, TOK_DELIM);
			printf("%s",token);
			while(token!=NULL)
			{
				token = strtok(NULL, TOK_DELIM);
				printf("%s",token);
			}
			fclose(file);
		}
		else{
			fprintf(stderr, "can not open the file\n");
		}
		return 1;

	}
	return 1;
}
int lsh_execute(char **args)
{
  int i;
  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}
void shell_loop()
{
	int counter = 0;
	char* line;
	char** args;
	char* allline = malloc(sizeof(char)*64);
	int state = 1;
	while(state == 1)
	{

		printf(">>>$ ");
		line = read_line();
		strcpy(allline,line);
		args = split_line(line);
		if (args[0] == NULL) { //space or \t
			continue;
		}
		else if(strcmp(args[0],"replay"))// not replay
		{
			if (counter == 0)
			{
				top = (node*)malloc(sizeof(node));
				strcpy(top->str,allline);
				top->next = NULL;

				cur = top;
			}
			else{
				node* m = (node*)malloc(sizeof(node));
				strcpy(m->str,allline);
				m->next = NULL;

				cur->next = m;
				cur = m;
			}
			if(counter>=16)
			{
				node* del;
				del = top;
				top = top->next;
				free(del);
			}
		}
		else if(!strcmp(args[0],"replay")) //replay
		{
			node* go = top;
			for(int i=0;i<atoi(args[1])-1;i++)
			{
				go = go->next;
			}
			node* add = (node*)malloc(sizeof(node));
			strcpy(add->str,go->str);
			add->next = NULL;
			cur->next = add;

			if(counter>=16)
			{
				node* del = (node*)malloc(sizeof(node));
				del = top;
				top = top->next;
				free(del);
			}
		}
		state = lsh_execute(args);

		free(line);
		free(args);
		//free(allline); don't really know why it can not be freed
		counter++;

	}
}



int main(){

	shell_loop();
	
	return EXIT_SUCCESS;

}
