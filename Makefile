all: pruner

pruner: main.c
	gcc main.c -Wall -Wextra -g -O3 -std=c11 -o pruner