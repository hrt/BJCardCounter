#ifndef UTIL_H
#define UTIL_H

#include "card_lib.h"
#include "hand_lib.h"

#define NUMBER_OF_SIMULATIONS 10000000

typedef struct
{
  int value;
  int count;
} simulation_t;


hand_t* player;
hand_t* split1;
hand_t* split2;
hand_t* dealer;

int** lookSplitTable;
int** lookDoubleTable_SOFT;
int** lookDoubleTable_HARD;
int** lookHitTable_SOFT;
int** lookHitTable_HARD;

void simulateGame(simulation_t* simulation);
void simulateDealer();
void simulatePlayer();
void simulateSplit(int dealerCard);
int lookSplit(int player, int dealer);
void initialiseLookSplitTable();
void initialiseLookDoubleTable_SOFT();
void initialiseLookDoubleTable_HARD();
void initialiseLookHitTable_HARD();
void initialiseLookHitTable_SOFT();
void destroyLookDoubleTable_SOFT();
void destroyLookDoubleTable_HARD();
void destroyLookHitTable_SOFT();
void destroyLookHitTable_HARD();
void destroyLookSplitTable();

#endif