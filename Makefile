CC      = gcc
CFLAGS  = -std=c99 -Wall -Werror

.SUFFIXES: .c .h

count: 
	$(CC) Main.c $(CFLAGS) -O3 -o count
	$(CC) card_lib_tests.c $(CFLAGS) -O3 -o tests_card
	$(CC) hand_lib_tests.c $(CFLAGS) -O3 -o tests_hand

debug: 
	$(CC) Main.c $(CFLAGS) -g -o count
	$(CC) card_lib_tests.c $(CFLAGS) -g -o tests_card
	$(CC) hand_lib_tests.c $(CFLAGS) -g -o tests_hand

clean:
	rm -f test
	rm -f count
	rm -f count.txt
	rm -f tests_card
	rm -f tests_hand

.PHONY: all clean
