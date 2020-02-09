#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <limits.h>
#include <getopt.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void options(int argc, char** argv, int* conections_num, int* port, float* intervals, float* full_time);
void generate_pathname(char* path);
void func_inet(int sockfd, struct sockaddr_un server, int* socket_unix,int* active_connections);
void print_time(int desc, struct timespec time);
void funct_server(int* socket_unix, int active_clients,struct sockaddr_un server_unix);
void time_to_text(char* text, struct timespec time);
struct timespec check_time(struct timespec real_time,float intervals);
struct timespec time_difference(struct timespec first_time, struct timespec second_time);

int main(int argc, char** argv)
{
    srand(time(NULL));
    int sockfd, connfd;
    struct sockaddr_in server_inet, cli;
    struct sockaddr_un server_unix;
    int conections_num, port,unix_desc;
    float intervals, full_time;
    options(argc,argv,&conections_num,&port,&intervals, &full_time);

    char pathname[108];
    generate_pathname(pathname);
    unix_desc = socket(AF_UNIX,SOCK_STREAM,0);
    if (unix_desc == -1) {
        perror("unix socket creation failed.");
        exit(-1);
    }
    memset(&server_unix, 0, sizeof(struct sockaddr_un));
    server_unix.sun_family=AF_UNIX;
    strcpy(server_unix.sun_path,pathname);
    if (-1==bind(unix_desc,(const struct sockaddr*)&server_unix,sizeof(struct sockaddr_un)))
    {
        perror("unix bind faliture" );
        exit(-1);
    }

    if (-1==listen(unix_desc,128))
    {
        perror("listen error");
        exit(-1);
    }

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("inet socket creation failed.");
        exit(-1);
    }
    memset(&server_inet,0, sizeof(server_inet));

    // assign IP, PORT
    server_inet.sin_family = AF_INET;
    server_inet.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_inet.sin_port = htons(port);

    int* socket_unix = calloc(100, sizeof(int));
    int active_connections =0;
    printf("Before connect\n");
    if (connect(sockfd, (struct sockaddr*)&server_inet, sizeof(server_inet)) != 0) {
        perror("connection with the server failed.");
        exit(-1);
    }
    printf("After connect\n");
    for (int i=0; i<conections_num;i++)
    {
        func_inet(sockfd,server_unix,socket_unix,&active_connections);
    }
    printf("After cli_funct\n");
    int control = 0;
    write(sockfd,&control,sizeof(int));
    printf("LAST\n");
    close(sockfd);
    struct timespec real_time,min_time,max_time,rest_time;
    max_time.tv_nsec=999999999;
    max_time.tv_sec=LONG_MAX;
    min_time.tv_nsec=0;
    min_time.tv_sec=0;
    clock_t time = clock();
    clock_t end_of_time = (clock_t)(full_time/100)*CLOCKS_PER_SEC;
    end_of_time+=time;
    do
    {
        clock_gettime(CLOCK_REALTIME,&real_time);
        funct_server(socket_unix,active_connections,server_unix);
        rest_time=check_time(real_time,intervals);
    }while (clock()<end_of_time) ;
    for (int i=0; i<active_connections;i++) close(socket_unix[i]);
    free(socket_unix);
    //printf("Full message sending time: %f\nShortest message time: ", ???);
    //print_time(0,min_time);
    //printf("\nLongest message time: ");
    //print_time(0,min_time);
    //printf("\n");
    return 0;
}


void funct_server(int* socket_unix, int active_clients,struct sockaddr_un server_unix)
{
    if (!active_clients) return;
    printf("Active client %i\n",active_clients );
    //printf("%i\n",rand()%active_clients);
    printf("I'm still alive!!!\n");
    int socket = socket_unix[/*rand()%active_clients*/0];
    printf("I'm still alive!!!\n");
    struct timespec to_send;
    clock_gettime(CLOCK_REALTIME,&to_send);
    char to_send_text[19];
    time_to_text(to_send_text,to_send);
    to_send_text[18]='\0';
    write(socket,to_send_text,19*sizeof(char));
    write(socket,&server_unix,sizeof(struct sockaddr_un));
    write(socket,&to_send,sizeof(struct timespec));

}

