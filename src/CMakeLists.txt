cmake_minimum_required(VERSION 3.21.5)
project(Workshop_Network_ex3 C)

set(CMAKE_C_STANDARD 11)

link_libraries("ibverbs")

add_executable(client
        bw_template.c bw_template.h
        client.c client.h
        database.c database.h
        main.c)

add_executable(server main.c
        bw_template.c bw_template.h
        client.c client.h
        database.c database.h
        )