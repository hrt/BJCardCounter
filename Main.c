#include "card_lib.c"
#include "hand_lib.c"
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

int main()
{
  initialiseLookSplitTable();
  initialiseLookDoubleTable_HARD();
  initialiseLookDoubleTable_SOFT();
  initialiseLookHitTable_SOFT();
  initialiseLookHitTable_HARD();
  printf("INPUT  \t\tAction\n");
  printf("-----  \t\t-------------------------------\n");
  printf("b X    \t\tBurn X\n");
  printf("s      \t\tBegin simulations\n");
  printf("r      \t\tReset game\n");
  initialRNG();
  player = malloc(sizeof(hand_t));
  split1 = malloc(sizeof(hand_t));
  split2 = malloc(sizeof(hand_t));
  dealer = malloc(sizeof(hand_t));

  allocDeck();
  initialiseDeck();

  char* lineBuffer = malloc(sizeof(char) * 255);
  int inputCard;
  while (fgets(lineBuffer, 6, stdin))
  {
    inputCard = (int) (lineBuffer[2] - '0');
    if (inputCard < 2)
    {
      inputCard *= 10;
      inputCard += (int) (lineBuffer[3] - '0');
    }

    switch (lineBuffer[0])
    { 
      case 'b' : 
      if (inputCard > 1 && inputCard < 12)
      {
        removeCardTypeFromDeck(inputCard);
        printf("Card %d was burnt..\n", inputCard);
      }
      break;

      case 'r' :
      {
        initialiseDeck();
        printf("Game has been reset..\n");
      }

      case 's' : 
      printf("Simulating..\n");
      simulation_t* simulation = malloc(sizeof(simulation_t));
      simulateGame(simulation);
      printf("EV       : %.3f \n", ((double) simulation->value / (double) simulation->count) * 100);
      printf("Playouts : %d\n", simulation->count);
      printf("left    : %d\n", (int) deck->size);
      free(simulation);
      break;


      default :
      break;
    }
  }

  destroyLookSplitTable();
  destroyLookDoubleTable_HARD();
  destroyLookDoubleTable_SOFT();
  destroyLookHitTable_HARD();
  destroyLookHitTable_SOFT();
  free(lineBuffer);
  destroyDeck();
  free(player);
  free(dealer);
  return 0;
}

void simulateGame(simulation_t* simulation)
{
  // assuming only one player and dealer

  int oldDeckSize = deck->size;

  simulation->count = 0;
  simulation->value = 0;

  for (int i = 0; i < NUMBER_OF_SIMULATIONS; i++)
  {
    clearHand(player);
    clearHand(dealer);
    int card1 = dealCard();
    updateHandWithCard(player, card1);
    int deal1 = dealCard();
    updateHandWithCard(dealer, deal1);
    int card2 = dealCard();
    updateHandWithCard(player, card2);
    updateHandWithCard(dealer, dealCard());

    if (card1 != card2)
    {
      player->canSplit = 0;
    }

    if (player->canSplit && lookSplit(card1, deal1))
    {
      clearHand(split1);
      clearHand(split2);

      split1->canSplit = 0;
      split1->hasSplit = 1;

      split2->canSplit = 0;
      split2->hasSplit = 1;


      updateHandWithCard(split1, card1);
      updateHandWithCard(split2, card2);

      if (card1 == 11)
      {
        split1->canHit = 0;
        split2->canHit = 0;
      }

      updateHandWithCard(split1, dealCard());
      updateHandWithCard(split2, dealCard());

      simulateSplit(deal1);

      simulateDealer(dealer);

      simulation->count += 1;
      simulation->value += getSplitValue(split1, split2, dealer);

      deck->size = oldDeckSize;
    }
    else
    {
      simulatePlayer(deal1);

      if (player->value <= 21)
      {
        simulateDealer(dealer);
        simulation->value += getGameValue(player, dealer);
      }
      else
      {
        simulation->value -= 2 * (player->hasDoubled + 1);
      }

      simulation->count += 1;

      deck->size = oldDeckSize;
    }
  }
}

void simulateDealer()
{
  // TODO: TESTS
  while (dealer->value < 17)
  {
    updateHandWithCard(dealer, dealCard());
  }
}

