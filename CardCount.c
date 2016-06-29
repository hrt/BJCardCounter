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
#define NTIMES 2
typedef struct {
	size_t size;
	int* cards;
} deck_t;

int simulateGame(int* player, int playerHandSize, int* dealer, int hasSplit);
int simulateGameAuxilery(int* player, int playerHandSize, int* dealer, int hasSplit, int isStart);
int calculateHand(int* cards, int handSize);
int calculateHandHelper(int* cards, int start, int end);
int calculateHandHelper2(int* cards, int start, int end);
void initialRNG(void);
deck_t* allocDeck(void);
void initialiseDeck(void);
void addCardToDeck(int c);
int dealCard(void);
int simulateDealer(int* dealer);
int simulateSplit(int* player,int* dealer);
void removeCardFromDeck(int c);
int compareHands(int pv, int dv, int playerHandSize, int hasSplit);

XX_RAND_DECL
deck_t* deck;
int move;
int* player2;
int* values;
clock_t begin, end;
double time_spent;
int* yz;

int main() {

	initialRNG();
	deck = allocDeck();
	initialiseDeck();

	yz = malloc(sizeof(int) * 50);
	int* dealer = malloc(sizeof(int) * 18);
	int* player = malloc(sizeof(int) * 22);
	player2 = malloc(sizeof(int) * 22);
	values = malloc(sizeof(int) * 5);

	int playerHandSize = 0;

	while (1) {
		printf("Input instruction : (d, p, b, r, c)\n");
		char str[100];
		int zs[100];

		int z = scanf("%s", str);

		switch (str[0]) {
			case 'b': 
				printf("Input card to burn :\n");
				z = scanf("%d", zs);
				z = zs[0];
				removeCardFromDeck(z);
				break;

			case 'p' : 
				printf("Input card for Player :\n");
				z = scanf("%d", zs);
				z = zs[0];
				removeCardFromDeck(z);
				player[playerHandSize++] = z;
				break;

			case 't' :
				printf("Input card for Player :\n");
				z = scanf("%d", zs);
				z = zs[0];
				player[playerHandSize++] = z;
				break;				

			case 'd' :
				printf("Input card for Dealer :\n");
				z = scanf("%d", zs);
				z = zs[0];
				removeCardFromDeck(z);
				dealer[0] = z;
				break;

			case 'r' :
				playerHandSize = 0;
				break;

			case 'c' :
			{
				printf("Previously split? : (0 or 1)\n");
				z = scanf("%d", zs);
				z = zs[0];
				int hasSplit = z;
				begin = clock();
				int v = simulateGame(player, playerHandSize, dealer, hasSplit);
				switch (move) {
					case 0: printf("STAND"); 	break;
					case 1: printf("HIT"); 		break;
					case 2: printf("DOUBLE"); 	break;
					case 3: printf("SPLIT"); 	break;
					default : printf("invalid case\n");
				}
				printf("  \tvalue : %d\n", v);
				printf("P hand :");
				for (int i = 0; i < playerHandSize; i++) {
					printf(" %d", player[i]);
				}
				printf("\nD hand : %d\n", dealer[0]);
				end = clock();
				time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				printf("T : %f\n", time_spent);
			}
		}
	}
	return 0;
}
int simulateGame(int* player, int playerHandSize, int* dealer, int hasSplit) {
	values[0] = 0;
	values[1] = -2147000000;
	values[2] = -2147000000;
	values[3] = -2147000000;

	if (playerHandSize == 2) {
		if (player[0] == player[1]) {
			values[3] = 0;
		}
		values[2] = 0;
	}
	int cv = calculateHand(player, playerHandSize);
	if (cv < 19) {
		values[1] = 0;
	}

	if (cv < 12) {
		values[0] = -2147000000;
	}
	int k = 10;
	double time_spent2 = 0;
	while (1) {
		dealer[1] = dealCard();
		simulateGameAuxilery(player, playerHandSize, dealer, hasSplit, 1);
		deck->size += 1;
		end = clock();
		time_spent = ((double)(end - begin) / CLOCKS_PER_SEC) - time_spent2;
		if (time_spent > k) {
			clock_t begin2, end2;
			begin2 = clock();
			printf("Continue? (How much longer to spend) : \n");
			int z = scanf("%d", yz);
			z = yz[0];
			if (!z) {
				break;
			}
			end2 = clock();
			time_spent2 += (double)(end2 - begin2) / CLOCKS_PER_SEC;
			k += z;
		}
	}

	int maxVal = -2147000004;

	if (values[0] > maxVal) {
		maxVal = values[0];
		move = 0;
	}

	if (values[1] > maxVal) {
		maxVal = values[1];
		move = 1;
	}

	if (values[2] > maxVal) {
		maxVal = values[2];
		move = 2;
	}

	if (values[3] > maxVal) {
		maxVal = values[3];
		move = 3;
	}

	return maxVal;
}

