. scripts.config
bash debug_compile.sh
valgrind --tool=callgrind ./$program < tests/$file.txt > tests/$file.myoutput.txt