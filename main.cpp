#include "raylib.h"
#include <math.h>
#include <ctime>

#define ROWS 20
#define COLS 10

typedef struct Tetromino {
    int shape[4][4]; // 4x4 grid to define the shape of the tetromino
    int x, y; // Position of the tetromino
};

// Define tetromino shapes for O and T
Tetromino tetrominos[] = {
        {{{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0}, // O
        {{{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0},  // T
        {{{1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0}, // г
        {{{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0}, // г
        {{{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0}, // z
        {{{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0}, // z
        {{{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 0, 0} // -
};

void CheckAndRemoveFullRows(bool grid[ROWS][COLS]) {
    // Check each row for fullness and remove if full
    for (int i = 0; i < ROWS; i++) {
        bool fullRow = true;
        for (int j = 0; j < COLS; j++) {
            if (!grid[i][j]) {
                fullRow = false;
                break;
            }
        }
        if (fullRow) {
            // Move all rows above down by one row
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < COLS; j++) {
                    grid[k][j] = grid[k - 1][j];
                }
            }
            // Clear the topmost row
            for (int j = 0; j < COLS; j++) {
                grid[0][j] = false;
            }
        }
    }
}

bool CheckCollision(Tetromino t, bool grid[ROWS][COLS]) {
    // Check collision of tetromino with grid boundaries and existing blocks
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (t.shape[i][j]) {
                int newX = t.x + j;
                int newY = t.y + i;
                // Check boundaries and collisions with existing blocks
                if (newX < 0 || newX >= COLS || newY >= ROWS || (newY >= 0 && grid[newY][newX])) {
                    return true;
                }
            }
        }
    }
    return false;
}

void PlaceTetromino(Tetromino t, bool grid[ROWS][COLS]) {
    // Place tetromino onto the grid
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (t.shape[i][j]) {
                grid[t.y + i][t.x + j] = true;
            }
        }
    }
}

void Rotate(Tetromino *t) {
    int rotatedShape[4][4] = {0};

    // transfer the current shape to a temporary matrix, taking into account the size
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            rotatedShape[j][3 - i] = t->shape[i][j];
        }
    }

    // transfer the rotated shape back to the main matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            t->shape[i][j] = rotatedShape[i][j];
        }
    }
}

Tetromino GetRandomTetromino() {
    // Get a random tetromino (O or T)
    int index = GetRandomValue(0, 6);
    Tetromino t = tetrominos[index];
    t.x = COLS / 2 - 2; // Initial position to start at the center
    t.y = 0;            // Start at the top of the grid
    return t;
}

int main() {
    int screenWidth = 1000;
    int screenHeight = 900;

    // Calculate optimal cell size based on screen dimensions and desired grid size
    int cellSize = sqrt((screenWidth * screenHeight) / (ROWS * COLS * 2.77));

    // Calculate grid dimensions based on cell size and number of rows/columns
    int gridWidth = COLS * cellSize;
    int gridHeight = ROWS * cellSize;

    // Calculate starting position to center the grid horizontally and vertically
    int startX = (screenWidth - gridWidth) / 2;
    int startY = (screenHeight - gridHeight) / 2;

    // Adjust grid size if it exceeds the screen dimensions
    if (gridWidth > screenWidth || gridHeight > screenHeight) {
        while (gridWidth > screenWidth || gridHeight > screenHeight) {
            cellSize--;
            gridWidth = COLS * cellSize;
            gridHeight = ROWS * cellSize;
            startX = (screenWidth - gridWidth) / 2;
            startY = (screenHeight - gridHeight) / 2;
        }
    }

    InitWindow(screenWidth, screenHeight, "Tetris");
    SetTargetFPS(60);

    // Grid to track occupied cells
    bool grid[ROWS][COLS] = { false };

    srand(time(NULL)); // Seed random number generator

    // Initialize the first tetromino
    Tetromino currentTetromino = GetRandomTetromino();

    // Falling speed variables and intervals
    float timeSinceLastFall = 0.0f;
    float superInterval = 0.05f; // Interval for super speed falling
    float usualInterval = 0.15f; // Interval for usual speed falling
    float fallInterval = usualInterval; // Current falling interval

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        timeSinceLastFall += deltaTime;

        // Handle input and update tetromino position
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
            fallInterval = usualInterval; // Reset to normal falling speed
        }

        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            Rotate(&nextPosition);
            if (CheckCollision(nextPosition, grid)) {
                // Восстановление оригинальной позиции, если поворот вызывает коллизию
                nextPosition = currentTetromino; // Вернуть оригинальную позицию
            }
        }

        // Update falling position based on elapsed time and interval
        if (timeSinceLastFall >= fallInterval) {
            nextPosition.y++;
            timeSinceLastFall = 0.0f; // Reset time since last fall
        }

        // Check collision and update tetromino position or place if landed
        bool landed = CheckCollision(nextPosition, grid);
        if (landed) {
            PlaceTetromino(currentTetromino, grid);
            CheckAndRemoveFullRows(grid);
            currentTetromino = GetRandomTetromino();
            if (CheckCollision(currentTetromino, grid)) {
                break; // Game over if new tetromino collides immediately
            }
        } else {
            currentTetromino = nextPosition;
        }

        // Render the game
        BeginDrawing();
        ClearBackground(BLUE); // Clear the screen with blue color

        // Draw grid cells
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                int x = startX + j * cellSize;
                int y = startY + i * cellSize;
                if (grid[i][j]) {
                    DrawRectangle(x, y, cellSize - 1, cellSize - 1, GRAY); // Draw occupied cell
                } else {
                    DrawRectangle(x, y, cellSize - 1, cellSize - 1, LIGHTGRAY); // Draw empty cell
                }
            }
        }

        // Draw the falling tetromino
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (currentTetromino.shape[i][j]) {
                    int x = startX + (currentTetromino.x + j) * cellSize;
                    int y = startY + (currentTetromino.y + i) * cellSize;
                    DrawRectangle(x, y, cellSize - 1, cellSize - 1, RED); // Draw tetromino cell
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
