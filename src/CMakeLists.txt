cmake_minimum_required(VERSION 3.21.5)
project(Workshop_Network_ex2 C)

set(CMAKE_C_STANDARD 11)


link_libraries("ibverbs")

add_executable(server bw_template.c)
add_executable(client bw_template.c)