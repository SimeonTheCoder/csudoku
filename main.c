#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "raylib.h"

#define CELL_WIDTH 10
#define CELL_HEIGHT 10

#define index(y,x) (y)*9 + (x)

Font font;

int board[9 * 9];
bool highlighted [9 * 9];

int history[100];
int historyIndex = 0;

int convertColToX(int col)
{
    return (int) ((col - 4.5f) * CELL_WIDTH * GetScreenHeight() / 100 + GetScreenWidth() / 2);
}

int convertRowToY(int row)
{
    return (int) (GetScreenHeight() / 2 - (row - 4.5f) * CELL_HEIGHT * GetScreenHeight() / 100);
}

int convertXToCol(float x)
{
    x -= GetScreenWidth() / 2;
    x = x / (CELL_WIDTH * GetScreenHeight() / 100);

    int col = floor(x + 4.5f);

    return col >= 0 && col <= 9 ? col : -1;
}

int convertYToRow(float y)
{
    y = GetScreenHeight() / 2 - y;
    y = y / (CELL_HEIGHT * GetScreenHeight() / 100);

    int row = floor(y + 4.5f);

    return row >= 0 && row <= 9 ? row : -1;
}

void drawBoard()
{
    Vector2 mousePos = GetMousePosition();

    int mr = convertYToRow(mousePos.y);
    int mc = convertXToCol(mousePos.x);

    for (int i = 0; i < 9; i ++)
    {
        for (int j = 0; j < 9; j ++)
        {
            int x = convertColToX(j) / 2 + convertColToX(j + 1) / 2 - 20;
            int y = convertRowToY(i) / 2 + convertRowToY(i + 1) / 2 - 20;
            
            bool highlightedMouse = i == mr && j == mc;

            if (highlightedMouse || highlighted[index(i, j)])
            {
                DrawRectangle(
                    convertColToX(j),
                    convertRowToY(i + 1),
                    CELL_WIDTH * GetScreenHeight() / 100,
                    CELL_HEIGHT * GetScreenHeight() / 100,
                    highlightedMouse ? ORANGE : BROWN
                );
            }

            if (board[index(i, j)] == -1) continue;

            char curr[1];
            sprintf(curr, "%d", board[index(i, j)]);

            DrawTextEx(font, curr, (Vector2){x, y}, 40, 10, !highlightedMouse && !highlighted[index(i, j)] ? GRAY : WHITE);
        }
    }

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
}

void generateBoard()
{
    for (int i = 0; i < 9; i ++)
    {
        for (int j = 0; j < 9; j ++)
        {
            board[index(i, j)] = GetRandomValue(0, 1) == 0 ? -1 : GetRandomValue(1, 9);
        }
    }
}

void highlight(int row, int col)
{
    for (int i = 0; i < 9 * 9; i ++) highlighted[i] = false;

    int number = board[row * 9 + col];
    if (number == -1) return;

    for (int i = 0; i < 9 * 9; i ++)
    {
        if (board[i] == number) highlighted[i] = true;
    }
}

int count(int number)
{
    int counter = 0;

    for (int i = 0; i < 9 * 9; i ++)
        if (board[i] == number) counter ++;

    return counter;
}

bool isBoardValid()
{
    int histogram[9];

    //Rows
    for (int i = 0; i < 9; i ++)
    {
        for (int j = 0; j < 9; j ++) histogram[j] = 0;
        for (int j = 0; j < 9; j ++) histogram[board[index(i, j)]] ++;
        for (int j = 0; j < 9; j ++) if (histogram[j] > 1) return false;
    }

    //Cols
    for (int j = 0; j < 9; j ++)
    {
        for (int i = 0; i < 9; i ++) histogram[i] = 0;
        for (int i = 0; i < 9; i ++) histogram[board[index(i, j)]] ++;
        for (int i = 0; i < 9; i ++) if (histogram[i] > 1) return false;
    }

    //Squares
    for (int i = 0; i < 3; i ++)
    {
        for (int j = 0; j < 3; j ++)
        {
            for (int k = 0; k < 9; k ++) histogram[k] = 0;

            for (int k = 0; k < 3; k ++)
                for (int l = 0; l < 3; l ++)
                    histogram[board[index(i * 3 + k, j * 3 + l)]] ++;

            for (int k = 0; k < 9; k ++) if (histogram[k] > 1) return false;
        }
    }

    return true;
}

int main()
{
    for (int i = 0; i < 9 * 9; i ++) board[i] = -1;
    // generateBoard();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(1280, 720, "Sudoku");
    SetTargetFPS(120);

    font = LoadFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf");

    while(!WindowShouldClose())
    {
        Vector2 mousePos = GetMousePosition();

        int mr = convertYToRow(mousePos.y);
        int mc = convertXToCol(mousePos.x);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            highlight(mr, mc);

        for (int i = 1; i < 10; i ++)
        {
            if (!IsKeyPressed(48 + i)) continue;
            if (board[index(mr, mc)] != -1) continue;

            board[index(mr, mc)] = i;
            history[historyIndex] = index(mr, mc);
            historyIndex ++;

            break;
        }

        if (IsKeyPressed(KEY_U) && historyIndex > 0)
            board[history[--historyIndex]] = -1;

        BeginDrawing();
        ClearBackground(isBoardValid() ? BLACK : RED);

        drawBoard();

        for (int i = 1; i <= 9; i ++)
        {
            char curr[4];
            sprintf(curr, "%d: %d", i, 9 - count(i));

            DrawTextEx(font, curr, (Vector2) {50, 50 + i * 60}, 40, 10, YELLOW);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}