#include "hand_lib.c"
#include "card_lib.c"

int updateHandWithCard_CardsTest();

int main()
{
  initialRNG();
  int someTestsFailed = 0;


  someTestsFailed += updateHandWithCard_CardsTest();


  if (!someTestsFailed)
  {
    printf("All Tests Passed!\n");
  }
  return 0;
}


int updateHandWithCard_CardsTest()
{
  hand_t* hand = malloc(sizeof(hand_t));
  clearHand(hand);

  updateHandWithCard(hand, 7);
  updateHandWithCard(hand, 9);

  if (hand->value != 16 || hand->hasBlackjack || !(hand->canHit) || hand->hasDoubled || hand->hasSplit || hand->size != 2 || hand->isSoft)
  {
    printf("updateHandWithCard_CardsTest failed 1!\n");
    return 1;
  }

  clearHand(hand);

  updateHandWithCard(hand, 10);
  updateHandWithCard(hand, 11);

  if (hand->value != 21 || !(hand->hasBlackjack) || hand->canSplit || hand->canHit || hand->hasDoubled || hand->hasSplit || hand->size != 2 || !(hand->isSoft))
  {
    printf("updateHandWithCard_CardsTest failed 2!\n");
    return 1;
  }

  clearHand(hand);

  updateHandWithCard(hand, 11);
  updateHandWithCard(hand, 9);
  updateHandWithCard(hand, 10);
  updateHandWithCard(hand, 10);

  if (hand->value != 30 || hand->hasBlackjack || hand->size != 4)
  {
    printf("updateHandWithCard_CardsTest failed 3!\n");
    return 1;
  }

  clearHand(hand);

  updateHandWithCard(hand, 11);
  updateHandWithCard(hand, 9);
  updateHandWithCard(hand, 11);

  if (hand->value != 21 || hand->hasBlackjack || hand->size != 3)
  {
    printf("updateHandWithCard_CardsTest failed 4!\n");
    return 1;
  }

  free(hand);
  return 0;
}