//////////////////////////////////////////////////////////////////
void time_to_text(char* text, struct timespec time)
{
    int minutes = time.tv_sec/60;
    text[0] = (char)(minutes % 100 /10+48);
    text[1] = (char)(minutes % 10 + 48);
    text[2] = ':';
    int seconds = time.tv_sec % 60;
    text[3] = (char)(seconds /10 +48);
    text[4] = (char)(seconds %10 +48);
    text[5] = ',';
    int nanoseconds = time.tv_nsec;
    int n = 6;
    int j =0;
    for (int i=1000000000;i>1;i/=10)
    {
        text[n++] = (char)(nanoseconds%i/(i/10)+48);
        if (j == 1 && n<16)
        {
            text[n++] = '.';
            j=0;
        }
        else
            j=1;
    }
}
//////////////////////////////////////////////////////////////////
void func_inet(int sockfd,struct sockaddr_un server,int* socket_unix, int* active_connections)
{
    struct sockaddr_un return_message;
    int control = 1;
    write(sockfd,&control,sizeof(int));
    write(sockfd,&server,sizeof(struct sockaddr_un));
    int unix_connection=accept(sockfd,NULL,NULL);
    read(sockfd,&return_message,sizeof(struct sockaddr_un));
    if (return_message.sun_family!=-1)
    {
        socket_unix[(*active_connections)++]=unix_connection;
    }
}
//////////////////////////////////////////////////////////////////
void options(int argc, char** argv, int* conections_num, int* port, float* intervals, float* full_time)
{
    *conections_num=5;
    *port =-1;
    *intervals = 50;
    *full_time = 1000;
    char c;
    while ((c = getopt (argc, argv, "S:p:d:T:")) != -1)
    {
        switch (c)
        {
            case 'S':
                *conections_num=(int)strtol(optarg,NULL,10);
                break;

            case 'p':
                *port=(int)strtol(optarg,NULL,10);
                break;

            case 'd':
                *intervals = strtof(optarg,NULL);
                break;

            case 'T':
                *full_time=strtof(optarg,NULL);
                break;
        }
    }
    if(*port==-1)
    {
        perror("Port number required");
        exit(-1);
    }
    if(*port<=1024)
    {
        perror("Port number must be greater than 1024");
        exit(-1);
    }
}
//////////////////////////////////////////////////////////////////
void generate_pathname(char* path)
{

    memset(path,0,sizeof(char)*108);
    char random_bytes[40];
    for (int i=0;i<40;++i)
    {
        if (rand()%2)
        {
            random_bytes[i]= 'A' + (random() % 26);
        }
        else
        {
            random_bytes[i]= 'a' + (random() % 26);
        }
    }
    sprintf(path,"/tmp/%s.socket",random_bytes);
}

///////////////////////////////////////////////////////////
void print_time(int desc, struct timespec time)
{
    int minutes = time.tv_sec/60;
    char temp = (char)(minutes % 100 /10+48);
    write(desc,&temp,sizeof(char));
    temp = (char)(minutes % 10 + 48);
    write(desc,&temp,sizeof(char));
    temp = ':';
    write(desc,&temp,sizeof(char));
    int seconds = time.tv_sec % 60;
    temp = (char)(seconds /10 +48);
    write(desc,&temp,sizeof(char));
    temp = (char)(seconds %10 +48);
    write(desc,&temp,sizeof(char));
    temp = ',';
    write(desc,&temp,sizeof(char));
    int nanoseconds = time.tv_nsec;
    int j =0;
    for (int i=1000000000;i>1;i/=10)
    {
        temp = (char)(nanoseconds%i/(i/10)+48);
        write(desc,&temp,sizeof(char));
        if (j == 1)
        {
            temp = '.';
            write(desc,&temp,sizeof(char));
            j=0;
        }
        else
            j=1;
    }
}
//////////////////////////////////////////////////////////////////
struct timespec time_difference(struct timespec first_time, struct timespec second_time)
{
    struct timespec diff;
    diff.tv_sec = second_time.tv_sec-first_time.tv_sec;
    if (second_time.tv_nsec<first_time.tv_nsec)
    {
        --diff.tv_sec;
        diff.tv_nsec=second_time.tv_nsec-first_time.tv_nsec+1000000000;
    }
    else
    {
        diff.tv_nsec=second_time.tv_nsec-first_time.tv_nsec;
    }
    return diff;
}
//////////////////////////////////////////////////////////////
struct timespec check_time(struct timespec real_time,float intervals)
{
    struct timespec to_diff;

    to_diff.tv_nsec=((time_t)intervals+real_time.tv_nsec)%1000000000;
    to_diff.tv_sec=((time_t)intervals+real_time.tv_sec)/1000000000;
    if((time_t)intervals+real_time.tv_nsec<1000000000) to_diff.tv_sec++;
    return time_difference(real_time, to_diff);
}