cmake_minimum_required(VERSION 3.21)
project(diggy C)

set(CMAKE_C_STANDARD 99)

add_executable(diggy main.c buffer_utils/bytebuf.c buffer_utils/bytebuf.h)
