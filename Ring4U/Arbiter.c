#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "log_maker.h"

void setsig();
void sighandle();
void totem(struct timespec to_open, struct timespec to_close,char* path);
struct timespec to_time(long microseconds);

int i=0;

int main(int argc, char** argv)
{
    char* var;
    char* proper;
    var = getenv("TOTEM_OP");
    if(var==NULL)
    {
        perror("TOTEM_OP variable wasn't set");
        exit(-1);
    }
    long to_open = strtol(var,&proper,10);

    var = getenv("TOTEM_CL");
    if(var==NULL)
    {
        perror("TOTEM_CL variable wasn't set");
        exit(-1);
    }
    long to_close = strtol(var,&proper,10);

    var = getenv("CAMP");
    if(var==NULL)
    {
        perror("CAMP variable wasn't set");
        exit(-1);
    }
    char path[256];
    sprintf(path,"%s/Totem",var);
    setsig();
    if(-1==mkfifo(path,0666))
    {
        perror("Can't create FIFO file");
        exit(-1);
    }
    struct timespec time_open = to_time(to_open);
    struct  timespec time_close = to_time(to_close);
    totem(time_open,time_close,path);

    return -1;
}


void setsig()
{
    struct sigaction new_action;
    new_action.sa_handler = sighandle;
    sigemptyset (&new_action.sa_mask);
    sigaction(SIGRTMIN+13,&new_action,NULL);
}


void sighandle()
{
    i=!i;
}

struct timespec to_time(long microseconds)
{
    struct timespec new;
    new.tv_nsec = microseconds%1000000000;
    new.tv_sec = microseconds/1000000000;
    return new;
}

void totem(struct timespec to_open, struct timespec to_close,char* path)
{
    if(!i)
    {
        pause();
        nanosleep(&to_close,NULL);
    }
    int desc = open(path,O_RDONLY|O_NONBLOCK);
    nanosleep(&to_open,NULL);
    close(desc);
    nanosleep(&to_close,NULL);
}