void simulateSplit(int dealerCard)
{
  // TODO : tests
  while (split1->canHit)
  {
    if (split1->canDouble)
    {
      if ((split1->isSoft && lookDoubleTable_SOFT[split1->value][dealerCard]) || (!(split1->isSoft) && lookDoubleTable_HARD[split1->value][dealerCard]))
      {
        split1->canDouble = 0;
        split1->hasDoubled = 1;
        split1->canHit = 0;
        updateHandWithCard(split1, dealCard());
        continue;
      }
    }

    if ((split1->isSoft && lookHitTable_SOFT[split1->value][dealerCard]) || (!(split1->isSoft) && lookHitTable_HARD[split1->value][dealerCard]))
    {
      split1->canDouble = 0;
      updateHandWithCard(split1, dealCard());
      continue;
    }
    split1->canHit = 0;
  }

  while (split2->canHit)
  {
    if (split2->canDouble)
    {
      if ((split2->isSoft && lookDoubleTable_SOFT[split2->value][dealerCard]) || (!(split2->isSoft) && lookDoubleTable_HARD[split2->value][dealerCard]))
      {
        split2->canDouble = 0;
        split2->hasDoubled = 1;
        split2->canHit = 0;
        updateHandWithCard(split2, dealCard());
        continue;
      }
    }


    if ((split2->isSoft && lookHitTable_SOFT[split2->value][dealerCard]) || (!(split2->isSoft) && lookHitTable_HARD[split2->value][dealerCard]))
    {
      split2->canDouble = 0;
      updateHandWithCard(split2, dealCard());
      continue;
    }
    split2->canHit = 0;
  }
}

void simulatePlayer(int dealerCard)
{
  // TODO : tests
  while (player->canHit)
  {
    if (player->canDouble)
    {
      if ((player->isSoft && lookDoubleTable_SOFT[player->value][dealerCard]) || (!(player->isSoft) && lookDoubleTable_HARD[player->value][dealerCard]))
      {
        player->canDouble = 0;
        player->hasDoubled = 1;
        player->canHit = 0;
        updateHandWithCard(player, dealCard());
        continue;
      }
    }

    if ((player->isSoft && lookHitTable_SOFT[player->value][dealerCard]) || (!(player->isSoft) && lookHitTable_HARD[player->value][dealerCard]))
    {
      player->canDouble = 0;
      updateHandWithCard(player, dealCard());
      continue;
    }
    player->canHit = 0;
  }
}

int lookSplit(int player, int dealer)
{
  // TODO : test
  return lookSplitTable[player][dealer];
}

void initialiseLookSplitTable()
{
  // TODO : test
  lookSplitTable = malloc(sizeof(int*) * 12);
  for (int i = 0; i < 12; i++)
  {
    lookSplitTable[i] = malloc(sizeof(int) * 12);
  }

  for (int i = 2; i < 12; i++)
  {
    for (int j = 2; j < 12; j++)
    {
      lookSplitTable[i][j] = 1;
    }
  }

  for (int i = 8; i < 12; i++)
  {
    lookSplitTable[2][i] = 0;
    lookSplitTable[3][i] = 0;
    lookSplitTable[4][i] = 0;
    lookSplitTable[6][i] = 0;
    lookSplitTable[7][i] = 0;
  }

  lookSplitTable[4][7] = 0;
  lookSplitTable[6][7] = 0;

  for (int i = 2; i < 5; i++)
  {
    lookSplitTable[4][i] = 0;
  }

  lookSplitTable[9][7] = 0;
  lookSplitTable[9][10] = 0;
  lookSplitTable[9][11] = 0;

  for (int i = 2; i < 12; i++)
  {
    lookSplitTable[5][i] = 0;
    lookSplitTable[10][i] = 0;
  }
}


void destroyLookSplitTable()
{
  // TODO : test
  for (int i = 0; i < 12; i++)
  {
    free(lookSplitTable[i]);
  }
  free(lookSplitTable);
}

