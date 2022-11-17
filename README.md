# Progetto Finale Api - Politecnico di Milano - 2022

- I suggest using Linux to run this project, you may also run this on the [WSL](https://learn.microsoft.com/en-us/windows/wsl/install) like I did. Example code is using ubuntu.
- You may use the [GWSL](https://opticos.github.io/gwsl/) app to run massif-visualizer and kcachegrind from the WSL
- If you're on Windows, you may use [QCacheGrind](https://sourceforge.net/projects/qcachegrindwin/) as the callgrind output visualizer, instead of kcachegrind

## **Usage**

- Install gcc, massif-visualizer and kcachegrind

```sh
sudo apt install gcc massif-visualizer kcachegrind
```

- Open _scripts.config_ to set the file to run, and select the input you want to give to the program (found in tests folder)

- Run the program and look for differences between the correct output and the current output using _diff_, printing nothing if the current output is correct

```sh
bash execute.sh
```

- Analyze and view the memory usage during execution. By default, I set it to include stack memory, but you may open `massif.sh` and replace `--stacks=yes` with `--pages-as-heap=yes` or remove it entirely

```sh
bash massif.sh
```

- Analyze and view the % of the total execution time each function is taking up

```sh
bash callgrind.sh
```

- Time the execution with the given input

```sh
bash time.sh
```

Remember the input is received through `stdin` and the output is written to `stdout`; configuration for vscode debugging is already provided

If you want to use the GWSL, remember to configure the windows firewall accordingly. This app behaves like an X server, enabling linux under the WSL to output its GUIs to the display. Neat!