gcc -Wall -Werror -O2 -g3 -o test5 test5.c
./test5 < opentestcases/test3.txt > opentestcases/test3.myoutput.txt
diff opentestcases/test3.myoutput.txt opentestcases/test3.output.txt