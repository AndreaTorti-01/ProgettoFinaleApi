. scripts.config
bash debug_compile.sh
valgrind --tool=massif --stacks=yes --massif-out-file=massif.out.txt ./$program < tests/$file.txt > tests/$file.myoutput.txt
massif-visualizer massif.out.txt
# --pages-as-heap=yes