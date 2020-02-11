//
// Created by student on 11.02.2020.
//

#ifndef RING4U_LOG_MAKER_H
#define RING4U_LOG_MAKER_H

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>



int create_log(char* name);
int print_time(char end);
int print_message(char* message);

#endif //RING4U_LOG_MAKER_H
