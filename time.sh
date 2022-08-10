. scripts.config
gcc -Wall -Werror -O2 -o $program $program.c
/usr/bin/time -v ./$program < tests/$file.txt > tests/$file.myoutput.txt
