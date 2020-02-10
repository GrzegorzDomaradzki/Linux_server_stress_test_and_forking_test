cmake_minimum_required(VERSION 3.14.5)
project(zad_zad3 C)

set(CMAKE_C_STANDARD 11)

add_executable(zad_zad3 main.c)
add_executable(sr serwer.c)
add_executable(cl klient.c)