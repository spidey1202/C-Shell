CC=gcc
CFLAGS=-I. -lcurses -g -Wall -W
OBJ =  builtinCommand.o parse.o shell.o commandExecution.o bgJobs.o historyQueue.o
shell: $(OBJ)
	$(CC) $(OBJ) -o shell 
.PHONY: valgrind
valgrind: shell
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all -s ./shell
clean:
	rm shell $(OBJ)

