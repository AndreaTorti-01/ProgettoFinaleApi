. scripts.config
bash debug_compile.sh
valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$program < tests/$file.txt > tests/$file.myoutput.txt