cmake_minimum_required(VERSION 3.21)
project(diggy C)

set(CMAKE_C_STANDARD 99)

add_executable(diggy main.c lib/buffer_utils/bytebuf.c lib/buffer_utils/bytebuf.h)
