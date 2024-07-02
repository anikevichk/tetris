#include "raylib.h"
#include <math.h>

#define ROWS 20
#define COLS 10

int main() {
    int screenWidth = 1000;
    int screenHeight = 800;

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

    // Main game loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLUE); // Clear the screen with a blue color

        // Draw grid of cells
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                // Calculate coordinates for each cell in the grid
                int x = startX + j * cellSize;
                int y = startY + i * cellSize;
                // Draw each cell as a square with a light gray color
                DrawRectangle(x, y, cellSize - 1, cellSize - 1, LIGHTGRAY); // -1 for gap between cells
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
