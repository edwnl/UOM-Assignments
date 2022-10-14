#!/bin/sh
clear
gcc -Wextra -g -O program.c -o program
valgrind --leak-check=full ./program < ./tests/test0.txt
valgrind --leak-check=full ./program < ./tests/test1.txt
valgrind --leak-check=full ./program < ./tests/test2.txt