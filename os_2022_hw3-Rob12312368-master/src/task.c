#include "../include/task.h"
#include "../include/builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
void task_sleep(int ms)
{
    printf("Task %s goes to sleep.\n",curtask->name);
    curtask->state = -1; //pause
    curtask->wait = ms/10;
    //curtask->waiting += ms/10;
    while(curtask->state == -1)
    {
        
    }
}

void task_exit()
{
    curtask->state = 0; //terminate
    printf("Task %s has terminated.\n",curtask->name);
    strcpy(curtask->resources,"none");
    setcontext(&back);
}
