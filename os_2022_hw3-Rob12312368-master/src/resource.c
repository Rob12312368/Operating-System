#include "../include/resource.h"
#include "../include/builtin.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
void get_resources(int count, int *resources)
{
    while(1)
    {
        int temp = curtask->entercounter;
        bool pass = true;
        for(int i=0;i<count;i++)
        {
            if(resourcearr[resources[i]] != true)
            {
                pass = false;
                break;
            }
        }
        if(pass)
        {
            for(int i=0;i<count;i++)
            {
                printf("Task %s gets resources %d.\n",curtask->name,resources[i]);
                resourcearr[resources[i]] = false;

                char temp[5];
                sprintf(temp," %d",resources[i]);
                if(!strcmp(curtask->resources,"none"))
                    strcpy(curtask->resources,temp);
                else
                    strcat(curtask->resources,temp);
            }
            curtask->state = 1;
        }
        else{
            curtask->state = -1; //pause
            //curtask->waiting++;
            printf("Task %s is waiting resource.\n",curtask->name);
        }
        if(pass)
            break;
        while(!pass && curtask->entercounter == temp)
        {

        }
    }
    
    return;
}

void release_resources(int count, int *resources)
{
    for(int i=0;i<count;i++)
    {
        resourcearr[resources[i]] = true; //the resource has been released;
        printf("Task %s releases resource %d.\n",curtask->name,resources[i]);
    }
    return;
}
