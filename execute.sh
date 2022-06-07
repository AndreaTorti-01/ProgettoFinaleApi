gcc -g -o test4opt test4opt.c
./test4opt
diff opentestcases/test4opt.myoutput.txt opentestcases/test4opt.output.txt
valgrind --tool=callgrind ./test4opt