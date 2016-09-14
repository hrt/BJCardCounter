#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "pcg_variants.h"
#include "entropy.h"
#define NUMBER_OF_DECKS 8
#define XX_RAND_DECL                pcg32s_random_t rng;
#define XX_SEEDSDECL(seeds)         uint64_t seeds[1];
#define XX_SRANDOM_SEEDARGS(seeds)  seeds[0]
#define XX_SRANDOM_SEEDCONSTS       42u
#define XX_SRANDOM(...)             \
            pcg32s_srandom_r(&rng, __VA_ARGS__)
#define XX_RANDOM()                 \
            pcg32s_random_r(&rng)
#define XX_BOUNDEDRAND(bound)       \
            (rand() % bound)
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

XX_RAND_DECL
deck_t* deck;


int dealCard(void)
{
  int randomPosition = XX_BOUNDEDRAND(deck->size);
  int dealtCard = deck->cards[randomPosition];
  removeCardFromDeck(randomPosition);
  return dealtCard;
}

void removeCardFromDeck(int position)
{
  deck->size -= 1;
  if (position == deck->size)
  {
    return;
  }
  deck->cards[position] ^= deck->cards[deck->size];
  deck->cards[deck->size] ^= deck->cards[position];
  deck->cards[position] ^= deck->cards[deck->size];
}

void removeCardTypeFromDeck(int card)
{
  for (int i = 0; i < deck->size; i++)
  {
    if (deck->cards[i] == card)
    {
      removeCardFromDeck(i);
      return;
    } 
  }
  printf("Attempting to remove a card that does not exist in the deck..\n");
}

void destroyDeck(void) 
{
  free(deck->cards);
  free(deck);
}

void initialiseDeck(void) 
{
  deck->size = 0;
  for (int i = 2; i < 10; i++)
  {
    for (int j = 0; j < (4 * NUMBER_OF_DECKS); j++)
      addCardToDeck(i);
  }

  for (int j = 0; j < (16 * NUMBER_OF_DECKS); j++)
    addCardToDeck(10);

  for (int j = 0; j < (4 * NUMBER_OF_DECKS); j++)
    addCardToDeck(11);
}

void allocDeck(void) 
{
  deck = malloc(sizeof(deck_t));
  deck->cards = malloc(sizeof(int) * (52 * NUMBER_OF_DECKS));
}

void initialRNG(void) 
{
  XX_SEEDSDECL(seeds)
  entropy_getbytes((void*) seeds, sizeof(seeds)); 
  XX_SRANDOM(XX_SRANDOM_SEEDARGS(seeds));
}

