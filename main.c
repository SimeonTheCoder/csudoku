#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "raylib.h"

#define CELL_WIDTH 9
#define CELL_HEIGHT 9

#define index(y,x) (y)*9 + (x)

Font font;

Color themeCol = (Color) {255, 128, 0, 255};
bool lightTheme = false;

Color bgCol;
Color textCol;
Color textDarkCol;
Color highlightedCol;
Color highlightedDarkCol;

Color invertColor(Color col)
{
    return (Color) {255 - col.r, 255 - col.g, 255 - col.b, 255};
}

void createTheme(Color col)
{
    bgCol = (Color) {col.r / 4, col.g / 4, col.b / 4, col.a};
    textCol = (Color) {128 + col.r / 2, 128 + col.g / 2, 128 + col.b / 2, col.a};
    textDarkCol = (Color) {64 + col.r / 4, 64 + col.g / 4, 64 + col.b / 4, col.a};

    highlightedCol = (Color) {col.r, col.g, col.b, col.a};
    highlightedDarkCol = (Color) {col.r / 2, col.g / 2, col.b / 2, col.a};

    if (lightTheme)
    {
        bgCol = invertColor(bgCol);
        textCol = invertColor(textCol);
        textDarkCol = invertColor(textDarkCol);

        highlightedCol = invertColor(highlightedCol);
        highlightedDarkCol = invertColor(highlightedDarkCol);
    }
}

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
                    highlightedMouse ? highlightedCol : highlightedDarkCol
                );
            }

            if (board[index(i, j)] == -1) continue;

            char curr[1];
            sprintf(curr, "%d", board[index(i, j)]);

            DrawTextEx(font, curr, (Vector2){x, y}, 40, 10, !highlightedMouse && !highlighted[index(i, j)] ? textDarkCol : textCol);
        }
    }

    for (int i = 0; i <= 9; i ++)
    {
        int x = convertColToX(i);
        DrawLine(x, convertRowToY(0), x, convertRowToY(9), i % 3 == 0 ? highlightedCol : textDarkCol);
    }

    for (int i = 0; i <= 9; i ++)
    {
        int y = convertRowToY(i);
        DrawLine(convertColToX(0), y, convertColToX(9), y, i % 3 == 0 ? highlightedCol : textDarkCol);
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

bool eightQueens(int depth, int number)
{
    if (depth == 9) return true;

    for (int i = 0; i < 9; i ++)
    {
        if (board[index(depth, i)] != -1) continue;

        board[index(depth, i)] = number;

        if (!isBoardValid())
        {
            board[index(depth, i)] = -1;
            continue;
        }

        if (eightQueens(depth + 1, number)) return true;
        else board[index(depth, i)] = -1;
    }

    return false;
}

int main()
{
    createTheme(themeCol);

    for (int i = 0; i < 9 * 9; i ++) board[i] = -1;
    
    for (int i = 1; i <= 9; i ++) eightQueens(0, i);

    for (int i = 0; i < 9 * 9; i ++)
        if (GetRandomValue(0, 100) >= 70) board[i] = -1;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(1280, 720, "Sudoku");
    SetTargetFPS(120);

    font = LoadFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf");

    int cellX = -1, cellY = -1;

    float time = 0;

    while(!WindowShouldClose())
    {
        time += GetFrameTime();

        Vector2 mousePos = GetMousePosition();

        int mr = convertYToRow(mousePos.y);
        int mc = convertXToCol(mousePos.x);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            highlight(mr, mc);
            highlighted[index(mr, mc)] = true;
            cellY = mr;
            cellX = mc;
        }

        for (int i = 1; i < 10; i ++)
        {
            if (!IsKeyPressed(48 + i)) continue;
            if (cellX == -1 || cellY == -1) continue;
            if (board[index(cellY, cellX)] != -1) continue;

            board[index(cellY, cellX)] = i;
            history[historyIndex] = index(cellY, cellX);
            historyIndex ++;

            break;
        }

        if (IsKeyPressed(KEY_U) && historyIndex > 0)
            board[history[--historyIndex]] = -1;

        if (IsKeyPressed(KEY_R))
        {
            createTheme((Color) {GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255});
        }

        if (IsKeyPressed(KEY_L)) lightTheme = !lightTheme;

        BeginDrawing();
        ClearBackground(!isBoardValid() ? RED : count(-1) == 0 ? DARKGREEN : bgCol);

        int minutes = (int) time / 60;
        int seconds = ((int) time) % 60;

        int secondsFirstDigit = seconds / 10;
        int secondsSecondDigit = seconds % 10;

        char clock[5];
        sprintf(clock, "%d:%d%d", minutes, secondsFirstDigit, secondsSecondDigit);
        DrawTextEx(font, clock, (Vector2) { GetScreenWidth() / 2 - 50, 50 }, 40, 10, highlightedCol);

        drawBoard();

        for (int i = 1; i <= 9; i ++)
        {
            int currCount = count(i);
            if (9 - currCount == 0) continue;

            char curr[4];
            sprintf(curr, "%d: %d", i, 9 - currCount);

            DrawTextEx(font, curr, (Vector2) {50, 50 + i * 60}, 40, 10, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}