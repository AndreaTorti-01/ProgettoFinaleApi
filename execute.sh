gcc -Wall -Werror -O2 -g3 -o $1 $1.c
./$1
diff opentestcases/$2.myoutput.txt opentestcases/$2.output.txt
