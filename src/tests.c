#include "tests.h"

int main()
{
  initialRNG();
  int someTestsFailed = 0;

  someTestsFailed += initialiseDeck_SizeTest();
  someTestsFailed += initialiseDeck_CardsTest();

  someTestsFailed += removeCardFromDeck_SizeTest();
  someTestsFailed += removeCardFromDeck_SwapTest();

  someTestsFailed += dealCard_SizeTest();
  someTestsFailed += dealCard_CardsTest();

  someTestsFailed += removeCardTypeFromDeck_CardsTest();

  someTestsFailed += updateHandWithCard_CardsTest();

  if (!someTestsFailed)
  {
    printf("All Tests Passed!\n");
  }
  return 0;
}