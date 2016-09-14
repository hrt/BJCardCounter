CC      = gcc
CFLAGS  = -std=c99 -Wall -Werror -g
HEADERS = entropy.h pcg_spinlock.h pcg_variants.h

.SUFFIXES: .c .h

count: 
	$(CC) Main.c entropy.c $(HEADERS) libpcg_random.a $(CFLAGS) -o count
	$(CC) card_lib_tests.c entropy.c $(HEADERS) libpcg_random.a $(CFLAGS) -o tests_card
	$(CC) hand_lib_tests.c entropy.c $(HEADERS) libpcg_random.a $(CFLAGS) -o tests_hand

clean:
	rm -f test
	rm -f count
	rm -f count.txt
	rm -f tests_card
	rm -f tests_hand

.PHONY: all clean
