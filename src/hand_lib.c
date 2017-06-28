#include "hand_lib.h"

int getSplitValue(hand_t* split1, hand_t* split2, hand_t* dealer)
{
  // TODO: TESTS
  if (dealer->hasBlackjack)
  {
    return -2*(split1->hasDoubled + 1) -2*(split2->hasDoubled + 1);
  }

  int totalValue = 0;
  int betMultiplier1 = split1->hasDoubled + 1;
  int betMultiplier2 = split2->hasDoubled + 1;

  if (split1->value <= 21)
  {
    if (dealer->value > 21)
    {
      totalValue += 2*betMultiplier1;
    }
    else if (split1->value > dealer->value)
    {
      totalValue += 2*betMultiplier1;
    }
    else if (split1->value != dealer->value)
    {
      totalValue -= 2*betMultiplier1;
    }
  }
  else
  {
    totalValue -= 2*betMultiplier1;
  }

  if (split2->value <= 21)
  {
    if (dealer->value > 21)
    {
      totalValue += 2*betMultiplier2;
    }
    else if (split2->value > dealer->value)
    {
      totalValue += 2*betMultiplier2;
    }
    else if (split2->value != dealer->value)
    {
      totalValue -= 2*betMultiplier2;
    }
  }
  else
  {
    totalValue -= 2*betMultiplier2;
  }

  return totalValue;
}

int getGameValue(hand_t* player, hand_t* dealer)
{
  // TODO: TESTS
  int betMultiplier = player->hasDoubled + 1;
  if (player->hasBlackjack)
  {
    if (dealer->hasBlackjack)
    {
      return 0;
    }
    else
    {
      return 3;
    }
  }
  else if (dealer->hasBlackjack)
  {
    return -2 * betMultiplier;
  }

  if (player->value > 21)
  {
    return -2 * betMultiplier;
  }

  if (dealer->value > 21)
  {
    return 2 * betMultiplier;
  }

  if (player->value == dealer->value)
  {
    return 0;
  }

  if (player->value > dealer->value)
  {
    return 2 * betMultiplier;
  }

  return -2 * betMultiplier;
}

void updateHandWithCard(hand_t* hand, int card)
{
  hand->size += 1;
  hand->value += card;
  if (hand->isSoft)
  {
    if (hand->value > 21)
    {
      hand->value -= 10;
      hand->isSoft = 0;
    }
  }
  else
  {
    if (card == 11)
    {
      if (hand->value > 21)
      {
        hand->value -= 10;
      }
      else
      {
        hand->isSoft = 1;
      }
    }
  }
  if (hand->size == 2 && hand->value == 21 && !(hand->hasSplit))
  {
    hand->hasBlackjack = 1;
  }
  if (hand->value >= 19)
  {
    hand->canHit = 0;
    hand->canDouble = 0;
    hand->canSplit = 0;
  }
  if (hand->size > 2)
  {
    hand->canSplit = 0;
    hand->canDouble = 0;
  }
}

void clearHand(hand_t* hand)
{
  hand->size = 0;
  hand->value = 0;
  hand->isSoft = 0;
  hand->canDouble = 1;
  hand->canHit = 1;
  hand->canSplit = 1;
  hand->hasDoubled = 0;
  hand->hasBlackjack = 0;
  hand->hasSplit = 0;
}