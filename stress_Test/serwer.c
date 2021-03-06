#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>



char prefix[80];
int log_nr = 0;
int log_changed=0;
int out =0;

void sighandle();
int open_log();
void setsig();
void server_function(int inet_desc, int* client_num, int** table_desc,struct sockaddr_un** table_addr);
void client_function(int client_num, int* desc_array , struct sockaddr_un* array_addr, int* desc);
struct timespec time_difference(struct timespec first_time, struct timespec second_time);
void print_time(int desc, struct timespec time);
int authorisation(struct sockaddr_un x, struct sockaddr_un y);
void sighandle_out();


int main(int argc, char** argv)
{
    if(argc!=3)
    {
        perror("Port number required");
        return -1;
    }

    long int port = strtol(argv[1],NULL,10);

    if(port<1024 )
    {
        perror("Provided number is not valid port number");
        return -1;
    }
    //długość bufora
    strcpy(prefix,argv[2]);
    int desc = open_log();
    setsig();
    int inet_desc, inet_connection;
    unsigned len;
    struct sockaddr_in servaddr, cliaddr;
    inet_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (inet_desc == -1) {
        perror("socket creation failed...");
        return -1;
    }
    memset(&servaddr,0,sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port);

    if ((bind(inet_desc, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        perror("socket bind failed");
        exit(0);
    }
    if ((listen(inet_desc, 2)) != 0) {
        perror("Listen failed");
        exit(0);
    }
    len = sizeof(cliaddr);
    int active_clients = 0;
    int* clients_table = calloc(1000, sizeof(int));
    struct sockaddr_un* clients_addr = calloc(1000, sizeof(struct sockaddr_un));
    inet_connection = accept(inet_desc, (struct sockaddr*)&cliaddr, &len);
    if (inet_connection < 0) {
        perror("connection error");
        exit(-1);
    }
    server_function(inet_connection,&active_clients,&clients_table,&clients_addr);
    close(inet_connection);
    close(inet_desc);
    client_function(active_clients,clients_table,clients_addr,&desc);
    free(clients_table);
    close(desc);
    return 0;
}
//////////////////////////////////////////////////////////////////
void sighandle()
{
    ++log_nr;
    log_changed =1;
}
//////////////////////////////////////////////////////////////////
int open_log()
{
    int desc = -1;
    while (1)
    {
        char f_name[83];
        sprintf(f_name,"%s%03d",prefix,log_nr);
        desc = open(f_name,O_RDWR|O_CREAT|O_TRUNC,0666);
        if (desc >=0) break;
        ++log_nr;
    }
    log_changed=0;
    return desc;
}

//////////////////////////////////////////////////////////////////
void setsig()
{
    struct sigaction new_action, new_action2;
    new_action.sa_handler = sighandle;
    sigemptyset (&new_action.sa_mask);
    sigaction(SIGUSR1,&new_action,NULL);
    new_action.sa_handler = sighandle_out;
    sigemptyset (&new_action2.sa_mask);
    sigaction(SIGUSR2,&new_action2,NULL);
}
//////////////////////////////////////////////////////////////////
void server_function(int inet_desc, int* client_num, int** table_desc,struct sockaddr_un** table_addr)
{

    struct sockaddr_un unix_server;
    int unix_client;
    int control;
    (void)read(inet_desc,&control,sizeof(int));
    while(control) {
        memset(&unix_server, 0, sizeof(unix_server));
        memset(&unix_client, 0, sizeof(unix_client));
        (void)read(inet_desc, &unix_server, sizeof(struct sockaddr_un));
        //make unix client
        if (-1 == (unix_client = socket(AF_UNIX, SOCK_STREAM, 0))) {
            perror("error while creating unix_client socket");
            exit(-1);
        }

        int ret_val = connect(unix_client, (struct sockaddr *) &unix_server, sizeof(unix_server));
        if (ret_val == -1) {
            unix_server.sun_family = USHRT_MAX;
        } else {
            int flags = fcntl(unix_client, F_GETFL);
            fcntl(unix_client, F_SETFL, flags | O_NONBLOCK);
            table_desc[0][*client_num] = unix_client;
            table_addr[0][*client_num] = unix_server;
            ++(*client_num);
            //resize table
        }
        (void)write(inet_desc, &unix_server, sizeof(unix_server));
        (void)read(inet_desc,&control,sizeof(int));
    }
}
//////////////////////////////////////////////////////////////////
void client_function(int client_num, int* desc_array , struct sockaddr_un* array_addr, int* desc)
{
    struct timespec send_time, read_time;
    struct sockaddr_un adress;
    int* closed_lines = calloc(client_num,sizeof(int));
    int client_quit = 0;
    while(!out && !client_quit) {
        client_quit =1;
        for (int i = 0; i < client_num; i++) {
            if (closed_lines[i]) continue;
            client_quit =0;
            char buff[256];
            memset(buff, 0, sizeof(char) * 256);
            if (log_changed) {
                close(*desc);
                *desc = open_log();
            }
            int curr_desc = desc_array[i];
            char c = 0;
            int j = 0;
            int ret = read(curr_desc, &c, sizeof(char));
            if (ret == -1 || c=='\0') {
                if (errno == EAGAIN) {
                    continue;
                }
                close(curr_desc);
                closed_lines[i]=1;
            }
            int flags = fcntl(curr_desc, F_GETFL);
            fcntl(curr_desc, F_SETFL, flags & ~O_NONBLOCK);
            buff[j++] = c;
            (void)read(curr_desc, &c, 1);
            while ('\0' != c) {

                buff[j++] =  c;
                (void)read(curr_desc, &c, sizeof(char));
            }
            char *separator = " : \n";
            (void)read(curr_desc, &adress, sizeof(struct sockaddr_un));
            (void)read(curr_desc, &send_time, sizeof(struct timespec));
            clock_gettime(CLOCK_REALTIME, &read_time);
            if (!authorisation(adress, array_addr[i])) continue;
            struct timespec diff = time_difference(send_time, read_time);
            print_time(*desc, read_time);
            (void)write(*desc, separator, 3 * sizeof(char));
            (void)write(*desc, buff, j*sizeof(char));
            (void)write(*desc, separator, 3 * sizeof(char));
            print_time(*desc, diff);
            (void)write(*desc, &separator[3], sizeof(char));
            flags = fcntl(curr_desc, F_GETFL);
            fcntl(curr_desc, F_SETFL, flags | O_NONBLOCK);
        }
    }
    free(closed_lines);
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

//////////////////////////////////////////////////////////////////
void print_time(int desc, struct timespec time)
{
    long minutes = time.tv_sec/60;
    char temp = (char)(minutes % 100 /10+48);
    (void)write(desc,&temp,sizeof(char));
    temp = (char)(minutes % 10 + 48);
    (void)write(desc,&temp,sizeof(char));
    temp = ':';
    (void)write(desc,&temp,sizeof(char));
    long seconds = time.tv_sec % 60;
    temp = (char)(seconds /10 +48);
    (void)write(desc,&temp,sizeof(char));
    temp = (char)(seconds %10 +48);
    (void)write(desc,&temp,sizeof(char));
    temp = ',';
    (void)write(desc,&temp,sizeof(char));
    int nanoseconds = time.tv_nsec;
    int j =0;
    int n = 6;
    for (int i=1000000000;i>1 ;i/=10)
    {
        temp = (char)(nanoseconds%i/(i/10)+48);
        n++;
        (void)write(desc,&temp,sizeof(char));
        if (j == 1 && n<16)
        {
            n++;
            temp = '.';
            (void)write(desc,&temp,sizeof(char));
            j=0;
        }
        else
            j=1;
    }
}
//////////////////////////////////////////////////////////////////
int authorisation(struct sockaddr_un x, struct sockaddr_un y)
{
    if(x.sun_family!=y.sun_family) return 0;
    for(int i =0; i<108; i++)
    {
        if(x.sun_path[i]!=y.sun_path[i]) return 0;
        if (x.sun_path[i]=='\0') break;
    }
    return 1;
}

void sighandle_out()
{
    out =1;
}