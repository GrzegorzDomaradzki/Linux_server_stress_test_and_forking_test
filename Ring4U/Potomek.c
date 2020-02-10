#include <unistd.h>
#include <signal.h>

void setsig();
void sighandle();


int main(int argc, char** argv)
{
    setsig();
    pause();
    execlp("./Hooligan","Hooligan",NULL);
    return -1;
}

void setsig()
{
    struct sigaction new_action;
    new_action.sa_sigaction = sighandle;
    sigemptyset (&new_action.sa_mask);
    sigaction(SIGRTMIN+13,&new_action,NULL);
}


void sighandle()
{

}