#include "card_lib.h"

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
  srand(time(NULL));
}

