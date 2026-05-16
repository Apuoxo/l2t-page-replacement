CC = gcc
CFLAGS = -O3 -Wall -lm

all: vmsim

vmsim: vmsim.c
	$(CC) $(CFLAGS) -o vmsim vmsim.c

clean:
	rm -f vmsim

run: vmsim
	./vmsim 1000 msr_trace.txt