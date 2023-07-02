# C-Shell

## Introduction

C-Shell is a Linux shell implemented in C that supports pipes, background processes, and more.

## Run the Shell

1. Clone this repository and navigate to the project directory.
2. Run the command `make` to build the shell.
3. Execute `./Shell` to start the shell. You will see a prompt of the form `Shell:path$ `.
4. Enter commands in the shell. Multiple commands can be separated by pipes (`|`).
5. To exit the shell, run the `exit` command or press `CTRL+D` on an empty prompt.
6. Use `make clean` to remove object files and the executable file.

## Memory Leaks

1. Install **Valgrind** using the command Ubuntu/Debian:
- arduino: ```sudo apt-get install valgrind```
- Fedora: ```sudo dnf install valgrind```
- CentOS: ```sudo yum install valgrind```
- Arch Linux: ```sudo pacman -S valgrind```
2. Run ```make valgrind``` to check for memory leaks. This command runs the `./Shell` executable file in the Valgrind environment.

    **Example:**
    ```
    $ make valgrind
    valgrind --leak-check=full ./Shell
    ...
    ==1234== 0 bytes in 0 blocks are definitely lost in loss record 0 of 1
    ...
    ```

    The output shows "0 bytes in 0 blocks are definitely lost", indicating that there are no memory leaks in the shell.

## Features

### Commands

The shell supports the execution of commands in the following format: `command1 [ < infile] [ | command ]* [ > outfile] [&]`.

#### Built-in Commands

The following built-in commands are supported:

1. `cd [path]`: Change the current directory to the specified directory. An error is thrown if the directory does not exist.
   - Example: `cd Documents/` navigates to the "Documents" directory.
2. `history`: Display the last 10 commands executed. The number of commands stored in history can be configured using the `HISTORY_QUEUE_SIZE` macro in [shell.c](shell.c).
   - Example: Running `history` shows a list of the previous commands executed.
3. `!num`: Re-run the command numbered `num` from the history. A negative number can be used to refer to commands relative to the last command (e.g., `!-1` runs the previous command). Piping can also be used with this command (e.g., `!3 | head -n 1`).
   - Example: `!5` re-executes the fifth command from the history.
4. `jobs`: Display information about background jobs, including their process IDs.
   - Example: Running `jobs` shows the currently running background jobs.
5. `kill [%num | pid]`: Terminate a process by sending the `SIGKILL` signal. If `%num` is used, it refers to a process by its number in the list of background processes returned by `jobs`. Otherwise, an error is thrown. If `pid` is used, it refers to a process by its process ID.
   - Example: `kill %3` terminates the background process with job number 3.
6. `help`: Display the syntax accepted by the shell and information about all the built-in commands.

### Background Processes

- To run a process in the background, append the command with the '&' symbol. For example: `gedit file_name.ext &`.
- Upon termination of a background process, the shell prints its process ID and exit status.

### Additional Features

1. `CTRL-C`: Sends the `SIGINT` signal to the current foreground job of the shell. If there is no foreground job, the signal has no effect.
2. `CTRL-D`: Logs you out of the shell without impacting the terminal. This shortcut only works on an empty prompt.
3. Input-Output Redirection and Piping: The shell handles input/output redirection (`<`, `>`) and piping (`|`) operators appropriately, ensuring correct syntax. An error is thrown if the syntax is incorrect.
