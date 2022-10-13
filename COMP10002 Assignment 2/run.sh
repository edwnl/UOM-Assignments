#!/bin/sh
clear
gcc -Wextra -g -O utils.c program.c -o program
#./program < ./tests/test0.txt > ./tests/testout.txt
#diff ./tests/testout.txt ./tests/test0-out.txt
valgrind --leak-check=full ./program < ./tests/test2e.txt