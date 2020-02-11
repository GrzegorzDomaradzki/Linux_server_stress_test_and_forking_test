#include <unistd.h>
#include <signal.h>
#include "log_maker.h"

void setsig();
void sighandle(int signal, siginfo_t * signal_info, void * some_weird_variable);

int enemy_pgid =-1;

int main(int argc, char** argv)
{
    setsig();
    pause();
    if(enemy_pgid!=-1)execlp("./Hooligan","Hooligan",enemy_pgid,NULL);
    return -1;
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
    enemy_pgid=signal_info->si_value.sival_int;
}