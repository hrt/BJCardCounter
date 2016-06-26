#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pcg_variants.h"
#include "entropy.h"
#define XX_RAND_DECL                pcg32s_random_t rng;
#define XX_SEEDSDECL(seeds)         uint64_t seeds[1];
#define XX_SRANDOM_SEEDARGS(seeds)  seeds[0]
#define XX_SRANDOM_SEEDCONSTS       42u
#define XX_SRANDOM(...)             \
            pcg32s_srandom_r(&rng, __VA_ARGS__)
#define XX_RANDOM()                 \
            pcg32s_random_r(&rng)
#define XX_BOUNDEDRAND(bound)       \
            pcg32s_boundedrand_r(&rng, bound)
#define NUMBER_OF_DECKS 8
#define CARDS_PER_DECK 52
#define NTIMES 50

typedef struct {
	size_t size;
	int* cards;
} deck_t;

void initialRNG(void);
deck_t* allocDeck(void);
void initialiseDeck(void);
void addCardToDeck(int c);
int dealCard(void);
int calculateHand(int* cards, int handSize);
int simulateGame(int playerHandSize, int* player, int dealerHandSize, int* dealer, int hasSplit, int hasInsured);
int simulateDealer(int* dealer);
int simulateSplit(int* player,int* dealer);
void removeCardFromDeck(int c);


XX_RAND_DECL
int move;
deck_t* deck;
int simulations;

int main() {
	/*
	clock_t begin, end;
	double time_spent;
	begin = clock();
	*/
	initialRNG();
	deck = allocDeck();
	initialiseDeck();

	int* dealer = malloc(sizeof(int) * 18);
	int* player = malloc(sizeof(int) * 22);


	initialiseDeck();

	int playerHandSize = 0;

	while (1) {
		char str[100];
		int zs[100];
		printf("input card\n");
		scanf("%s %d", str, zs);
		int z = zs[0];
		char c = str[0];
		if (c == 'b') {
			// burn
			removeCardFromDeck(z);
		} else if (c == 'p') {
			// deal player
			removeCardFromDeck(z);
			player[playerHandSize++] = z;
		} else if (c == 'd') {
			// deal dealer
			dealer[0] = z;
		} else if (c == 's') {
			int v = simulateGame(playerHandSize, player, 1, dealer, 0, 0);
			switch (move) {
				case 0: printf("STAND"); 	break;
				case 1: printf("HIT"); 		break;
				case 2: printf("DOUBLE"); 	break;
				case 3: printf("INSURE"); 	break;
				case 4: printf("SPLIT"); 	break;
				default : printf("invalid case\n");
			}
			printf("  \tvalue : %d\n", v);
			printf("P hand : %d %d\n", player[0], player[1]);
			printf("D hand : %d\n", dealer[0]);
		} else {
			playerHandSize = 0;
		}
	}
/*
	int v = 0;
	for (int i = 0; i < 50; i++) {    		
		player[0] = dealCard();
		dealer[0] = dealCard();
		player[1] = dealCard();
		v += simulateGame(2, player, 1, dealer, 0, 0);
		deck->size += 3;
	}

	//if (v != 0) {		
		switch (move) {
			case 0: printf("STAND"); 	break;
			case 1: printf("HIT"); 		break;
			case 2: printf("DOUBLE"); 	break;
			case 3: printf("INSURE"); 	break;
			case 4: printf("SPLIT"); 	break;
			default : printf("invalid case\n");
		}
		printf("  \tvalue : %d\n", v);
		printf("P hand : %d %d\n", player[0], player[1]);
		printf("D hand : %d\n", dealer[0]);
		printf("Simulations : %d\n\n\n", simulations);
	//}
	totalSimulations += simulations;

    printf("Total Simulations : %d\n", totalSimulations);
    printf("EV : %d\n", ev);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("T : %f\n", time_spent);
*/
	return 0;
}

int compareHands(int pv, int dv, int playerHandSize) {
	if (pv > 21) {
		return -2;
	} else if (pv > dv) {
		return 2;
	} else if (dv == 100) {
		if (playerHandSize == 2 && pv == 21) {

		} else {
			return -2;
		}
	} else if (playerHandSize == 2 && pv == 21) {
		return 3;
	} else if (pv < dv) {
		return -2;
	}
	return 0;
}

