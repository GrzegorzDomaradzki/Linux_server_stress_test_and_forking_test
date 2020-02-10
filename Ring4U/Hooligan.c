#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    int id = fork();
    if (id==0)
    {
        printf("I'm child, my pid is: %i, and my pgid is: %i\n",getpid(),getpgrp());
        setpgrp();
        printf("I'm child, my pid is: %i, and my pgid is: %i\n",getpid(),getpgrp());
        return 0;
    }

    printf("I'm parent, my pid is: %i, and my pgid is: %i\n",getpid(),getpgrp());
    return 0;
}