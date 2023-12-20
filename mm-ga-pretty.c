#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int CODE_LENGTH = 5;
int COLORS = 8;

int MAX_GUESS = 10;
int POPULATION_LENGTH = 300;
int MAX_GEN = 200;

float MUTATION_RATE = 0.1;

#define BLACK_WEIGHT 3
#define WHITE_WEIGHT 2

int FANCY_MODE = 1;

// Just for printing array
void PrintCode(int arr[])
{
  const char emojis[][5] = {"\U0001F534", "\U0001F7E0", "\U0001F7E1", "\U0001F7E2", "\U0001F535", "\U0001F7E3", "\U0001F7E4", "\U000026AB", "\U000026AA"};

  for (int i = 0; i < CODE_LENGTH; i++)
  {
    if (FANCY_MODE == 1)
    {
      printf("%s ", emojis[arr[i]]);
    }
    else
    {
      printf("%d ", arr[i]);
    }
  }
  printf("\n");
}

// Check if the guess is duplicated in the past guesses
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

// Check if an individual is duplicated in the population
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

// Generate random one random guess
void GenerateRandomGuess(int guess[CODE_LENGTH])
{
  for (int j = 0; j < CODE_LENGTH; j++)
  {
    guess[j] = rand() % COLORS;
  }
}

// Generate population consists of random guess
void GenerateRandomPopulation(int population[POPULATION_LENGTH][CODE_LENGTH])
{
  for (int i = 0; i < POPULATION_LENGTH; i++)
  {
    GenerateRandomGuess(population[i]);
  }
}

// Calculate fitness score
void FitnessScore(int individual[CODE_LENGTH + 1], int guesses[MAX_GUESS][CODE_LENGTH + 2], int round)
{
  int fitness = 0;
  for (int i = 0; i < round; i++)
  {
    int black, white;
    EvaluateGuess(individual, guesses[i], &black, &white);

    fitness += abs(black - guesses[i][CODE_LENGTH]) * BLACK_WEIGHT;
    fitness += abs(white - guesses[i][CODE_LENGTH + 1]) * WHITE_WEIGHT;
  }

  individual[CODE_LENGTH] = fitness;
}

// Do single point crossover
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

// Do two point crossover
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

// Mutate individual
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

// Entire genetic algorithm thing is here
int GeneticEvolution(int eligibles[][CODE_LENGTH + 1], int guesses[MAX_GUESS][CODE_LENGTH + 2], int round, int poplen)
{
  // Create initial population by random
  int population[poplen][CODE_LENGTH];
  GenerateRandomPopulation(population);

  // e = eligibles guess, h = generation count
  int e = 0, h = 0;

  // Evolution loop (basically do repdoduce MAX_GEN times)
  while (e <= poplen && h < MAX_GEN)
  {
    // Create next-gen population array
    int children[poplen][CODE_LENGTH + 1];

    // Do some reproducing in the population
    for (int i = 0; i < poplen; i += 2)
    {
      // Crossover to create next-gen population 50% random between single and two point
      if (rand() % 2 == 0)
        TwoPointCrossover(population[i], population[rand() % (poplen)], children[i], children[i + 1]);
      else
        SinglePointCrossover(population[i], population[rand() % (poplen)], children[i], children[i + 1]);

      // Randomly mutate individual
      if (rand() % 100 < MUTATION_RATE * 100)
      {
        if (rand() % 2 == 0)
          Mutate(children[i]);
        else
          Mutate(children[i + 1]);
      }

      // Check if current entry is duplicated in the generation if yes then replate with random
      if (CheckPopulationDuplicate(children, children[i], i) == 1)
        GenerateRandomGuess(children[i]);

      if (CheckPopulationDuplicate(children, children[i + 1], i) == 1)
        GenerateRandomGuess(children[i + 1]);
    }

    // Number of individual that have fitness score = 0
    int nzero = 0;

    // Calculate fitness score entire population
    for (int i = 0; i < poplen; i++)
    {
      FitnessScore(children[i], guesses, round);
      if (children[i][CODE_LENGTH] == 0)
        nzero++;
    }

    // If there isnt any fitness score = 0 individual
    // means this generation is not good enough.
    // So, we basically THANOS half of the generation
    // and replace with random.
    if (nzero == 0)
    {
      for (int i = 0; i < POPULATION_LENGTH; i++)
      {
        if (rand() % 2 == 0)
          GenerateRandomGuess(population[i]);
      }

      h++;
      continue;
    }

    // Sort for easy access to zero fitness score individual
    qsort(children, poplen, sizeof(children[0]), compare);

    // Copy eligible individual to eligible list
    for (int i = 0; i < nzero; i++)
    {
      for (int j = 0; j < CODE_LENGTH + 1; j++)
      {
        eligibles[i][j] = children[i][j];
      }

      e++;
    }

    // Create next-gen population
    // I found that most paper called this method 'elitism',
    // but I call it Alabama method cuz we just add the parent into repoducing with their children.
    // This is just a joke don't sue me, please.
    for (int i = 0; i < poplen; i++)
    {
      // Create alabama point 5% of the next-gen population gonna be best 5% from this gen
      int alabamaPoint = (int)(poplen * 0.95);

      if (i < alabamaPoint)
      {
        // Children of this gen become next-gen population
        for (int j = 0; j < CODE_LENGTH; j++)
        {
          population[i][j] = children[i][j];
        }
      }
      else
      {
        // Best 5% of current gen become next-gen population
        for (int j = 0; j < CODE_LENGTH; j++)
        {
          population[i][j] = population[i - alabamaPoint][j];
        }
      }
    }

    h++;
  }

  // In case of when we cannot obtain enough eligibles,
  // we'll fill it up with random guess. (leaving them blank leads to bugs)
  for (int i = e; i < poplen; i++)
  {
    for (int j = 0; j < CODE_LENGTH; j++)
    {
      eligibles[i][j] = rand() % COLORS;
    }
  }

  return e;
}

