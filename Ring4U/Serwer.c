#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

void make_fifo(char* FI, char* FO);
void feed_them(char* FI,char* FO,long delay,long max_recruit_num,long sign_in_open_time);

int main(int argc, char** argv)
{
    char* var = getenv("CAMP");
    if(var==NULL)
    {
        perror("CAMP variable wasn't set");
        exit(-1);
    }
    char FI[256];
    char FO[256];
    sprintf(FI,"%s/FI",var);
    sprintf(FO,"%s/FO",var);
    var = getenv("RECRUIT_DLY");
    if(var==NULL)
    {
        perror("RECRUIT_DLY variable wasn't set");
        exit(-1);
    }
    long delay = strtol(var,NULL,10);

    var = getenv("RECRUIT_MAX");
    if(var==NULL)
    {
        perror("RECRUIT_MAX variable wasn't set");
        exit(-1);
    }
    long max_recruit_num = strtol(var,NULL,10);

    var = getenv("RING_REG");
    if(var==NULL)
    {
        perror("RING_REG variable wasn't set");
        exit(-1);
    }
    long sign_in_open_time = strtol(var,NULL,10);
    make_fifo(FI,FO);
    feed_them(FI,FO,delay,max_recruit_num,sign_in_open_time);
    return 0;
}


void make_fifo(char* FI, char* FO)
{
    if(-1==mkfifo(FI,0666))
    {
        perror("Can't create FIFO (FI) file");
        exit(-1);
    }
    if(-1==mkfifo(FO,0666))
    {
        perror("Can't create FIFO (FO) file");
        exit(-1);
    }
}