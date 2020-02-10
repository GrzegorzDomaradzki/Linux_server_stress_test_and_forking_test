#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void karmnik(char* FI, char* FO);
void ring4u();
void setsig();
void sighandle(int, siginfo_t *, void *);
void sign_in(char* file);
void forking();

int fight=0;
int children=0;
int pgid = -1;
int enemy_pgid = -1;

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
    srand(time(NULL));
    setsig();
    karmnik(FI,FO);
    return 0;
}

void setsig()
{
    struct sigaction new_action;
    new_action.sa_sigaction = sighandle;
    sigemptyset (&new_action.sa_mask);
    sigaction(SIGRTMIN+13,&new_action,NULL);
}


void sighandle(int signal, siginfo_t * signal_info, void * some_weird_variable)
{
    fight=1;
    enemy_pgid=signal_info->si_value.sival_int;
}

void sign_in(char* file)
{
    __pid_t info[2];
    memset(info,0,sizeof(__pid_t));
    __pid_t pid = getpid();
    memcpy(info,&pid,sizeof(__pid_t));
    pid = getpgrp();
    memcpy(&info[1],&pid,sizeof(__pid_t));
    int desc = open(file,O_WRONLY|O_NONBLOCK);
    if (desc==-1) return;
    write(desc,info,2* sizeof(__pid_t));
    close(desc);
}

void forking()
{
    children++;
    int res = fork();
    if (res==0)
    {
        if (pgid!=-1)
        {
            setpgrp();
        }
        else
        {
            setpgid(0, pgid);
        }
        execlp("./Potomek","Potomek",NULL);
    }
    if (pgid==-1)
    {
        pgid=res;
    }
}

void karmnik(char* FI, char* FO)
{
    struct timespec to_sleep;
    to_sleep.tv_sec=0;
    int desc = open(FO,O_RDONLY|O_NONBLOCK);
    while(!desc && errno==ENXIO)
    {
        to_sleep.tv_nsec=rand()/1000;
        nanosleep(&to_sleep,NULL);
        desc = open(FO,O_RDONLY|O_NONBLOCK);
    }

    while(!fight)
    {
        to_sleep.tv_nsec=rand()/1000;
        nanosleep(&to_sleep,NULL);
        if (children>=3 && !fight)
        {
            sign_in(FI);
            continue;
        }
        char pass = 0;
        if(-1!=read(desc,&pass, sizeof(char)) && !fight) forking();
    }
    ring4u();
}