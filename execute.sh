gcc -Wall -Werror -O2 -g3 -o test5 test5.c
./test5 < opentestcases/upto18.txt > opentestcases/upto18.myoutput.txt
diff opentestcases/upto18.myoutput.txt opentestcases/upto18.output.txt