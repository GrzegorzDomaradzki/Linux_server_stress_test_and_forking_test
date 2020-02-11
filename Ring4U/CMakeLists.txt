cmake_minimum_required(VERSION 3.15)
project(Ring4U C)

set(CMAKE_C_STANDARD 11)

add_executable(Ring4U ring4u_seter.c log_maker.c log_maker.h)
add_executable(Serwer Serwer.c log_maker.c log_maker.h)
add_executable(Klient Klient.c log_maker.c log_maker.h)
add_executable(Arbiter Arbiter.c log_maker.c log_maker.h)
add_executable(Potomek Potomek.c log_maker.c log_maker.h)
add_executable(Hooligan Hooligan.c log_maker.c log_maker.h)