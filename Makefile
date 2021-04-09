app=amx

CC = icc
CCFLAGS = -Wall -O3 -DAMX -DBF16
INC = amx.h
SRC = amx.c main.c

all:
	mkdir -p bin
	$(CC) $(CCFLAGS) $(SRC) -o bin/$(app)
