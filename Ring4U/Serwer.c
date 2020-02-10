#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

void make_fifo(char* FI, char* FO);
void feed_them(char* FI,char* FO,long delay,long max_recruit_num,long sign_in_open_time);
void give_food(int FO_desc,long max_recruit_num);
void read_letters(char* FI,struct timespec sign_in_open_time);
void give_them_hell(int* good_fathers,int* goons);
int make_arbiter();
int is_epmty(int FO_desc,char* FO);

int arbiter_pid;
int fight=0;

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
    if(-1==make_arbiter())
    {
        perror("Arbiter making error");
        exit(-1);
    }
    feed_them(FI,FO,delay,max_recruit_num,sign_in_open_time);
    return 0;
}

int make_arbiter()
{
    int pid = fork();
    if(pid==0)
    {
        execlp("./Arbiter","Arbiter",NULL);
    }
    arbiter_pid=pid;
    return pid;
}

void feed_them(char* FI,char* FO,long delay,long max_recruit_num,long sign_in_open_time)
{
    srand(time(NULL));
    struct timespec sleep_delay, sleep_open_time;
    sleep_delay.tv_nsec=delay%1000000000;
    sleep_delay.tv_sec=delay/1000000000;
    sleep_open_time.tv_nsec=sign_in_open_time%1000000000;
    sleep_open_time.tv_sec=sign_in_open_time/1000000000;
    int FO_desc=open(FO,O_WRONLY);
    if(FO_desc==-1)
    {
        perror("Can't open FIFO (FO) file");
        exit(-1);
    }
    while(1)
    {
        if(is_epmty(FO_desc,FO)) give_food(FO_desc,max_recruit_num);
        if(!fight) {
            read_letters(FI, sleep_open_time);
        }
    }
}

void give_food(int FO_desc,long max_recruit_num)
{
    int packs = (int)(rand()/max_recruit_num);
    char c =1;
    for (int i=0; i<packs;i++)
    {
        write(FO_desc,&c,sizeof(char));
    }
}

int is_epmty(int FO_desc,char* FO)
{
    int desc = open(FO,O_RDONLY|O_NONBLOCK);
    int c;
    int ret = read(desc,&c,sizeof(char));
    if (ret==-1) return 1;
    ret = write(FO_desc,&c,sizeof(char));
    if(ret==-1)
    {
        printf("Something very bad happened to your FIFO file...");
        exit(-1);
    }
    return 0;
}

void read_letters(char* FI,struct timespec sign_in_open_time)
{
    int success_table[4];
    int goons[2];
    int good_fathers[2];
    int desc=open(FI,O_RDWR|O_NONBLOCK);
    if (desc==-1)
    {
        perror("Error opening FIFO (FI) file");
        exit(-1);
    }
    nanosleep(&sign_in_open_time,NULL);
    success_table[0]=read(desc,good_fathers,sizeof(__pid_t));
    success_table[1]=read(desc,goons,sizeof(__pid_t));
    success_table[2]=read(desc,&good_fathers[1],sizeof(__pid_t));
    success_table[3]=read(desc,&goons[1],sizeof(__pid_t));
    close(desc);
    for (int i=0;i<4;i++)
    {
        if (success_table[i]==-1) return;
    }
    give_them_hell(good_fathers,goons);
}


void give_them_hell(int* good_fathers,int* goons)
{
    union sigval first,second;
    struct timespec to_sleep;
    to_sleep.tv_sec=0;
    to_sleep.tv_nsec=100;
    first.sival_int=goons[1];
    second.sival_int=goons[0];
    sigqueue(good_fathers[0],SIGRTMIN+13,first);
    sigqueue(good_fathers[1],SIGRTMIN+13,second);
    nanosleep(&to_sleep,NULL);
    kill(arbiter_pid,SIGRTMIN+13);
    fight=1;
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