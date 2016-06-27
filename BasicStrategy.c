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
#define NUMBER_OF_SHOES 1000000
#define NUMBER_OF_HANDS 50

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
int calculateHandHelper(int* cards, int start, int end);
int calculateHandHelper2(int* cards, int start, int end);
int simulateDealer(int* dealer);
void removeCardFromDeck(int c);
int basicStrategy(int* player, int playerHandSize, int* dealer);
int compareHands(int pv, int dv, int playerHandSize);

int soft;
XX_RAND_DECL
deck_t* deck;

int main(void) {
	clock_t begin, end;
	double time_spent;
	initialRNG();
	deck = allocDeck();
	begin = clock();
	int* dealer = malloc(sizeof(int) * 18);
	int* player = malloc(sizeof(int) * 22);
	int wins = 0;
	int losses = 0;

	for (int i = 0; i < NUMBER_OF_SHOES; i++) {
		initialiseDeck();
		for (int i = 0; i < NUMBER_OF_HANDS; i++) {
			int betSize = 1;
			int playerHandSize = 0;
			int dealerHandSize = 0;
			player[playerHandSize++] = dealCard();
			dealer[dealerHandSize++] = dealCard();
			player[playerHandSize++] = dealCard();
			dealer[dealerHandSize++] = dealCard();

			int move = basicStrategy(player, playerHandSize, dealer);

			if (move == 2) { //double down
				betSize *= 2;
				player[playerHandSize++] = dealCard();
				int pv = calculateHand(player, playerHandSize);
				int dv = simulateDealer(dealer);
				int v = compareHands(pv, dv, playerHandSize);
				if (v < 0) {
					losses -= betSize * v;
				} else if (v > 0) {
					wins += betSize * v;
				}
			} else if (move == 0) { //stand
				int pv = calculateHand(player, playerHandSize);
				int dv = simulateDealer(dealer);
				int v = compareHands(pv, dv, playerHandSize);
				if (v < 0) {
					losses -= betSize * v;
				} else if (v > 0) {
					wins += betSize * v;
				}
			} else if (move == 1) { //hit
				while (move != 0)  {				
					player[playerHandSize++] = dealCard();
					move = basicStrategy(player, playerHandSize, dealer);
				}
				int pv = calculateHand(player, playerHandSize);
				int dv = simulateDealer(dealer);
				int v = compareHands(pv, dv, playerHandSize);
				if (v < 0) {
					losses -= betSize * v;
				} else if (v > 0) {
					wins += betSize * v;
				}
			} else if (move == 4) { //split
				int* player2 = malloc(sizeof(int) * 22);
				int player2HandSize = 0;
				player2[player2HandSize++] = player[0];
				player2[player2HandSize++] = 0;
				player[1] = 0;

				player2[player2HandSize++] = dealCard();
				player[playerHandSize++] = dealCard();

				int move2 = basicStrategy(player, playerHandSize, dealer);
				while (move2 != 0) {
					player[playerHandSize++] = dealCard();
					move2 = basicStrategy(player, playerHandSize, dealer);
				}

				int move3 = basicStrategy(player2, player2HandSize, dealer);
				while (move3 != 0) {
					player2[player2HandSize++] = dealCard();
					move3 = basicStrategy(player2, player2HandSize, dealer);
				}

				int pv = calculateHand(player, playerHandSize);
				int pv2 = calculateHand(player2, player2HandSize);
				int dv = simulateDealer(dealer);
				int v = compareHands(pv, dv, playerHandSize);
				v += compareHands(pv2, dv, player2HandSize);
				if (v < 0) {
					losses -= betSize * v;
				} else if (v > 0) {
					wins += betSize * v;
				}
				free(player2);
			}
		}
	}

	printf("W : %d\n", wins);
	printf("L : %d\n", losses);
	printf("EV: %f\n", (((double) wins) / ((double) wins + (double) losses)) * 100);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("T : %f\n", time_spent);
	return 0;
}

