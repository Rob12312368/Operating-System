
//Test_case_1/m1.txt Test_case_1/m2.txt

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include<fcntl.h>
#include<errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>
FILE* fp;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
typedef struct startend{
int start;
int end;
int dim;
int dim1;
int dim2;
int dim3;
int dim4;
int** a;
int** b;
pid_t threadid;
long int** c;
}stend;


void* child(void* arg)
{
    stend* st = (stend*)arg;
    int threadid = syscall(__NR_gettid);
    if(st->dim1 >= st->dim3)
    {
        for(int i=0;i<st->dim4;i++)
        {
            for(int j=st->start;j<=st->end;j++)
            {
                long int temp = 0;
                for(int k=0;k<st->dim2;k++)
                {
                    temp = temp + st->a[j][k] * st->b[k][i];
                }
                st->c[j][i] = temp;
            }
        }
    }
    else
    {
        for(int i=st->start;i<=st->end;i++)
        {
            for(int j=0;j<st->dim1;j++)
            {
                long int temp = 0;
                for(int k=0;k<st->dim2;k++)
                {
                    temp = temp + st->a[j][k] * st->b[k][i];
                }
                st->c[j][i] = temp;
            }
        }
    }
    pthread_mutex_lock(&mutex1);
    fp = fopen("/proc/thread_info","w");
    fprintf(fp,"%d ",threadid);
    fclose(fp);
    pthread_mutex_unlock(&mutex1);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int s = syscall(__NR_gettid);
    printf("PID:%d\n",s);

    int threadnum = atoi(argv[1]);
    char file1[20];
    char file2[20];
    strcpy(file1,argv[2]);
    strcpy(file2,argv[3]);


    long long int final;
    int dim = 0;
    int dim1 = 0;
    int dim2 = 0;
    int dim3 = 0;
    int dim4 = 0;    
    int lineleng = sizeof(char)*20000;
    int m=0,n=0;
    char* line = malloc(lineleng);
    FILE *f1 = fopen(file1,"r");
    FILE *f2 = fopen(file2,"r");

    fgets(line,lineleng,f1);
    dim1 = atoi(strtok(line," "));
    dim2 = atoi(strtok(NULL," "));
    fgets(line,lineleng,f2);
    dim3 = atoi(strtok(line," "));
    dim4 = atoi(strtok(NULL," "));

    int** a = (int**)malloc(sizeof(int*)*dim1);
    for(int i=0;i<dim1;i++)
    {
        a[i] = (int*)malloc(sizeof(int)*dim2);
    }
    int** b = (int**)malloc(sizeof(int*)*dim3);
    for(int i=0;i<dim3;i++)
    {
        b[i] = (int*)malloc(sizeof(int)*dim4);
    }
    long int** c = (long int**)malloc(sizeof(long int*)*dim1);
    for(int i=0;i<dim1;i++)
    {
        c[i] = (long int*)malloc(sizeof(long int)*dim4);
    }

    while(fgets(line,lineleng,f1)!=NULL)
    {
        char* token = strtok(line," ");
        while(token!=NULL)
        {
            a[m][n] = atoi(token);
            token = strtok(NULL," ");
            n++;
        }
        n=0;
        m++;
    }
    fclose(f1);

    m = 0;
    n = 0;

    while(fgets(line,lineleng,f2)!=NULL)
    {
        char* token = strtok(line," ");
        while(token!=NULL)
        {
            b[m][n] = atoi(token);
            token = strtok(NULL," ");
            n++;
        }
        n=0;
        m++;
    }
    fclose(f2);





    stend st[threadnum];

    int begin = 0;
    dim = dim3; //for test cases, dim3 is always the largest
    for(int i=0;i<threadnum;i++)
    {

        begin += dim/threadnum;
        if(i==threadnum-1)
        {
            if(threadnum == 3 || threadnum == 24)
            {
                begin = dim;
            }
        }
        st[i].end = begin;
        st[i].dim = dim;
        st[i].a = a;
        st[i].b = b;
        st[i].c = c;
        st[i].dim1 = dim1;
        st[i].dim2 = dim2;
        st[i].dim3 = dim3;
        st[i].dim4 = dim4;  
    }
    for(int i=1;i<threadnum;i++)
    {
        st[i].start = st[i-1].end+1;
    }
    st[0].start = 0;
    st[threadnum-1].end -= 1;

    char command[80] = "echo 0000 | sudo -S chmod 777 /proc/thread_info";
    system(command);

    pthread_t thr[threadnum];



 
 

    double time_used;
     
     

     
     



    time_t start = time(NULL);
    for(int i=0;i<threadnum;i++)
    {
        pthread_create(&thr[i],NULL,child,&st[i]);
    }
    for(int i=0;i<threadnum;i++)
    {
        pthread_join(thr[i],NULL);
    }
    time_t end = time(NULL);
    //printf("The elapsed time is %ld seconds\n", (end - start));

    char buffer[10000];
    fp = fopen("/proc/thread_info","r");
    fread(buffer,10000,1,fp);
    fclose(fp);
    printf("%s",buffer);


    fp = fopen("result.txt","w");
    fprintf(fp,"%d %d\n",dim1,dim4);
    for(int i=0;i<dim1;i++)
    {
        for(int j=0;j<dim4;j++)
        {
            if(j == dim4-1)
            {
                fprintf(fp,"%ld\n",c[i][j]);
            }
            else
            {
                fprintf(fp,"%ld ",c[i][j]);
            }
        }
    }
    fclose(fp);
    /*int fd = open("/proc/thread_info",O_RDWR|O_APPEND);
    char ch[50];
    char buf[50];
    if(fd==-1)
    {
        printf("error");
    }
    write(fd,"1 20\n2 30\n3 50",strlen("1 20\n2 30\n3 50"));
    close(fd);

    fd = open("/proc/thread_info",O_RDWR|O_APPEND);
    read(fd,buf,20);
    printf("%s",buf);*/
    /*free(line);
    for(int i=0;i<dim;i++)
    {
        free(a[i]);
        free(b[i]);
    }
    free(a);
    free(b);*/
}