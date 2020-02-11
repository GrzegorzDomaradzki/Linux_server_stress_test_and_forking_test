#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log_maker.h"

int set_options(int argc,char** argv);
void run_klients(int num_of_clients);
void run_serwer();

int main(int argc, char** argv)
{
    int num_of_clients=set_options(argc,argv);
    run_klients(num_of_clients);
    run_serwer();
    while(1) pause();
    return 0;
}

int set_options(int argc,char** argv)
{
    char c;
    char CAMP[255]="default";
    char RECRUIT_DLY[255]="1000";
    char RECRUIT_MAX[255]="3";
    char RING_REG[255]="25";
    char TOTEM_OP[255]="100";
    char TOTEM_CL[255]="100";
    int clients = 3;
    while ((c = (char)getopt (argc, argv, "C:d:m:r:o:c:l:")) != -1)
    {
        switch (c)
        {
            case 'C':
                strcpy(CAMP,optarg);
                break;

            case 'd':
                strcpy(RECRUIT_DLY,optarg);
                break;

            case 'm':
                strcpy(RECRUIT_MAX,optarg);
                break;

            case 'r':
                strcpy(RING_REG,optarg);
                break;

            case 'o':
                strcpy(TOTEM_OP,optarg);
                break;

            case 'c':
                strcpy(TOTEM_CL,optarg);
                break;

            case 'l':
                clients = (int)strtol(optarg,NULL,10);
                break;

            default:
                perror("Unknown flag: valid flags are -C <CAMP> -d <RECRUIT_DLY> -m <RECRUIT_MAX> -r <RING_REG> -o <TOTEM_OP> -c <TOTEM_CL> -l <number_of_client_processes>\n");
                exit(-1);
        }
    }
    setenv("CAMP",CAMP, 1);
    setenv("RECRUIT_DLY",RECRUIT_DLY, 1);
    setenv("RECRUIT_MAX",RECRUIT_MAX, 1);
    setenv("RING_REG",RING_REG, 1);
    setenv("TOTEM_OP",TOTEM_OP, 1);
    setenv("TOTEM_CL",TOTEM_CL, 1);
    if (clients<3) clients =3;
    return clients;
}

void run_klients(int num_of_clients)
{
    for (int i=0;i<num_of_clients;i++)
    {
        int ret = fork();
        if(!ret)
        {
            execlp("./Klient","Klient",NULL);
            perror("exec error");
            exit(-1);
        }
        if(ret==-1)
        {
            perror("Forking error");
            exit(-1);
        }
    }
}

void run_serwer()
{
    int ret = fork();
    if(!ret)
    {
        execlp("./Serwer","Serwer",NULL);
        perror("exec error");
        exit(-1);
    }
    if(ret==-1)
    {
        perror("Forking error");
        exit(-1);
    }
}