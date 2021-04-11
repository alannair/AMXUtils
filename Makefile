app=amx

CC = icc
CCFLAGS = -Wall -O3 -DAMX -DBF16
INC = amx.h
SRC = amx-general.c

all:
	mkdir -p bin
	$(CC) $(CCFLAGS) $(SRC) -o bin/$(app)
