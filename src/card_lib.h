#ifndef CARD_LIB_H
#define CARD_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#define NUMBER_OF_DECKS 8
#define XX_BOUNDEDRAND(bound)       \
            (rand() % (bound))
#define addCardToDeck(card)       \
            (deck->cards[deck->size++] = card)

typedef struct
{
  size_t size;
  int* cards;
} deck_t;


void initialRNG(void);
void allocDeck(void);
void initialiseDeck(void);
void destroyDeck(void);
void removeCardFromDeck(int position);
void removeCardTypeFromDeck(int c);
int dealCard(void);

deck_t* deck;

#endif