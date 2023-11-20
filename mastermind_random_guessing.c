#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CODE_LENGTH 4
#define COLORS 6

// Function to check if the code has been correctly guessed
int isCorrectGuess(int guess[], int code[]) {
    for (int i = 0; i < CODE_LENGTH; i++) {
        if (guess[i] != code[i]) {
            return 0; // Incorrect guess
        }
    }
    return 1; // Correct guess
}

void evaluateGuess(int guess[], int code[], int* black, int* white) {
    *black = 0;
    *white = 0;

    // Arrays to keep track of which elements have been matched in guess and code arrays
    int guessMatched[CODE_LENGTH]; // 0 1
    int codeMatched[CODE_LENGTH]; // 0 1 
    for(int i =0; i<CODE_LENGTH; i++){
        guessMatched[i] = 0;
        codeMatched[i] = 0;
    }

    // First, check for black pegs (correct digit in the correct position)
    for (int i = 0; i < CODE_LENGTH; i++) {
        if (guess[i] == code[i]) {
            (*black)++;
            guessMatched[i] = 1;
            codeMatched[i] = 1;
        }
    }

    // Then, check for white pegs (correct digit in the wrong position)
    for (int i = 0; i < CODE_LENGTH; i++) {
        for (int j = 0; j < CODE_LENGTH; j++) {
            if (!guessMatched[i] && !codeMatched[j] && guess[i] == code[j]) {
                (*white)++;
                guessMatched[i] = 1;
                codeMatched[j] = 1;
            }
        }
    }
}


// permutations
void generateNextGuess(int guess[]) {
    for (int i = 0; i < CODE_LENGTH; i++) {
        guess[i] = (guess[i] + 1) % COLORS;
        if (guess[i] != 0) {
            break;
        }
    }
}

int main() {
    int code_length = 4;
    int secret_code[code_length];
    int guess[code_length];
    int black, white;

    for(int i =0; i<CODE_LENGTH; i++){
        guess[i] = 0;
    }

    srand(time(NULL));
    for (int i = 0; i < CODE_LENGTH; i++) {
        secret_code[i] = rand() % COLORS; 
    }

    int round = 0;
    while (!isCorrectGuess(guess, secret_code)) {
        round++;
        evaluateGuess(guess, secret_code, &black, &white);
        printf("Round %d --> Guess: %d%d%d%d Black: %d White: %d\n", round, guess[0], guess[1], guess[2], guess[3], black, white);
        generateNextGuess(guess);
    }

    evaluateGuess(guess, secret_code, &black, &white);
    printf("Round %d --> Guess: %d%d%d%d Black: %d White: %d\n", round, guess[0], guess[1], guess[2], guess[3], black, white);
    
    printf("Code cracked in %d rounds!\n", round);
    printf("Secret Code: %d%d%d%d\n", secret_code[0], secret_code[1], secret_code[2], secret_code[3]);
    return 0;
}
