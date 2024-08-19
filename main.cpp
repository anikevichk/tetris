#include "raylib.h"
#include <math.h>
#include <ctime>
#include <string>

#define ROWS 20
#define COLS 10

int counter = 0;
int score = 0;
int level = 1;
float usualInterval = 0.5f; // Interval for normal falling speed

typedef struct Tetromino {
    int shape[4][4]; // 4x4 grid for defining the tetromino shape
    int x, y; // Tetromino position
    Color color; // Tetromino color
} Tetromino;

Color newGreen = {140, 203, 94, 255};
Color newRed = {203, 65, 84, 255};
Color newYellow = {239, 211, 52, 255};
Color newBlue = {100, 149, 237, 225};
Color cellColor = {50, 50, 50, 255};

// Definition of tetromino shapes
Tetromino tetrominos[] = {
        {{{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0, newRed}, // O
        {{{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0, newGreen}, // -
        {{{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0, PINK},  // T
        {{{1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0, PURPLE}, // L
        {{{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0, ORANGE}, // J
        {{{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0, newYellow}, // Z
        {{{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0, newBlue}, // S
};

void CheckAndRemoveFullRows(Color grid[ROWS][COLS]) {
    int rowCounter = 0;
    // Check for full rows and remove them
    for (int i = 0; i < ROWS; i++) {
        bool fullRow = true;
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j].a == 0) { // Check if cell is empty (alpha == 0)
                fullRow = false;
                break;
            }
        }
        if (fullRow) {
            rowCounter++;
            counter++;
            level = counter / 5 + 1;
            // Shift all rows above down by one row
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < COLS; j++) {
                    grid[k][j] = grid[k - 1][j];
                }
            }
            // Clear the top row
            for (int j = 0; j < COLS; j++) {
                grid[0][j] = {0}; // Clear row, set alpha to 0 (transparent color)
            }
        }
    }
    if (rowCounter == 1) score += 100;
    else if (rowCounter == 2) score += 300;
    else if (rowCounter == 3) score += 500;
    else if (rowCounter == 4) score += 800;
}

bool CheckCollision(Tetromino t, Color grid[ROWS][COLS]) {
    // Check for collision of the tetromino with the grid boundaries and existing blocks
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (t.shape[i][j]) {
                int newX = t.x + j;
                int newY = t.y + i;
                // Check boundaries and collisions with existing blocks
                if (newX < 0 || newX >= COLS || newY >= ROWS || (newY >= 0 && grid[newY][newX].a != 0)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void PlaceTetromino(Tetromino t, Color grid[ROWS][COLS]) {
    // Place the tetromino on the grid
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (t.shape[i][j]) {
                grid[t.y + i][t.x + j] = t.color; // Save color in the grid
            }
        }
    }
}

void Rotate(Tetromino *t) {
    int rotatedShape[4][4] = {0};

    // Transfer the current shape to a temporary matrix, considering size
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            rotatedShape[j][3 - i] = t->shape[i][j];
        }
    }

    // Transfer the rotated shape back to the main matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            t->shape[i][j] = rotatedShape[i][j];
        }
    }
}

Tetromino GetRandomTetromino() {
    // Get a random tetromino
    int index = GetRandomValue(0, 6);
    Tetromino t = tetrominos[index];
    t.x = COLS / 2 - 2; // Initial position centered
    t.y = 0;            // Start at the top of the grid
    return t;
}

int main() {
    int screenWidth = 1000;
    int screenHeight = 900;

    // Determine optimal cell size
    int cellSize = sqrt((screenWidth * screenHeight) / (ROWS * COLS * 2.77));

    // Calculate grid size and initial position
    int gridWidth = COLS * cellSize;
    int gridHeight = ROWS * cellSize;
    int startX = (screenWidth - gridWidth) / 2;
    int startY = (screenHeight - gridHeight) / 2;

    // Adjust grid size if it exceeds screen dimensions
    while (gridWidth > screenWidth || gridHeight > screenHeight) {
        cellSize--;
        gridWidth = COLS * cellSize;
        gridHeight = ROWS * cellSize;
        startX = (screenWidth - gridWidth) / 2;
        startY = (screenHeight - gridHeight) / 2;
    }

    InitWindow(screenWidth, screenHeight, "Tetris");
    SetTargetFPS(60);

    // Grid for tracking occupied cells
    Color grid[ROWS][COLS] = { 0 };

    srand(time(NULL)); // Initialize random number generator

    // Initialize the first tetromino
    Tetromino currentTetromino = GetRandomTetromino();

    // Falling speed variables and intervals
    float timeSinceLastFall = 0.0f;
    float superInterval = 0.05f; // Interval for super fast falling
    float fallSpeedIncreaseRate = 0.05f; // Speed increase rate per level
    float fallInterval = usualInterval; // Current falling interval

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        timeSinceLastFall += deltaTime;

        // Adjust usualInterval based on level
        if (usualInterval > superInterval) usualInterval = 0.5f - (level - 1) * fallSpeedIncreaseRate;

        // Input handling and tetromino position update
        Tetromino nextPosition = currentTetromino;

        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            nextPosition.x--;
            if (CheckCollision(nextPosition, grid)) {
                nextPosition.x++;
            }
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            nextPosition.x++;
            if (CheckCollision(nextPosition, grid)) {
                nextPosition.x--;
            }
        }
        if (IsKeyDown(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            fallInterval = superInterval; // Increase falling speed
        } else {
            fallInterval = usualInterval; // Return to normal falling speed
        }

        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            Rotate(&nextPosition);
            if (CheckCollision(nextPosition, grid)) {
                nextPosition = currentTetromino; // Revert to original position if collision occurs
            }
        }

        // Update falling position based on elapsed time and interval
        if (timeSinceLastFall >= fallInterval) {
            nextPosition.y++;
            if (IsKeyDown(KEY_S) || IsKeyPressed(KEY_DOWN)) score += 2;
            timeSinceLastFall = 0.0f; // Reset time since last fall
        }

        // Check collision and update tetromino position or place it on the grid
        bool landed = CheckCollision(nextPosition, grid);
        if (landed) {
            PlaceTetromino(currentTetromino, grid);
            CheckAndRemoveFullRows(grid);
            currentTetromino = GetRandomTetromino();
            if (CheckCollision(currentTetromino, grid)) {
                break; // Game over if new tetromino immediately collides
            }
        } else {
            currentTetromino = nextPosition;
        }

        // Drawing the game
        BeginDrawing();
        ClearBackground(BLACK); // Clear screen

        // Draw grid cells
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                int x = startX + j * cellSize;
                int y = startY + i * cellSize;
                if (grid[i][j].a != 0) { // If cell is occupied
                    DrawRectangle(x, y, cellSize - 1, cellSize - 1, grid[i][j]); // Draw occupied cell
                } else {
                    DrawRectangle(x, y, cellSize - 1, cellSize - 1, cellColor); // Draw empty cell
                }
            }
        }

        // Draw falling tetromino
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (currentTetromino.shape[i][j]) {
                    int x = startX + (currentTetromino.x + j) * cellSize;
                    int y = startY + (currentTetromino.y + i) * cellSize;
                    DrawRectangle(x, y, cellSize - 1, cellSize - 1, currentTetromino.color); // Draw tetromino cell
                }
            }
        }

        // Draw text
        std::string counterText = "Lines: " + std::to_string(counter);
        std::string scoreText = "Score: " + std::to_string(score);
        std::string levelText = "Level: " + std::to_string(level);

        DrawText(counterText.c_str(), 10, 10, 20, RAYWHITE);
        DrawText(scoreText.c_str(), 10, 30, 20, RAYWHITE);
        DrawText(levelText.c_str(), 10, 50, 20, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