int basicStrategy(int* player, int playerHandSize, int* dealer) {
	int pv = calculateHand(player, playerHandSize);
	if (player[0] == player[1] && playerHandSize == 2) {
		if (player[0] == 11) {
			if (dealer[0] == 11) {
				return 1;
			} else {
				return 4;
			}
		} else if (player[0] == 5) {
			if (dealer[0] >= 10) {
				return 1;
			} else {
				return 2;
			}
		} else if (player[0] <= 3) {
			if (dealer[0] <= 3) {
				return 1;
			} else if (dealer[0] <= 7) {
				return 4;
			} else {
				return 1;
			}
		} else if (player[0] <= 4) {
			return 1;
		} else if (player[0] <= 6) {
			if (dealer[0] <= 2) {
				return 1;
			} else if (dealer[0] <= 6) {
				return 4;
			} else {
				return 1;
			}
		} else if (player[0] <= 7) {
			if (dealer[0] <= 7) {
				return 4;
			} else {
				return 1;
			}
		} else if (player[0] <= 8) {
			if (dealer[0] <= 9) {
				return 4;
			} else {
				return 1;
			}
		} else if (player[0] <= 9) {
			if (dealer[0] == 7 || dealer[0] >= 10) {
				return 0;
			} else {
				return 4;
			}
		} else if (player[0] <= 10) {
			return 0;
		}
	}

	if (soft) {
		if (pv <= 14) {
			if (dealer[0] <= 4) {
				return 1;
			} else if (dealer[0] <= 6) {
				if (playerHandSize == 2) {
					return 2;
				} else {
					return 1;
				}
			} else {
				return 1;
			}
		} else if (pv <= 16) {
			if (dealer[0] <= 3) {
				return 1;
			} else if (dealer[0] <= 6) {
				if (playerHandSize == 2) {
					return 2;
				} else {
					return 1;
				}
			} else {
				return 1;
			}
		} else if (pv <= 17) {
			if (dealer[0] <= 2) {
				return 1;
			} else if (dealer[0] <= 6) {
				if (playerHandSize == 2) {
					return 2;
				} else {
					return 1;
				}
			} else {
				return 1;
			}
		} else if (pv <= 18) {
			if (dealer[0] <= 2) {
				return 0;
			} else if (dealer[0] <= 6) {
				if (playerHandSize == 2) {
					return 2;
				} else {
					return 0;
				}
			} else if (dealer[0] <= 8) {
				return 0;
			} else {
				return 1;
			}
		} else {
			return 0;
		}
	} else {
		if (pv <= 8) {
			return 1;
		} else if (pv <= 9) {
			if (dealer[0] <= 2) {
				return 1;
			} else if (dealer[0] <= 6) {
				if (playerHandSize == 2) {
					return 2;
				} else {
					return 1;
				}
			} else {
				return 1;
			}
		} else if (pv <= 11) {
			if (dealer[0] <= 9) {
				if (playerHandSize == 2) {
					return 2;
				} else {
					return 1;
				}
			} else {
				return 1;
			}
		} else if (pv <= 12) {
			if (dealer[0] <= 3) {
				return 1;
			} else if (dealer[0] <= 6) {
				return 0;
			} else {
				return 1;
			}
		} else if (pv <= 16) {
			if (dealer[0] <= 6) {
				return 0;
			} else {
				return 1;
			}
		} else {
			return 0;
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

int calculateHand(int* cards, int handSize) {
	return calculateHandHelper(cards, 0, handSize);
}

/* 7 11 11 3*/
/*    12    */
/*    22    */
int calculateHandHelper(int* cards, int start, int end) {
	int v = 0;
	for (int i = start; i < end; i++) {
		if (cards[i] == 11) {
			v += calculateHandHelper2(cards, i + 1, end);
			if (v > 10) {
				v += 1;
				soft = 0;
			} else {
				v += 11;
				soft = 1;
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

int simulateDealer(int* dealer) {
	int dealerHandSize = 2;
	int dv = calculateHand(dealer, dealerHandSize);
	while (dv < 17) {
		dealer[dealerHandSize++] = dealCard();
		dv = calculateHand(dealer, dealerHandSize);
	}
	//deck->size += dealerHandSize - 1;
	if (dealerHandSize == 2 && dv == 21) {
		return 100;
	} else if (dv > 21) {
		return -100;
	} else {
		return dv;
	}
}

int compareHands(int pv, int dv, int playerHandSize) {
	if (pv > 21) {
		return -2;
	} else if (playerHandSize == 2 && pv == 21) {
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