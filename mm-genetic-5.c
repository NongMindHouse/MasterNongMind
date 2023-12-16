#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CODE_LENGTH 5
#define COLORS 8
#define MAX_GUESS 50
#define POPULATION_LENGTH 150
#define MAX_GEN 100

void PrintCode(int arr[])
{
  for (int i = 0; i < CODE_LENGTH; i++)
  {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

int CheckDuplicate(int code[], int guesses[MAX_GUESS][CODE_LENGTH + 2], int round)
{
  for (int i = 0; i < round; i++)
  {
    int isDup = 1;
    for (int j = 0; j < CODE_LENGTH; j++)
    {
      if (code[j] != guesses[i][j])
      {
        isDup = 0;
        break;
      }
    }
    if (isDup == 1)
      return 1;
  }

  return 0;
}

// Function to check if an individual is duplicated in the population
int CheckPopulationDuplicate(int population[POPULATION_LENGTH][CODE_LENGTH + 1],
                             int individual[CODE_LENGTH + 1], int currentSize)
{
  for (int i = 0; i < currentSize; i++)
  {
    int isSame = 1;
    for (int j = 0; j < CODE_LENGTH; j++)
    {
      if (population[i][j] != individual[j])
      {
        isSame = 0;
        break;
      }
    }
    if (isSame)
    {
      return 1; // Duplicate found
    }
  }
  return 0; // No duplicate found
}

void EvaluateGuess(int guess[], int secret[], int *black, int *white)
{
  *black = 0;
  *white = 0;

  // Arrays to keep track of which elements have been matched in guess and code arrays
  int guessMatched[CODE_LENGTH]; // 0 1
  int codeMatched[CODE_LENGTH];  // 0 1
  for (int i = 0; i < CODE_LENGTH; i++)
  {
    guessMatched[i] = 0;
    codeMatched[i] = 0;
  }

  // First, check for black pegs (correct digit in the correct position)
  for (int i = 0; i < CODE_LENGTH; i++)
  {
    if (guess[i] == secret[i])
    {
      (*black)++;
      guessMatched[i] = 1;
      codeMatched[i] = 1;
    }
  }

  // Then, check for white pegs (correct digit in the wrong position)
  for (int i = 0; i < CODE_LENGTH; i++)
  {
    for (int j = 0; j < CODE_LENGTH; j++)
    {
      if (!guessMatched[i] && !codeMatched[j] && guess[i] == secret[j])
      {
        (*white)++;
        guessMatched[i] = 1;
        codeMatched[j] = 1;
      }
    }
  }
}

void GenerateRandomGuess(int guess[CODE_LENGTH])
{
  for (int j = 0; j < CODE_LENGTH; j++)
  {
    guess[j] = rand() % COLORS;
  }
}

void GenerateRandomPopulation(int population[POPULATION_LENGTH][CODE_LENGTH])
{
  for (int i = 0; i < POPULATION_LENGTH; i++)
  {
    GenerateRandomGuess(population[i]);
    // PrintCode(population[i]);
  }
}

void FitnessScore(int test[CODE_LENGTH + 1], int guesses[MAX_GUESS][CODE_LENGTH + 2], int round)
{
  int fitness = 0;
  for (int i = 0; i < round; i++)
  {
    int black, white;
    EvaluateGuess(test, guesses[i], &black, &white);

    fitness += abs(black - guesses[i][CODE_LENGTH]);
    fitness += abs(white - guesses[i][CODE_LENGTH + 1]);
  }

  test[CODE_LENGTH] = fitness;
}

void SinglePointCrossover(int dad[], int mom[], int child_1[], int child_2[])
{
  int crossoverPoint = (rand() % CODE_LENGTH) + 1;

  for (int i = 0; i < crossoverPoint; i++)
  {
    child_1[i] = dad[i];
    child_2[i] = mom[i];
  }
  for (int i = crossoverPoint; i < CODE_LENGTH; i++)
  {
    child_1[i] = mom[i];
    child_2[i] = dad[i];
  }
}

void TwoPointCrossover(int dad[], int mom[], int child_1[], int child_2[])
{
  int crossoverPoint1 = rand() % CODE_LENGTH + 1; // Ensure it's not the first position
  int crossoverPoint2 = rand() % CODE_LENGTH + crossoverPoint1 + 2;

  // Ensure crossoverPoint2 is greater than crossoverPoint1
  if (crossoverPoint2 < crossoverPoint1)
  {
    int temp = crossoverPoint1;
    crossoverPoint1 = crossoverPoint2;
    crossoverPoint2 = temp;
  }

  for (int i = 0; i < crossoverPoint1; i++)
  {
    child_1[i] = dad[i];
    child_2[i] = mom[i];
  }

  for (int i = crossoverPoint1; i < crossoverPoint2; i++)
  {
    child_1[i] = mom[i];
    child_2[i] = dad[i];
  }

  for (int i = crossoverPoint2; i < CODE_LENGTH; i++)
  {
    child_1[i] = dad[i];
    child_2[i] = mom[i];
  }
}

void Mutate(int code[])
{
  int mutationPoint = rand() % CODE_LENGTH;
  code[mutationPoint] = rand() % COLORS;
}

// Comparator function for qsort
int compare(const void *a, const void *b)
{
  // Cast the parameters to pointers to integers
  const int *pa = (const int *)a;
  const int *pb = (const int *)b;

  // Compare the last elements
  return pa[CODE_LENGTH] - pb[CODE_LENGTH];
}

int GeneticEvolution(int eligibles[][CODE_LENGTH + 1], int guesses[MAX_GUESS][CODE_LENGTH + 2], int round, int poplen)
{
  int population[poplen][CODE_LENGTH];
  GenerateRandomPopulation(population);

  int e = 0, h = 0;
  while (e <= poplen && h < MAX_GEN)
  {
    int children[poplen][CODE_LENGTH + 1];

    // printf("====== GENERATE PHASE ======\n");
    int nzero = 0;
    for (int i = 0; i < poplen; i += 2)
    {
      // if (i == poplen - 1)
      // {
      //   // for (int j = 0; j < CODE_LENGTH; j++)
      //   // {
      //   //   children[i][j] = rand() % COLORS;
      //   // }
      // }
      // else
      // {
      // Crossover(population[i], population[i + 1], children[i], children[i + 1]);

      // TwoPointCrossover(population[i], population[rand() % (poplen)], children[i], children[i + 1]);

      // SinglePointCrossover(population[i], population[rand() % (poplen)], children[i], children[i + 1]);

      if (rand() % 2 == 0)
        TwoPointCrossover(population[i], population[rand() % (poplen)], children[i], children[i + 1]);
      else
        SinglePointCrossover(population[i], population[rand() % (poplen)], children[i], children[i + 1]);

      if (rand() % 100 < 10)
      {
        if (rand() % 2 == 0)
          Mutate(children[i]);
        else
          Mutate(children[i + 1]);
      }
      // }

      if (CheckPopulationDuplicate(children, children[i], i) == 1)
        GenerateRandomGuess(children[i]);

      if (CheckPopulationDuplicate(children, children[i + 1], i) == 1)
        GenerateRandomGuess(children[i + 1]);

      // FitnessScore(children[i], guesses, round);
      // if (children[i][CODE_LENGTH] == 0)
      //   nzero++;

      // printf("f:%d | ", children[i][CODE_LENGTH]);
      // PrintCode(children[i]);
    }

    for (int i = 0; i < poplen; i++)
    {
      FitnessScore(children[i], guesses, round);
      if (children[i][CODE_LENGTH] == 0)
        nzero++;
    }

    // printf("sorted:\n");
    // printf("nzero:%d\n", nzero);

    if (nzero == 0)
    {
      h++;
      // printf("%d\n", h);

      for (int i = 0; i < POPULATION_LENGTH; i++)
      {
        if (rand() % 2 == 0)
          GenerateRandomGuess(population[i]);
      }

      // GenerateRandomPopulation(population);

      continue;
    }

    // printf("====== NON SORT ======\n");
    // for (int i = 0; i < poplen; i++)
    // {
    //   printf("f:%d | ", children[i][CODE_LENGTH]);
    //   PrintCode(children[i]);
    // }
    qsort(children, poplen, sizeof(children[0]), compare);
    // printf("====== SORTED ======\n");
    // for (int i = 0; i < poplen; i++)
    // {
    //   printf("f:%d | ", children[i][CODE_LENGTH]);
    //   PrintCode(children[i]);
    // }

    for (int i = 0; i < nzero; i++)
    {
      // printf("e::%d\n", e);
      for (int j = 0; j < CODE_LENGTH + 1; j++)
      {
        eligibles[i][j] = children[i][j];
      }

      e++;
      // if (e < POPULATION_LENGTH - 1)
      //   e++;
      // else
      //   break;
    }

    // Create next gen population
    for (int i = 0; i < poplen; i++)
    {
      int startAlabama = (int)(poplen * 0.95);

      if (i < startAlabama)
      {
        // children -> next gen population
        for (int j = 0; j < CODE_LENGTH; j++)
        {
          population[i][j] = children[i][j];
        }
      }
      else
      {
        // top 5% of current population -> next gen population
        for (int j = 0; j < CODE_LENGTH; j++)
        {
          population[i][j] = population[i - startAlabama][j];
        }
      }
    }

    // printf("====== NEW POP ======\n");
    // for (int i = 0; i < poplen; i++)
    // {
    //   PrintCode(population[i]);
    // }

    // for (int i = 0; i < CODE_LENGTH; i++)
    // {
    //   population[poplen - 1][i] = rand() % COLORS;
    // }

    h++;
  }

  for (int i = e; i < poplen; i++)
  {
    for (int j = 0; j < CODE_LENGTH; j++)
    {
      eligibles[i][j] = rand() % COLORS;
    }
  }

  return e;
}

int main()
{
  int secret_code[CODE_LENGTH];
  int guesses[MAX_GUESS][CODE_LENGTH + 2];
  int black, white;

  for (int i = 0; i < MAX_GUESS; i++)
  {
    for (int j = 0; j < (CODE_LENGTH + 2); j++)
    {
      guesses[i][j] = 0;
    }
  }

  srand(time(NULL));
  for (int i = 0; i < CODE_LENGTH; i++)
  {
    secret_code[i] = rand() % COLORS;
  }

  printf("secret: ");
  PrintCode(secret_code);

  for (int i = 0; i < CODE_LENGTH; i++)
  {
    guesses[0][i] = rand() % COLORS;
  }

  EvaluateGuess(guesses[0], secret_code, &guesses[0][CODE_LENGTH], &guesses[0][CODE_LENGTH + 1]);
  printf("INIT b:%d w:%d | ", guesses[0][CODE_LENGTH], guesses[0][CODE_LENGTH + 1]);

  PrintCode(guesses[0]);

  int round = 1;
  while (black != CODE_LENGTH && round < MAX_GUESS)
  {
    int eligibles[POPULATION_LENGTH * 2][CODE_LENGTH + 1];

    int e = GeneticEvolution(eligibles, guesses, round, POPULATION_LENGTH);

    // printf("====== ELIGIBLEs ======\n");
    // for (int i = 0; i < POPULATION_LENGTH; i++)
    // {
    //   printf("f:%d | ", eligibles[i][CODE_LENGTH]);
    //   PrintCode(eligibles[i]);
    // }

    // if (e == 0)
    // {
    //   printf("run 2x\n");
    //   e = GeneticEvolution(eligibles, guesses, round, POPULATION_LENGTH * 2);
    // }

    printf("e:%d | ", e);
    int k = 0;
    while (CheckDuplicate(eligibles[k], guesses, round))
    {
      // printf("DUP!\n");
      k++;
    }
    // printf("k:%d | ", k);

    for (int i = 0; i < CODE_LENGTH; i++)
    {
      guesses[round][i] = eligibles[k][i];
    }

    EvaluateGuess(guesses[round], secret_code, &guesses[round][CODE_LENGTH], &guesses[round][CODE_LENGTH + 1]);
    printf("b:%d w:%d | ", guesses[round][CODE_LENGTH], guesses[round][CODE_LENGTH + 1]);
    PrintCode(eligibles[k]);

    if (guesses[round][CODE_LENGTH] == CODE_LENGTH)
    {
      PrintCode(guesses[round]);
      printf("we won in %d round\n", round + 1);
      return 0;
    }

    // printf("Ei:%d\n",)
    round++;
  }

  printf("oh no. not win\n");

  return 0;
}
