gcc -Wall -Werror -O2 -g3 -o $1 $1.c
./$1
diff tests/$2.myoutput.txt tests/$2.output.txt
