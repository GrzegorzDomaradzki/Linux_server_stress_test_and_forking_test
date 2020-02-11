#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>


void karmnik(char* FI, char* FO);
void setsig();
void sighandle(int, siginfo_t *, void *);

void sign_in(char* file);
int forking(int** child_table,int size);
void ring4u( int** child_table,int* table_size);
void surrender(int** child_table,int* table_size);
void kill_witness(int** child_table,int* table_size,int initial_number);
void count_bodies();


int fight=0;
int win=1;
int children=0;
int pgid = -1;
int enemy_pgid = -1;
int server_pid = -1;

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
    char Totem[256];
    sprintf(FI,"%s/FI",var);
    sprintf(FO,"%s/FO",var);
    sprintf(Totem,"%s/Totem",var);
    srand(time(NULL));
    setsig();
    karmnik(FI,FO);
    return 0;
}

void setsig()
{
    struct sigaction new_action, new_action2;
    new_action.sa_sigaction = sighandle;
    sigemptyset (&new_action.sa_mask);
    sigaction(SIGRTMIN+13,&new_action,NULL);
}


void sighandle(int signal, siginfo_t * signal_info, void * some_weird_variable)
{
    int sender=signal_info->si_pid;
    if (getpgid(sender)==pgid)
    {
        win=1;
        return;
    }
    fight=1;
    enemy_pgid=signal_info->si_value.sival_int;
    server_pid=sender;
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

int forking(int** child_table,int size)
{

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
    if(children==size)
    {
        size*=2;
        *child_table=realloc(*child_table,size);
        if(*child_table==NULL)
        {
            perror("Realloc fail");
            exit(-1);
        }
    }
    *child_table[children]=res;
    children++;
    return size;
}

void karmnik(char* FI, char* FO)
{

    struct timespec to_sleep;
    to_sleep.tv_sec = 0;
    int desc = open(FO, O_RDONLY | O_NONBLOCK);
    while (!desc && errno == ENXIO) {
        to_sleep.tv_nsec = rand() / 100;
        nanosleep(&to_sleep, NULL);
        desc = open(FO, O_RDONLY | O_NONBLOCK);
        }

    to_sleep.tv_nsec = rand() % 100;
    int table_size = 16;
    int* child_table=malloc(sizeof(int)*table_size);
    while(1) {
        while (!fight) {
            nanosleep(&to_sleep, NULL);
            if (children >= 3 && !fight) {
                sign_in(FI);
                continue;
            }
            char pass = 0;
            if (-1 != read(desc, &pass, sizeof(char)) && !fight) table_size=forking(&child_table,table_size);
            to_sleep.tv_nsec = rand() / 100;
        }
        ring4u(&child_table,&table_size);
    }
}

void ring4u( int** child_table,int* table_size)
{
    union sigval to_send;
    to_send.sival_int=enemy_pgid;
    for(int i =0;i<children;i++)
    {
        sigqueue(*child_table[i],SIGRTMIN+13,to_send);
    }
    int init_children_num = children;
    win=0;
    while (fight)
    {
        if(win)
        {
            kill_witness(child_table,table_size,init_children_num);
        } else {
            count_bodies();
            if(!children) surrender(child_table,table_size);
        }
    }
}

void surrender(int** child_table,int* table_size)
{
    union sigval to_send;
    to_send.sival_int = 0;
    sigqueue(server_pid,SIGRTMIN+13,to_send);
    *table_size=16;
    *child_table=realloc(*child_table, sizeof(int)*16);
    fight=0;
}

void kill_witness(int** child_table,int* table_size,int initial_number)
{
    union sigval to_send;
    to_send.sival_int=1;
    sigqueue(server_pid,SIGRTMIN+13,to_send);
    siginfo_t control, zeros;
    memset(&zeros, 0, sizeof(siginfo_t));
    int cmp;
    int res;
    do {
        memset(&control, 0, sizeof(siginfo_t));
        res = waitid(P_PGID,pgid,&control,WEXITED|WNOHANG);
        cmp = memcmp(&control,&zeros,sizeof(siginfo_t));
        if(!res && cmp) children--;
    }while(cmp && !res);
    for(int i=0;i<initial_number;i++)
    {
        if(!kill(*child_table[i],SIGTERM))
        {
            waitid(P_PID,*child_table[i],NULL,WEXITED);
        }
    }
    initial_number=children;
    children=0;
    *table_size=16;
    *child_table=realloc(*child_table,16*sizeof(int));
    for(int i=0;i<initial_number;i++) *table_size=forking(child_table,*table_size);
    fight=0;
}

void count_bodies()
{
    int res = waitid(P_PGID,pgid,NULL,WEXITED);
    if (!res) children--;
}