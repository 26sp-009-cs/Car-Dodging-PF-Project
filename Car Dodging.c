#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROAD_ROWS 12
#define ROAD_COLS 7
#define MAX_OBSTACLES 4
#define HIGH_SCORE_FILE "highscore.txt"

enum GameState { RUNNING, GAME_OVER, PLAYER_QUIT };

typedef struct {
    int row;
    int col;
    int active;
} Obstacle;

void displayWelcomeMessage();
void initializeObstacles(Obstacle *obsList, int size);
void spawnObstacle(Obstacle *obsList, int size, int turnCounter);
void moveObstacles(Obstacle *obsList, int size, int *currentScore);
void renderRoad(int playerCol, Obstacle *obsList, int size, int score, int topScore);
void updatePlayerPosition(int *playerCol, char movementInput, enum GameState *state);
int detectCollision(int playerCol, Obstacle *obsList, int size);
void triggerRecursiveCountdown(int countValue);
int loadHighScore();
void saveHighScore(int finalScore);

int main() {

    int playerColPosition = ROAD_COLS / 2;
    int currentScore = 0;
    int highTestCaseScore = 0;
    int gameTurnTicks = 0;
    char userActionInput;

    enum GameState continuousState = RUNNING;

    Obstacle *activeObstacles = (Obstacle*) malloc(MAX_OBSTACLES * sizeof(Obstacle));
    
    if (activeObstacles == NULL) {
        printf("Critical Error: Memory allocation failure!\n");
        return 1;
    }
    
    highTestCaseScore = loadHighScore();
    displayWelcomeMessage();
    initializeObstacles(activeObstacles, MAX_OBSTACLES);

    while (continuousState == RUNNING) {

        if (gameTurnTicks % 3 == 0) {
            spawnObstacle(activeObstacles, MAX_OBSTACLES, gameTurnTicks);
        }

        renderRoad(playerColPosition, activeObstacles, MAX_OBSTACLES, currentScore, highTestCaseScore);

        printf("\nYour Move -> [A] Left  [D] Right  [W] Stay  [Q] Quit: ");
        scanf(" %c", &userActionInput);

        updatePlayerPosition(&playerColPosition, userActionInput, &continuousState);
        
        if (continuousState == PLAYER_QUIT) {
            break;
        }

        moveObstacles(activeObstacles, MAX_OBSTACLES, &currentScore);

        if (detectCollision(playerColPosition, activeObstacles, MAX_OBSTACLES)) {
            continuousState = GAME_OVER;
        }
        
        gameTurnTicks++;
    }
    
    if (continuousState == GAME_OVER) {
        renderRoad(playerColPosition, activeObstacles, MAX_OBSTACLES, currentScore, highTestCaseScore);
        printf("\n?? BOOM! You crashed into an obstacle! ??\n");
        printf("Your Final Score: %d\n", currentScore);
        saveHighScore(currentScore);
        
        printf("\nReturning to main screen in:\n");
        triggerRecursiveCountdown(3);
    } else {
        printf("\nGame exited safely. Thanks for playing!\n");
    }

    free(activeObstacles);
    activeObstacles = NULL;
    
    return 0;
}

void displayWelcomeMessage() {
    system("cls");
    printf("===========================================\n");
    printf("     WELCOME TO THE CAR DODGING GAME       \n");
    printf("===========================================\n");
    printf(" Rules:\n");
    printf(" - Avoid incoming obstacles marked as [X]\n");
    printf(" - Your vehicle is positioned at the base: [P]\n");
    printf(" - Every move ticks time forward.\n\n");
    printf(" Controls:\n");
    printf(" - Type 'A' and press Enter to go Left\n");
    printf(" - Type 'D' and press Enter to go Right\n");
    printf(" - Type 'W' and press Enter to Stay in place\n\n");
    printf(" Press Enter to start your engines...");
    getchar(); 
}

void initializeObstacles(Obstacle *obsList, int size) {
    int i;
    for (i = 0; i < size; i++) {
        obsList[i].row = -1;
        obsList[i].col = -1;
        obsList[i].active = 0;
    }
}

void spawnObstacle(Obstacle *obsList, int size, int turnCounter) {
    int i;
    for (i = 0; i < size; i++) {
        if (!obsList[i].active) {
            obsList[i].row = 0;
            obsList[i].col = (turnCounter * 3 + 7) % ROAD_COLS; 
            obsList[i].active = 1;
            break; 
        }
    }
}

void moveObstacles(Obstacle *obsList, int size, int *currentScore) {
    int i;
    for (i = 0; i < size; i++) {
        if (obsList[i].active) {
            obsList[i].row++;
            if (obsList[i].row >= ROAD_ROWS) {
                obsList[i].active = 0; 
                (*currentScore)++;
            }
        }
    }
}

void renderRoad(int playerCol, Obstacle *obsList, int size, int score, int topScore) {
    int r, c, i;

    system("cls"); 
    
    printf("=== SCORE: %d | HIGH SCORE: %d ===\n\n", score, topScore);
    
    for (r = 0; r < ROAD_ROWS; r++) {
        printf("|");
        
        for (c = 0; c < ROAD_COLS; c++) {
            int elementPrinted = 0;

            if (r == ROAD_ROWS - 1 && c == playerCol) {
                printf("P");
                elementPrinted = 1;
            } else {
                for (i = 0; i < size; i++) {
                    if (obsList[i].active && obsList[i].row == r && obsList[i].col == c) {
                        printf("X");
                        elementPrinted = 1;
                        break;
                    }
                }
            }
            
            if (!elementPrinted) {
                printf(".");
            }
        }
        
        printf("|\n");
    }
}

void updatePlayerPosition(int *playerCol, char movementInput, enum GameState *state) {
    if (movementInput >= 'a' && movementInput <= 'z') {
        movementInput = movementInput - 32; 
    }
    
    switch (movementInput) {
        case 'A':
            if (*playerCol > 0) {
                (*playerCol)--;
            }
            break;
        case 'D':
            if (*playerCol < ROAD_COLS - 1) {
                (*playerCol)++;
            }
            break;
        case 'W':
            break;
        case 'Q':
            *state = PLAYER_QUIT;
            break;
        default:
            break;
    }
}

int detectCollision(int playerCol, Obstacle *obsList, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (obsList[i].active && obsList[i].row == ROAD_ROWS - 1 && obsList[i].col == playerCol) {
            return 1;
        }
    }
    return 0;
}

void triggerRecursiveCountdown(int countValue) {
    if (countValue <= 0) {
        printf("Done!\n\n");
        return;
    }
    
    printf("%d...\n", countValue);
    long int delayLoop;
    for (delayLoop = 0; delayLoop < 190000000; delayLoop++) {
    }
    
    triggerRecursiveCountdown(countValue - 1);
}

int loadHighScore() {
    int loadedScore = 0;
    FILE *filePointer = fopen(HIGH_SCORE_FILE, "r");
    
    if (filePointer != NULL) {
        fscanf(filePointer, "%d", &loadedScore);
        fclose(filePointer);
    }
    return loadedScore;
}

void saveHighScore(int finalScore) {
    int currentHighScore = loadHighScore();
    
    if (finalScore > currentHighScore) {
        FILE *filePointer = fopen(HIGH_SCORE_FILE, "w");
        if (filePointer != NULL) {
            fprintf(filePointer, "%d", finalScore);
            fclose(filePointer);
            printf("?? NEW HIGH SCORE RECORDED! ??\n");
        }
    }
}
