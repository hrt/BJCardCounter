CC      = gcc
CFLAGS  = -g -std=c99 -Wall -Werror #-O3
HEADERS = entropy.h pcg_spinlock.h pcg_variants.h

.SUFFIXES: .c .h


count: 
	$(CC) CardCount.c entropy.c $(HEADERS) libpcg_random.a $(CFLAGS) -o $@
basicStrat:
	$(CC) BasicStrategy.c entropy.c $(HEADERS) libpcg_random.a $(CFLAGS) -o $@

clean:
	rm -f count
	rm -f basicStrat
	rm -f *.txt

.PHONY: all clean basicStrat
