# Progetto Finale Api - Politecnico di Milano - 2022
## **Usage**
- I suggest using Linux to run this project, you may also run this on the WSL
- (optional) You may use the [GWSL](https://opticos.github.io/gwsl/) app to run massif-visualizer. To install it use `apt install massif-visualizer`
- (optional) If you're on Windows, you may use [QCacheGrind](https://sourceforge.net/projects/qcachegrindwin/) as the callgrind output visualizer

1. First install gcc: `apt install gcc`
2. Open *scripts.config* to set the file to run, and the input you want to give (found in tests/)
3. Run using `bash execute.sh`. It'll also look for differences between the correct output and the program's output.

Remember the input is received through `stdin` and the output is written to `stdout`.  
Same goes for the rest of the .sh files. Please read them to understand what they do, they're basic bash scripts.