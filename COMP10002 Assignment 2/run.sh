#!/bin/sh
clear
gcc -Wextra -g -O utils.c program.c -o program
valgrind -s --track-origins=yes ./program < test0.txt