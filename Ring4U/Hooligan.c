#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

void attack();
void defend();
void setsig();
void find_enemy(int enemy_pgid);
int choose_sig();
void sigaction_asked(int, siginfo_t *, void *);
void sigaction_answered(int, siginfo_t *, void *);
void fight(char* path,int enemy_pgid);

struct sigaction old_action;
int changed =0;
int enemy_id=-1;

int main(int argc, char** argv)
{
    srand(time(NULL));
    setsig();
    int enemy_pgid = (int)strtol(argv[2],NULL,10);
    char* var = getenv("CAMP");
    if(var==NULL)
    {
        perror("CAMP variable wasn't set");
        exit(-1);
    }
    char path[256];
    sprintf(path,"%s/Totem",var);
    fight(path,enemy_pgid);
    return 0;
}

void attack()
{
    if(enemy_id!=-1) kill(enemy_id,choose_sig());
}

void defend()
{
    int choose =  choose_sig();
    if(changed!=choose)
    {
        struct sigaction new_action;
        new_action.sa_handler = SIG_IGN;
        sigemptyset (&new_action.sa_mask);
        if(changed) sigaction(changed,&old_action,NULL);
        sigaction(choose,&new_action,&old_action);
        changed=choose;
    }
}

void setsig()
{
    struct sigaction new_action, new_action2;
    new_action.sa_sigaction = sigaction_asked;
    sigemptyset (&new_action.sa_mask);
    sigaction(SIGALRM,&new_action,NULL);
    new_action2.sa_sigaction = sigaction_answered;
    sigemptyset (&new_action2.sa_mask);
    sigaction(SIGBUS,&new_action2,NULL);
}

void find_enemy(int enemy_pgid)
{
    struct timespec to_sleep;
    to_sleep.tv_sec=0;
    to_sleep.tv_nsec=rand()%50;
    int rest;
    do
    {
        rest = nanosleep(&to_sleep,&to_sleep);
    }while(rest);
    kill(-enemy_pgid,SIGALRM);
}

int choose_sig()
{
    int random = rand()%2;
    if (!random) return SIGUSR1;
    return SIGUSR2;
}

void action_asked(int signal, siginfo_t * signal_info, void * some_weird_variable)
{
    int sender = signal_info->si_pid;
    kill(sender,SIGBUS);
}

void sigaction_answered(int signal, siginfo_t * signal_info, void * some_weird_variable)
{
    int sender = signal_info->si_pid;
    if (enemy_id!=-1)
    {
     if (0!=rand()%3) enemy_id=sender;
    }
    else enemy_id=sender;
}

void fight(char* path,int enemy_pgid)
{}
