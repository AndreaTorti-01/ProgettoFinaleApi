. scripts.config
gcc -Wall -Werror -O2 -o $program $program.c
./$program < tests/$file.txt > tests/$file.myoutput.txt
diff tests/$file.myoutput.txt tests/$file.output.txt
