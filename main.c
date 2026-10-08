#include <stdio.h>
#include <string.h>
#include "raylib.h"

#define CELL_WIDTH 10
#define CELL_HEIGHT 10

int board[9 * 9];

int convertColToX(int col)
{
    return (int) ((col - 4.5f) * CELL_WIDTH * GetScreenHeight() / 100 + GetScreenWidth() / 2);
}

int convertRowToY(int row)
{
    return (int) (GetScreenHeight() / 2 - (row - 4.5f) * CELL_HEIGHT * GetScreenHeight() / 100);
}

void drawBoard()
{
    for (int i = 0; i <= 9; i ++)
    {
        int x = convertColToX(i);
        DrawLine(x, convertRowToY(0), x, convertRowToY(9), i % 3 == 0 ? YELLOW : RAYWHITE);
    }

    for (int i = 0; i <= 9; i ++)
    {
        int y = convertRowToY(i);
        DrawLine(convertColToX(0), y, convertColToX(9), y, i % 3 == 0 ? YELLOW : RAYWHITE);
    }

    for (int i = 0; i < 9; i ++)
    {
        for (int j = 0; j < 9; j ++)
        {
            if (board[i * 9 + j] == -1) continue;

            char curr[1];
            sprintf(curr, "%d", board[i * 9 + j]);

            int x = convertColToX(j) / 2 + convertColToX(j + 1) / 2 - 20;
            int y = convertRowToY(i) / 2 + convertRowToY(i + 1) / 2 - 20;

            DrawText(curr, x, y, 40, GRAY);
        }
    }
}

void generateBoard()
{
    for (int i = 0; i < 9; i ++)
    {
        for (int j = 0; j < 9; j ++)
        {
            board[i * 9 + j] = GetRandomValue(0, 1) == 0 ? -1 : GetRandomValue(1, 9);
        }
    }
}

int main()
{
    generateBoard();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(1280, 720, "Sudoku");

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        drawBoard();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}