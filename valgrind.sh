gcc -g -o $1 $1.c
valgrind --tool=callgrind ./$1