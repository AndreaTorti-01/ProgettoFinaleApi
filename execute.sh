gcc -Wall -Werror -O2 -g3 -o test4opt test4opt.c
./test4opt < opentestcases/test3.txt > opentestcases/test3.myoutput.txt
diff opentestcases/test3.myoutput.txt opentestcases/test3.output.txt