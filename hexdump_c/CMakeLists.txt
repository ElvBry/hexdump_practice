cmake_minimum_required(VERSION 3.10)
project(hexdump C)

add_executable(hexdump_c src/main.c)
target_compile_options(hexdump_c PRIVATE -Wall -Wextra)
