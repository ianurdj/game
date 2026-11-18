
#include "raylib.h"
#include <stdlib.h>
#include <time.h>

// Game settings
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define GROUND_Y 540

#define BIRD_X 180
#define BIRD_RADIUS 17

#define PIPE_WIDTH 75
#define PIPE_GAP 175
#define PIPE_COUNT 3

#define GRAVITY 0.45f
#define JUMP_STRENGTH -7.5f

// Game states
typedef enum {
    START,
    PLAYING,
    GAME_OVER
} GameState;

// Bird structure
typedef struct {
    float y;
    float velocity;
} Bird;

// Pipe structure
typedef struct {
    float x;
    int gapY;
    int scored;
} Pipe;

// Global variables
Bird bird;
Pipe pipes[PIPE_COUNT];

int score = 0;
GameState gameState = START;

// Function declarations
void initBird(void);
void updateBird(void);
void jumpBird(void);
void drawBird(void);

void initPipes(void);
void updatePipes(void);
void drawPipes(void);

int checkCollision(void);

void updateScore(void);
void drawScore(void);

void resetGame(void);
void drawStartScreen(void);
void drawGameOverScreen(void);


// Initialize bird
void initBird(void)
{
    bird.y = 250;
    bird.velocity = 0;
}


// Update bird movement
void updateBird(void)
{
    bird.velocity += GRAVITY;
    bird.y += bird.velocity;

    // Prevent bird from going above screen
    if (bird.y < BIRD_RADIUS)
    {
        bird.y = BIRD_RADIUS;
        bird.velocity = 0;
    }
}


// Make bird jump
void jumpBird(void)
{
    bird.velocity = JUMP_STRENGTH;
}


// Draw bird
void drawBird(void)
{
    DrawCircle(BIRD_X, (int)bird.y,
               BIRD_RADIUS, YELLOW);

    DrawCircle(BIRD_X + 7, (int)bird.y - 5,
               4, WHITE);
}


// Initialize pipes
void initPipes(void)
{
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        pipes[i].x = SCREEN_WIDTH + i * 270;

        pipes[i].gapY = 180 + rand() % 220;

        pipes[i].scored = 0;
    }
}


// Update pipe movement
void updatePipes(void)
{
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        pipes[i].x -= 3;

        // Reposition pipe when it leaves screen
        if (pipes[i].x + PIPE_WIDTH < 0)
        {
            pipes[i].x = SCREEN_WIDTH;

            pipes[i].gapY = 180 + rand() % 220;

            pipes[i].scored = 0;
        }
    }
}


// Draw pipes
void drawPipes(void)
{
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        int topHeight = pipes[i].gapY - PIPE_GAP / 2;

        int bottomY = pipes[i].gapY + PIPE_GAP / 2;

        // Upper pipe
        DrawRectangle(
            (int)pipes[i].x,
            0,
            PIPE_WIDTH,
            topHeight,
            GREEN
        );

        // Lower pipe
        DrawRectangle(
            (int)pipes[i].x,
            bottomY,
            PIPE_WIDTH,
            GROUND_Y - bottomY,
            GREEN
        );
    }
}


// Check collision
int checkCollision(void)
{
    // Ground collision
    if (bird.y + BIRD_RADIUS >= GROUND_Y)
    {
        return 1;
    }

    for (int i = 0; i < PIPE_COUNT; i++)
    {
        int topHeight = pipes[i].gapY - PIPE_GAP / 2;

        int bottomY = pipes[i].gapY + PIPE_GAP / 2;

        Rectangle topPipe = {
            pipes[i].x, 0,
            PIPE_WIDTH, topHeight
        };

        Rectangle bottomPipe = {
            pipes[i].x, bottomY,
            PIPE_WIDTH, GROUND_Y - bottomY
        };

        // Check bird with upper and lower pipes
        if (CheckCollisionCircleRec(
                (Vector2){BIRD_X, bird.y},
                BIRD_RADIUS,
                topPipe))
        {
            return 1;
        }

        if (CheckCollisionCircleRec(
                (Vector2){BIRD_X, bird.y},
                BIRD_RADIUS,
                bottomPipe))
        {
            return 1;
        }
    }

    return 0;
}


// Update score
void updateScore(void)
{
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        if (!pipes[i].scored &&
            pipes[i].x + PIPE_WIDTH < BIRD_X)
        {
            score++;
            pipes[i].scored = 1;
        }
    }
}


// Draw score
void drawScore(void)
{
    DrawText(TextFormat("Score: %d", score),
             20, 20, 30, BLACK);
}


// Reset game
void resetGame(void)
{
    score = 0;

    initBird();
    initPipes();

    gameState = PLAYING;
}


// Draw start screen
void drawStartScreen(void)
{
    DrawText("FLAPPY BIRD",
             250, 180, 50, DARKBLUE);

    DrawText("Press SPACE to Start",
             260, 280, 25, BLACK);
}


// Draw Game Over screen
void drawGameOverScreen(void)
{
    DrawText("GAME OVER",
             260, 180, 50, RED);

    DrawText(TextFormat("Final Score: %d", score),
             310, 260, 25, BLACK);

    DrawText("Press ENTER to Restart",
             240, 330, 25, DARKBLUE);

    DrawText("Press ESC to Exit",
             280, 380, 20, DARKGRAY);
}


// Main function
int main(void)
{
    srand(time(NULL));

    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Flappy Bird"
    );

    SetTargetFPS(60);

    initBird();
    initPipes();

    while (!WindowShouldClose())
    {
        // START STATE
        if (gameState == START)
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                gameState = PLAYING;
            }
        }

        // PLAYING STATE
        else if (gameState == PLAYING)
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                jumpBird();
            }

            updateBird();
            updatePipes();

            updateScore();

            if (checkCollision())
            {
                gameState = GAME_OVER;
            }
        }

        // GAME OVER STATE
        else if (gameState == GAME_OVER)
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                resetGame();
            }
        }


        // DRAW EVERYTHING
        BeginDrawing();

        ClearBackground(SKYBLUE);

        if (gameState == START)
        {
            drawStartScreen();
            drawBird();
        }

        else if (gameState == PLAYING)
        {
            drawPipes();
            drawBird();
            drawScore();
        }

        else if (gameState == GAME_OVER)
        {
            drawPipes();
            drawBird();
            drawScore();
            drawGameOverScreen();
        }

        // Ground
        DrawRectangle(
            0, GROUND_Y,
            SCREEN_WIDTH,
            SCREEN_HEIGHT - GROUND_Y,
            BROWN
        );

        EndDrawing();
    }

    CloseWindow();

    return 0;
}


