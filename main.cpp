#include "raylib.h"
#include <math.h>

#define ROWS 5
#define COLS 5

void CheckAndRemoveFullRows(bool grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        bool fullRow = true;
        for (int j = 0; j < COLS; j++) {
            if (!grid[i][j]) {
                fullRow = false;
                break;
            }
        }
        if (fullRow) {
            // Remove row and move all rows up down
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < COLS; j++) {
                    grid[k][j] = grid[k - 1][j];
                }
            }
        }
    }
}

int main() {
    int screenWidth = 1000;
    int screenHeight = 900;

    // Calculate optimal cell size based on screen dimensions and desired grid size
    int cellSize = sqrt((screenWidth * screenHeight) / (ROWS * COLS * 2.77)); // Size of each square

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

    // Falling square variables
    int fallingSquareX = startX + (COLS / 2) * cellSize; // Start in the middle of the grid
    int fallingSquareY = startY; // Start at the top of the screen
    int usualSpeed = 3;
    int superSpeed = 9;
    int fallingSpeed = usualSpeed;

    // Main game loop
    while (!WindowShouldClose()) {
        // Update falling square position
        fallingSquareY += fallingSpeed;

        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            fallingSquareX -= cellSize;
        }

        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            fallingSquareX += cellSize;
        }

        if (IsKeyDown(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            fallingSpeed = superSpeed;
        } else {
            fallingSpeed = usualSpeed;
        }

        if (fallingSquareX < startX) {
            fallingSquareX = startX;
        }

        if (fallingSquareX > startX + (COLS - 1) * cellSize) {
            fallingSquareX = startX + (COLS - 1) * cellSize;
        }

        // Check if the square has landed
        int gridX = (fallingSquareX - startX) / cellSize;
        int gridY = (fallingSquareY - startY) / cellSize;

        bool landed = false;
        if (gridY >= ROWS - 1 || grid[gridY + 1][gridX]) {
            landed = true;
        }

        // If landed, mark the cell as occupied and reset the square to the top
        if (landed) {
            grid[gridY][gridX] = true;
            CheckAndRemoveFullRows(grid); // Проверить и удалить заполненные ряды
            fallingSquareX = startX + (COLS / 2) * cellSize;
            fallingSquareY = startY;
        }

        BeginDrawing();
        ClearBackground(BLUE); // Clear the screen with a blue color

        // Draw grid of cells
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                // Calculate coordinates for each cell in the grid
                int x = startX + j * cellSize;
                int y = startY + i * cellSize;
                // Draw each cell as a square with a light gray color
                if (grid[i][j]) {
                    DrawRectangle(x, y, cellSize - 1, cellSize - 1, GRAY); // -1 for gap between cells
                } else {
                    DrawRectangle(x, y, cellSize - 1, cellSize - 1, LIGHTGRAY); // -1 for gap between cells
                }
            }
        }

        // Draw the falling square
        DrawRectangle(fallingSquareX, fallingSquareY, cellSize, cellSize, RED);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