int simulateGame(int playerHandSize, int* player, int dealerHandSize, int* dealer, int hasSplit, int hasInsured) {
	int currentValue = calculateHand(player, playerHandSize);
	int currentMove = 0;
	/* 0, 1, 2, 3, 4 */
	/* stand, hit, double, insure, split */

	if (currentValue >= 19) {
		// return value of standing NTIMES times
		int avg = 0;
		for (int i = 0; i < NTIMES; i++) {
			int dv = simulateDealer(dealer);
			avg += compareHands(currentValue, dv, playerHandSize);
		}
		move = 0;
		return avg / NTIMES;
	} else if (currentValue <= 8) {
		// return value of hitting NTIMES times
		int avg = 0;
		for (int i = 0; i < NTIMES; i++) {
			player[playerHandSize++] = dealCard();
			avg += simulateGame(playerHandSize, player, dealerHandSize, dealer, hasSplit, hasInsured);
			playerHandSize -= 1;
			deck->size += 1;
		}
		move = 1;
		return avg / NTIMES;
	}

	int maxVal = -2147000000;

	// attempt value of standing NTIMES times
	int avg = 0;
	for (int i = 0; i < NTIMES; i++) {
		int dv = simulateDealer(dealer);
		avg += compareHands(currentValue, dv, playerHandSize);
	}

	if (avg > maxVal) {
		maxVal = avg;
		currentMove = 0;
	}

	// attempt value of hitting NTIMES times
	avg = 0;
	for (int i = 0; i < NTIMES; i++) {
		player[playerHandSize++] = dealCard();
		avg += simulateGame(playerHandSize, player, dealerHandSize, dealer, 1, hasInsured);
		playerHandSize -= 1;
		deck->size += 1;
	}
	if (avg > maxVal) {
		maxVal = avg;
		currentMove = 1;
	}

	if (playerHandSize == 2) {
		// attempt double
		int avg = 0;
		for (int i = 0; i < NTIMES; i++) {
			player[playerHandSize++] = dealCard();
			int pv = calculateHand(player, playerHandSize);
			int dv = simulateDealer(dealer);
			avg += compareHands(pv, dv, playerHandSize);
			playerHandSize -= 1;
			deck->size += 1;
		}

		avg *= 2;
		if (avg > maxVal) {
			maxVal = avg;
			currentMove = 2;
		}
	}

	if (!hasSplit && playerHandSize == 2 && player[0] == player[1]) {
		int avg = 0;
		for (int i = 0; i < 1000; i++) {
			avg += simulateSplit(player, dealer);
		}

		if (avg > maxVal) {
			maxVal = avg;
			currentMove = 4;
		}
	}


	if (!hasInsured && playerHandSize == 2 && dealerHandSize == 1 && dealer[0] == 11) {
		// attempt value of insuring NTIMES times
	}



	move = currentMove;	// setting best move
	return maxVal;
}

int simulateSplit(int* player1, int* dealer) {
	int player1HandSize = 2;
	int player2HandSize = 2;
	int* player2 = malloc(sizeof(int) * 22);
	player2[0] = player1[0];
	player1[1] = 0;
	player1[2] = 0;

	player1[player1HandSize++] = dealCard();
	player2[player2HandSize++] = dealCard();
	int avg = 0;
	if (player1[0] == 11) {
		int dv = simulateDealer(dealer);
		int pv1 = calculateHand(player1, player1HandSize);
		int pv2 = calculateHand(player2, player2HandSize);
		avg += compareHands(pv1, dv, player1HandSize);
		avg += compareHands(pv2, dv, player2HandSize);
	} else {
		simulateGame(player1HandSize, player1, 1, dealer, 1, 0);
		while (move != 0) {
			player1[player1HandSize++] = dealCard();
			simulateGame(player1HandSize, player1, 1, dealer, 1, 0);		// has insured is always 0
		}

		simulateGame(player1HandSize, player1, 1, dealer, 1, 0);
		while (move != 0) {
			player2[player2HandSize++] = dealCard();
			simulateGame(player2HandSize, player2, 1, dealer, 1, 0);		// has insured is always 0
		}

		int dv = simulateDealer(dealer);
		int pv1 = calculateHand(player1, player1HandSize);
		int pv2 = calculateHand(player2, player2HandSize);
		avg += compareHands(pv1, dv, player1HandSize);
		avg += compareHands(pv2, dv, player2HandSize);
	}

	player1[1] = player1[0];
	free(player2);
	deck->size += player1HandSize + player2HandSize - 4;
	return avg;
}

int simulateDealer(int* dealer) {
	simulations += 1;
	int dealerHandSize = 1;
	int dv = calculateHand(dealer, dealerHandSize);
	while (dv < 17) {
		dealer[dealerHandSize++] = dealCard();
		dv = calculateHand(dealer, dealerHandSize);
	}
	deck->size += dealerHandSize - 1;
	if (dealerHandSize == 2 && dv == 21) {
		return 100;
	} else if (dv > 21) {
		return -100;
	} else {
		return dv;
	}
}


int calculateHand(int* cards, int handSize) {
	int val = 0;
	for (int i = 0; i < handSize; i++) {
		if (cards[i] == 11 && val > 10) {
			val += 1;
		} else {
			val += cards[i];
		}
	}
	return val;
}

int dealCard(void) {
	int rnd = XX_BOUNDEDRAND(deck->size);
	deck->size -= 1;
	int temp = deck->cards[deck->size];
	deck->cards[deck->size] = deck->cards[rnd];
	deck->cards[rnd] = temp;
	return deck->cards[deck->size];
}

void addCardToDeck(int c) {
	deck->cards[deck->size++] = c;
}

void removeCardFromDeck(int v) {
	for (int i = 0; i < deck->size; i++) {
		if (deck->cards[i] == v) {
			deck->size -= 1;
			int temp = deck->cards[deck->size];
			deck->cards[deck->size] = deck->cards[i];
			deck->cards[i] = temp;
			break;
		}
	}
}

void initialiseDeck(void) {
	deck->size = 0;
	for (int i = 2; i < 10; i++) {
		for (int j = 0; j < (NUMBER_OF_DECKS * 4); j++) {
			addCardToDeck(i);
		}
	}

	for (int i = 0; i < (NUMBER_OF_DECKS * 16); i++) {
		addCardToDeck(10);
	}

	for (int i = 0; i < (NUMBER_OF_DECKS * 4); i++) {
		addCardToDeck(11);
	}
}

deck_t* allocDeck(void) {
	deck_t* deck = malloc(sizeof(deck_t));
	int* cards = malloc(sizeof(int) * NUMBER_OF_DECKS * CARDS_PER_DECK);
	deck->cards = cards;
	return deck;
}

void initialRNG(void) {
	XX_SEEDSDECL(seeds)
	entropy_getbytes((void*) seeds, sizeof(seeds)); 
	XX_SRANDOM(XX_SRANDOM_SEEDARGS(seeds));
}