int simulateGameAuxilery(int* player, int playerHandSize, int* dealer, int hasSplit, int isStart) {
	int currentValue = calculateHand(player, playerHandSize);
	int currentMove = -1;
	int avg;
	if (currentValue >= 19) {
		avg = 0;
		for (int i = 0; i < NTIMES; i++) {
			int dv = simulateDealer(dealer);
			avg += compareHands(currentValue, dv, playerHandSize, hasSplit);
		}

		if (isStart) {
			values[0] += avg;
		}
		move = 0;
		return avg;
	} else if (currentValue <= 8) {
		avg = 0;
		for (int i = 0; i < NTIMES; i++) {
			player[playerHandSize++] = dealCard();
			avg += simulateGameAuxilery(player, playerHandSize, dealer, hasSplit, 0);
			playerHandSize -= 1;
			deck->size += 1;
		}
		avg /= NTIMES;

		if (isStart) {
			values[1] += avg;
		}
		move = 1;
		return avg;
	}


	int maxVal = -2147000001;

	/****************STAND***********/
	avg = 0;
	for (int i = 0; i < NTIMES; i++) {
		int dv = simulateDealer(dealer);
		avg += compareHands(currentValue, dv, playerHandSize, hasSplit);
	}

	if (isStart) {
		values[0] += avg;
	}

	if (avg > maxVal) {
		currentMove = 0;
		maxVal = avg;
	}
	/********************************/


	/****************HIT*************/
	avg = 0;
	for (int i = 0; i < NTIMES; i++) {
		player[playerHandSize++] = dealCard();
		avg += simulateGameAuxilery(player, playerHandSize, dealer, hasSplit, 0);
		playerHandSize -= 1;
		deck->size += 1;
	}
	avg /= NTIMES;
	if (isStart) {
		values[1] += avg;
	}

	if (avg > maxVal) {
		currentMove = 1;
		maxVal = avg;
	}

	/********************************/

	/**************DOUBLE************/
	if (playerHandSize == 2) {
		avg = 0;
		for (int i = 0; i < NTIMES; i++) {
			player[playerHandSize++] = dealCard();
			int pv = calculateHand(player, playerHandSize);
			int dv = simulateDealer(dealer);
			avg += compareHands(pv, dv, playerHandSize, hasSplit);
			playerHandSize -= 1;
			deck->size += 1;
		}
		avg *= 2;
		if (isStart) {
			values[2] += avg;
		}

		if (avg > maxVal) {
			currentMove = 2;
			maxVal = avg;
		}
	}
	/********************************/

	/***************SPLIT************/
	if (!hasSplit && playerHandSize == 2 && player[0] == player[1]) {
		avg = 0;
		for (int i = 0; i < NTIMES; i++) {
			avg += simulateSplit(player, dealer);
		}

		avg /= NTIMES;

		if (isStart) {
			values[3] += avg;
		}

		if (avg > maxVal) {
			currentMove = 3;
			maxVal = avg;
		}
	}
	/********************************/

	move = currentMove;
	return maxVal;
}