void PrintGuessData(int guess[CODE_LENGTH + 2])
{
  if (FANCY_MODE == 1)
  {
    printf("%d \U000026AB, ", guess[CODE_LENGTH]);
    printf("%d \U000026AA\t| ", guess[CODE_LENGTH + 1]);
  }
  else
  {
    printf("b:%d w:%d\t| ", guess[CODE_LENGTH], guess[CODE_LENGTH + 1]);
  }

  PrintCode(guess);
}

int main(int argc, char *argv[])
{
  int SEED;
  // This part use to automate benchmarking
  if (argc == 9)
  {
    SEED = atoi(argv[1]);

    CODE_LENGTH = atoi(argv[2]);
    COLORS = atoi(argv[3]);
    // COLORS = CODE_LENGTH == 4 ? 6 : 8;

    MAX_GUESS = atoi(argv[4]);
    POPULATION_LENGTH = atoi(argv[5]);
    MAX_GEN = atoi(argv[6]);

    MUTATION_RATE = atof(argv[7]);

    FANCY_MODE = atof(argv[8]);

    printf("-- Manual Overwrite Detected --\n");
  }

  printf("SEEDS\t%d\n", SEED);
  printf("CODE_LENGTH\t%d\n", CODE_LENGTH);
  printf("COLORS\t%d\n", COLORS);
  printf("POPSIZE\t%d\n", POPULATION_LENGTH);
  printf("MAX_GEN\t%d\n", MAX_GEN);
  printf("MUTATION_RATE\t%.2f\n\n", MUTATION_RATE);

  // Initiate necessary variable for Mastermind
  int secret_code[CODE_LENGTH];
  int guesses[MAX_GUESS][CODE_LENGTH + 2];
  int black, white;

  // Initiate value in guess history
  for (int i = 0; i < MAX_GUESS; i++)
  {
    for (int j = 0; j < (CODE_LENGTH + 2); j++)
    {
      guesses[i][j] = 0;
    }
  }

  srand(SEED);

  // Generate secret code by random
  for (int i = 0; i < CODE_LENGTH; i++)
  {
    secret_code[i] = rand() % COLORS;
  }

  printf("Secret: ");
  PrintCode(secret_code);

  // Generate initial guess which is also random
  for (int i = 0; i < CODE_LENGTH; i++)
  {
    guesses[0][i] = rand() % COLORS;
  }

  // Calculate fitness score of the initial guess
  EvaluateGuess(guesses[0], secret_code, &guesses[0][CODE_LENGTH], &guesses[0][CODE_LENGTH + 1]);
  // printf("-INIT-\t| b:%d w:%d\t| ", guesses[0][CODE_LENGTH], guesses[0][CODE_LENGTH + 1]);
  printf("\n-INIT-\t| ");
  PrintGuessData(guesses[0]);

  // PrintCode(guesses[0]);

  // Start guessing loop where everything happens
  int round = 1;
  while (black != CODE_LENGTH && round < MAX_GUESS)
  {
    // List of eligible guess (possess potential to be the secret code)
    int eligibles[POPULATION_LENGTH * 2][CODE_LENGTH + 1];

    // Perform entire GA thing which is absolutely suffering to write tbh
    int e = GeneticEvolution(eligibles, guesses, round, POPULATION_LENGTH);

    if (FANCY_MODE == 1)
      printf("\n");

    printf("e:%d\t| ", e);

    // Check for duplicate from past guesses
    int k = 0;
    while (CheckDuplicate(eligibles[k], guesses, round))
    {
      k++;
    }

    // Make a guess
    for (int i = 0; i < CODE_LENGTH; i++)
    {
      guesses[round][i] = eligibles[k][i];
    }

    // Evaluate guess
    EvaluateGuess(guesses[round], secret_code, &guesses[round][CODE_LENGTH], &guesses[round][CODE_LENGTH + 1]);
    PrintGuessData(guesses[round]);

    // If the guess is correct then we won
    if (guesses[round][CODE_LENGTH] == CODE_LENGTH)
    {
      printf("\nThe secret code is ");
      PrintCode(guesses[round]);
      printf("We won in %d round!\n", round + 1);
      return 0;
    }

    round++;
  }

  printf("oh no. not win\n");

  return 0;
}
