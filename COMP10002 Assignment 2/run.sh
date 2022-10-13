#!/bin/sh
clear
gcc -Wextra -g -O utils.c program.c -o program
valgrind --leak-check=full ./program < ./tests/test0.txt