#ifndef HAND_LIB_H
#define HAND_LIB_H

typedef struct
{
  int size;
  int value;
  int isSoft;
  int canDouble;
  int canHit;
  int canSplit;
  int hasDoubled;
  int hasBlackjack;
  int hasSplit;
} hand_t;

int getGameValue(hand_t* player, hand_t* dealer);
int getSplitValue(hand_t* split1, hand_t* split2, hand_t* dealer);
void updateHandWithCard(hand_t* hand, int card);
void clearHand(hand_t* hand);

#endif