void initialiseLookDoubleTable_HARD()
{
  // TODO : test
  lookDoubleTable_HARD = malloc(sizeof(int*) * 27);
  for (int i = 0; i < 27; i++)
  {
    lookDoubleTable_HARD[i] = malloc(sizeof(int) * 12);
  }

  for (int i = 2; i < 27; i++)
  {
    for (int j = 2; j < 12; j++)
    {
      lookDoubleTable_HARD[i][j] = 0;
    }
  }

  for (int i = 2; i < 11; i++)
  {
    lookDoubleTable_HARD[11][i] = 1;
    lookDoubleTable_HARD[10][i] = 1;
  }
  
  lookDoubleTable_HARD[10][10] = 0;

  for (int i = 3; i < 7; i++)
  {
    lookDoubleTable_HARD[9][i] = 1;
  }
}

void destroyLookDoubleTable_HARD()
{
  // TODO : test
  for (int i = 0; i < 27; i++)
  {
    free(lookDoubleTable_HARD[i]);
  }
  free(lookDoubleTable_HARD);
}

void initialiseLookDoubleTable_SOFT()
{
  // TODO : test
  lookDoubleTable_SOFT = malloc(sizeof(int*) * 27);
  for (int i = 0; i < 27; i++)
  {
    lookDoubleTable_SOFT[i] = malloc(sizeof(int) * 12);
  }

  for (int i = 2; i < 27; i++)
  {
    for (int j = 2; j < 12; j++)
    {
      lookDoubleTable_SOFT[i][j] = 0;
    }
  }

  for (int i = 13; i < 19; i++)
  {
    for (int j = 3; j < 7; j++)
    {
      lookDoubleTable_SOFT[i][j] = 1;
    }
  }

  for (int i = 13; i < 17; i++)
  {
    lookDoubleTable_SOFT[i][3] = 0;
  }

  lookDoubleTable_SOFT[13][4] = 0;
  lookDoubleTable_SOFT[14][4] = 0;
}

void destroyLookDoubleTable_SOFT()
{
  // TODO : test
  for (int i = 0; i < 27; i++)
  {
    free(lookDoubleTable_SOFT[i]);
  }
  free(lookDoubleTable_SOFT);
}

void initialiseLookHitTable_HARD()
{
  // TODO : test
  lookHitTable_HARD = malloc(sizeof(int*) * 27);
  for (int i = 0; i < 27; i++)
  {
    lookHitTable_HARD[i] = malloc(sizeof(int) * 12);
  }

  for (int i = 2; i < 27; i++)
  {
    for (int j = 2; j < 12; j++)
    {
      lookHitTable_HARD[i][j] = 0;
    }
  }

  for (int i = 4; i < 17; i++)
  {
    for (int j = 2; j < 12; j++)
    {
      lookHitTable_HARD[i][j] = 1;
    }
  }

  for (int i = 13; i < 17; i++)
  {
    for (int j = 2; j < 7; j++)
    {
      lookHitTable_HARD[i][j] = 0;
    }
  }

  for (int i = 4; i < 7; i++)
  {
    lookHitTable_HARD[12][i] = 0;
  }
}

void destroyLookHitTable_HARD()
{
  // TODO : test
  for (int i = 0; i < 27; i++)
  {
    free(lookHitTable_HARD[i]);
  }
  free(lookHitTable_HARD);
}

void initialiseLookHitTable_SOFT()
{
  // TODO : test
  lookHitTable_SOFT = malloc(sizeof(int*) * 27);
  for (int i = 0; i < 27; i++)
  {
    lookHitTable_SOFT[i] = malloc(sizeof(int) * 12);
  }

  for (int i = 2; i < 27; i++)
  {
    for (int j = 2; j < 12; j++)
    {
      lookHitTable_SOFT[i][j] = 0;
    }
  }

  for (int i = 13; i < 18; i++)
  {
    for (int j = 2; j < 12; j++)
    {
      lookHitTable_SOFT[i][j] = 1;
    }
  }

  for (int i = 9; i < 12; i++)
  {
    lookHitTable_SOFT[18][i] = 1;
  }
}

void destroyLookHitTable_SOFT()
{
  // TODO : test
  for (int i = 0; i < 27; i++)
  {
    free(lookHitTable_SOFT[i]);
  }
  free(lookHitTable_SOFT);
}
