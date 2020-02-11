//
// Created by student on 11.02.2020.
//

#include <fcntl.h>
#include "log_maker.h"

int _log_maker_desc=-1;

int create_log(char* name)
{
    char path[256];
    sprintf(path,"logs/name.%i",getpid());
    _log_maker_desc = open(path,O_CREAT|O_TRUNC|O_WRONLY,0666);
    return  _log_maker_desc;
}

int print_time(char end)
{
    if(_log_maker_desc==-1) return -1;
    struct timespec time;
    clock_gettime(CLOCK_REALTIME,&time);
    int minutes = (int)time.tv_sec/60;
    char temp = (char)(minutes % 100 /10+48);
    (void)write(_log_maker_desc,&temp,sizeof(char)));
    temp = (char)(minutes % 10 + 48);
    (void) write(_log_maker_desc,&temp,sizeof(char));
    temp = ':';
    (void) write(_log_maker_desc,&temp,sizeof(char));
    int seconds = (int)time.tv_sec % 60;
    temp = (char)(seconds /10 +48);
    (void) write(_log_maker_desc,&temp,sizeof(char));
    temp = (char)(seconds %10 +48);
    (void) write(_log_maker_desc,&temp,sizeof(char));
    temp = ',';
    (void) write(_log_maker_desc,&temp,sizeof(char));
    int nanoseconds = time.tv_nsec;
    int j =0;
    int n = 6;
    for (int i=1000000000;i>1 ;i/=10)
    {
        temp = (char)(nanoseconds%i/(i/10)+48);
        n++;
        (void) write(_log_maker_desc,&temp,sizeof(char));
        if (j == 1 && n<16)
        {
            n++;
            temp = '.';
            (void) write(_log_maker_desc,&temp,sizeof(char));
            j=0;
        }
        else
            j=1;
    }
    (void) write(_log_maker_desc,&end,sizeof(char));
    return 0;
}

int print_message(char* message)
{
    if(_log_maker_desc==-1) return -1;
    int len = (int)strlen(message);
    print_time(' ');
    (void)write(_log_maker_desc,message,len);
    char temp = '\n';
    write(_log_maker_desc,&temp, sizeof(char));
    return 0;
}