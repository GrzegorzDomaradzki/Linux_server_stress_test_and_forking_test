cmake_minimum_required(VERSION 3.15)
project(Ring4U C)

set(CMAKE_C_STANDARD 11)

add_executable(Ring4U ring4u_seter.c)
add_executable(Serwer Serwer.c)
add_executable(Klient Klient.c)
add_executable(Arbiter Arbiter.c)
add_executable(Potomek Potomek.c)
add_executable(Hooligan Hooligan.c)