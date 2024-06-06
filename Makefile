CC=gcc
CFLAGS=-Wall -Wextra -Werror

bfc: main.c
	$(CC) $(CFLAGS) -o $@ $<
