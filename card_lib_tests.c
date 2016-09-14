#include "card_lib.c"

int initialiseDeck_SizeTest();
int initialiseDeck_CardsTest();
int removeCardFromDeck_SizeTest();
int removeCardFromDeck_SwapTest();

int dealCard_SizeTest();
int dealCard_CardsTest();

int removeCardTypeFromDeck_CardsTest();


int main() 
{
  initialRNG();
  int someTestsFailed = 0;

  someTestsFailed += initialiseDeck_SizeTest();
  someTestsFailed += initialiseDeck_CardsTest();

  someTestsFailed += removeCardFromDeck_SizeTest();
  someTestsFailed += removeCardFromDeck_SwapTest();

  // someTestsFailed += dealCard_SizeTest();
  // someTestsFailed += dealCard_CardsTest();

  // someTestsFailed += removeCardTypeFromDeck_CardsTest();

  if (!someTestsFailed)
  {
    printf("All Tests Passed!\n");
  }
  return 0;
}

int removeCardTypeFromDeck_CardsTest()
{
  allocDeck();
  deck->size = 0;

  deck->cards[deck->size++] = 4;
  deck->cards[deck->size++] = 7;
  deck->cards[deck->size++] = 3;
  deck->cards[deck->size++] = 9;
  deck->cards[deck->size++] = 10;
  deck->cards[deck->size++] = 11;
  deck->cards[deck->size++] = 9;
  deck->cards[deck->size++] = 9;
  deck->cards[deck->size++] = 10;
  deck->cards[deck->size++] = 2;

  if (deck->size != 10)
  {
    printf("removeCardTypeFromDeck_SizeTest 1 failed!\n");
    return 1;
  }

  removeCardTypeFromDeck(10);

  if (deck->size != 9)
  {
    printf("removeCardTypeFromDeck_SizeTest 2 failed!\n");
    return 1;
  }

  if (deck->cards[4] != 2)
  {
    printf("removeCardTypeFromDeck_CardsTest 1 failed!\n");
    return 1;
  }

  destroyDeck();
  return 0;
}

int dealCard_CardsTest()
{
  allocDeck();
  initialiseDeck();
  int* counts = malloc(sizeof(int) * 10);
  for (int i = 0; i < 10; i++) {
    counts[i] = 0;
  }

  for (int i = 0; i < (52 * 8); i++)
  {
    counts[dealCard() - 2] += 1;
  }

  for (int i = 0; i < 8; i++)
  {
    if (counts[i] != (4 * 8))
    {
      printf("dealCard_CardsTest failed!\n");
      return 1;
    }
  }

  if (counts[8] != (16 * 8))
  {
    printf("dealCard_CardsTest failed!\n");
    return 1;
  }

  if (counts[7] != (4 * 8))
  {
    printf("dealCard_CardsTest failed!\n");
    return 1;
  }

  free(counts);
  destroyDeck();
  return 0;
}

int dealCard_SizeTest() 
{
  allocDeck();
  initialiseDeck();

  for (int i = (52*8 - 1); i >= 0; i--) 
  {
    dealCard();
    if (deck->size != i)
    {
      printf("dealCard_SizeTest failed!\n");
      return 1;
    }
  }

  destroyDeck();
  return 0;
}

int removeCardFromDeck_SwapTest()
{
  allocDeck();
  initialiseDeck();

  for (int i = (52*8 - 1); i >= 0; i--) 
  {
    int randomPosition = XX_BOUNDEDRAND(i+1);
    printf("%d\n", randomPosition);

    // int sourceCard = deck->cards[i];
    // int destinationCard = deck->cards[randomPosition];

    // removeCardFromDeck(randomPosition);

    // if (deck->size != i || sourceCard != deck->cards[randomPosition] || destinationCard != deck->cards[i])
    // {
    //   printf("removeCardFromDeck_SwapTest failed!\n");
    //   return 1;
    // }
  }

  destroyDeck();
  return 0;
}

int removeCardFromDeck_SizeTest()
{
  allocDeck();
  initialiseDeck();

  for (int i = (52*8 - 1); i >= 0; i--) 
  {
    removeCardFromDeck(i);
    if (deck->size != i)
    {
      printf("removeCardFromDeck_SizeTest failed!\n");
      return 1;
    }
  }

  destroyDeck();
  return 0;
}

int initialiseDeck_SizeTest()
{
  allocDeck();
  initialiseDeck();

  if (deck->size != 52 * 8) 
  {
    printf("initialiseDeck_SizeTest failed!\n");
  }

  destroyDeck();
  return 0;
}

int initialiseDeck_CardsTest() 
{
  allocDeck();
  initialiseDeck();

  int deckPosition = 0;
  for (int i = 2; i < 10; i++) 
  {
    for (int j = 0; j < (4 * 8); j++) 
    {
      if (deck->cards[deckPosition++] != i)
      {
        printf("initialiseDeck_CardsTest failed!\n");
        return 1;
      }
    }
  }


  for (int j = 0; j < (16 * 8); j++) 
  {
    if (deck->cards[deckPosition++] != 10)
    {
      printf("initialiseDeck_CardsTest failed!\n");
      return 1;
    }
  }  

  
  for (int j = 0; j < (4 * 8); j++) 
  {
    if (deck->cards[deckPosition++] != 11)
    {
      printf("initialiseDeck_CardsTest failed!\n");
      return 1;
    }
  }

  destroyDeck();
  return 0;
}