int simulateSplit(int* player1,int* dealer) {
	int ogSize = deck->size;
	int player1HandSize = 1;
	int player2HandSize = 1;
	player2[0] = player1[0];
	player1[player1HandSize++] = dealCard();
	player2[player2HandSize++] = dealCard();

	int val = 0;
	int multiplier1 = 1;
	int multiplier2 = 1;

	if (player1[0] == 11) {
		for (int i = 0; i < NTIMES; i++) {		
			int dv = simulateDealer(dealer);
			int pv1 = calculateHand(player1, player1HandSize);
			int pv2 = calculateHand(player2, player2HandSize);
			val += compareHands(pv1, dv, player1HandSize, 1) * multiplier1;
			val += compareHands(pv2, dv, player2HandSize, 1) * multiplier2;
		}
	} else {
		simulateGameAuxilery(player1, player1HandSize, dealer, 1, 0);
		while (move != 0) {
			if (move == 1) {
				player1[player1HandSize++] = dealCard();
				simulateGameAuxilery(player1, player1HandSize, dealer, 1, 0);
			} else if (move == 2) {
				multiplier1 = 2;
				player1[player1HandSize++] = dealCard();
				break;
			}
		}

		simulateGameAuxilery(player2, player2HandSize, dealer, 1, 0);
		while (move != 0) {
			if (move == 1) {
				player2[player2HandSize++] = dealCard();
				simulateGameAuxilery(player2, player2HandSize, dealer, 1, 0);
			} else if (move == 2) {
				multiplier2 = 2;
				player2[player2HandSize++] = dealCard();
				break;
			}
		}

		for (int i = 0; i < NTIMES; i++) {		
			int dv = simulateDealer(dealer);
			int pv1 = calculateHand(player1, player1HandSize);
			int pv2 = calculateHand(player2, player2HandSize);
			val += compareHands(pv1, dv, player1HandSize, 1) * multiplier1;
			val += compareHands(pv2, dv, player2HandSize, 1) * multiplier2;
		}
	}

	player1[1] = player1[0];
	deck->size = ogSize;

	return val;
}


int compareHands(int pv, int dv, int playerHandSize, int hasSplit) {
	if (pv > 21) {
		return -2;
	} else if (playerHandSize == 2 && pv == 21 && !hasSplit) {
		if (dv == 100) {
		} else {
			return 3;
		}
	} else if (dv == 100) {
		return -2;
	} else if (pv > dv) {
		return 2;
	} else if (pv < dv) {
		return -2;
	}
	return 0;
}

int simulateDealer(int* dealer) {
	int originalSize = deck->size;
	int dealerHandSize = 2;
	int dv = calculateHand(dealer, dealerHandSize);
	while (dv < 17) {
		dealer[dealerHandSize++] = dealCard();
		dv = calculateHand(dealer, dealerHandSize);
	}
	deck->size = originalSize;
	if (dealerHandSize == 2 && dv == 21) {
		return 100;
	} else if (dv > 21) {
		return -100;
	} else {
		return dv;
	}
}

int calculateHand(int* cards, int handSize) {
	return calculateHandHelper(cards, 0, handSize);
}

int calculateHandHelper(int* cards, int start, int end) {
	int v = 0;
	for (int i = start; i < end; i++) {
		if (cards[i] == 11) {
			v += calculateHandHelper2(cards, i + 1, end);
			if (v > 10) {
				v += 1;
			} else {
				v += 11;
			}
			break;
		} else {
			v += cards[i];
		}
	}
	return v;
}

int calculateHandHelper2(int* cards, int start, int end) {
	int v = 0;
	for (int i = start; i < end; i++) {
		if (cards[i] == 11) {
			v += 1;
		} else {
			v += cards[i];
		}
	}
	return v;
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
