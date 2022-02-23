#include <Arduino.h>
#include <stdlib.h>
#include <time.h>
#include "core.h"

Piece::Piece()
{
    srand(time(NULL));
    Piece(rand() % 7);
}

Point::Point(int x, int y)
{
    this->x = x;
    this->y = y;
}

Point::Point()
{
    this->x = 0;
    this->y = 0;
}

Piece::Piece(int type)
{
    this->type = type;
    switch (type)
    {
    case 0: //'T':
        blocks[0] = Point(1, 0);
        blocks[1] = Point(0, 1);
        blocks[2] = Point(1, 1);
        blocks[3] = Point(2, 1);
        break;
    case 1: //'L':
        blocks[0] = Point(0, 1);
        blocks[1] = Point(1, 1);
        blocks[2] = Point(2, 0);
        blocks[3] = Point(2, 1);
        break;
    case 2: //'I':
        blocks[0] = Point(0, 1);
        blocks[1] = Point(1, 1);
        blocks[2] = Point(2, 1);
        blocks[3] = Point(3, 1);
        break;
    case 3: //'J':
        blocks[0] = Point(0, 0);
        blocks[1] = Point(0, 1);
        blocks[2] = Point(1, 1);
        blocks[3] = Point(2, 1);
        break;
    case 4: //'S':
        blocks[0] = Point(1, 0);
        blocks[1] = Point(2, 0);
        blocks[2] = Point(0, 1);
        blocks[3] = Point(1, 1);
        break;
    case 5: //'Z':
        blocks[0] = Point(0, 0);
        blocks[1] = Point(1, 0);
        blocks[2] = Point(1, 1);
        blocks[3] = Point(2, 1);
        break;
    case 6: //'O':
        blocks[0] = Point(0, 0);
        blocks[1] = Point(1, 0);
        blocks[2] = Point(0, 1);
        blocks[3] = Point(1, 1);
        break;
    default:
        break;
    }

    width = 0;
    height = 0;
    for (int i = 0; i < BLOCKS_PER_PIECE; i++)
    {
        if (blocks[i].x > width)
            width = blocks[i].x;
        if (blocks[i].y > height)
            height = blocks[i].y;
    }
}

Core::Core()
{
    reset();
}

void Core::reset()
{
    for (int i = 0; i < BOARD_WIDTH; i++)
        for (int j = 0; j < BOARD_HEIGHT; j++)
            gameMap[i][j] = (j == (BOARD_HEIGHT - 1)) ? true : false;
}

bool Core::hasBlock(int x, int y)
{
    return gameMap[x][y];
}

Piece Core::getCurrentPiece()
{
    return currentPiece;
}

/*
@brief: add a piece to the game map
*/
void Core::addPiece()
{
    // start position
    srand(time(NULL));
    currentPiece = Piece(rand() % 7);
    currentPiece.x = 6;
    currentPiece.y = 0;
}

void Core::movePiece(int direction)
{
    // check if the piece can move
    switch (direction)
    {
    case 0: // left
        currentPiece.x--;
        break;
    case 1: // right
        currentPiece.x++;
        break;
    case 2: // down
        currentPiece.y++;
        break;
    default:
        break;
    }

    if (checkPieceCollision())
    {
        // move back
        switch (direction)
        {
        case 0: // x--
            currentPiece.x++;
            break;
        case 1: // x++
            currentPiece.x--;
            break;
        case 2: // y++
            currentPiece.y--;
            placePiece();
            checkLineFull();
            addPiece();
            break;
        default:
            break;
        }
    }
}

void Core::placePiece()
{
    for (int i = 0; i < BLOCKS_PER_PIECE; i++)
        gameMap[currentPiece.blocks[i].x + currentPiece.x][currentPiece.blocks[i].y + currentPiece.y] = true;
}

bool Core::checkLineFull()
{
    bool full = true;
    for (int j = BOARD_HEIGHT - 2; j > -1; j--)
    {
        full = true;
        for (int i = 0; i < BOARD_WIDTH; i++)
            if (!gameMap[i][j])
            {
                full = false;
                break;
            }

        if (full)
        {
            for (int i = 0; i < BOARD_WIDTH; i++)
                gameMap[i][j] = false;
            for (int i = j; i > 0; i--)
                for (int k = 0; k < BOARD_WIDTH; k++)
                    gameMap[k][i] = gameMap[k][i - 1];
            j++;
        }
    }
    return true;
}

bool Core::checkPieceCollision()
{
    for (int i = 3; i > -1; i--)
        if (currentPiece.blocks[i].x + currentPiece.x < 0 || currentPiece.blocks[i].x + currentPiece.x > 15)
            return true;

    // start checking from the right/bottom (speedup)
    int bottomCheck = currentPiece.y + currentPiece.height + 1;
    if (bottomCheck > BOARD_HEIGHT)
        bottomCheck = BOARD_HEIGHT;

    int rightCheck = currentPiece.x + currentPiece.width + 1;
    if (rightCheck > BOARD_WIDTH)
        rightCheck = BOARD_WIDTH;

    for (int x = (currentPiece.x == 0 ? 0 : (currentPiece.x - 1)); x < rightCheck; x++)
        for (int y = currentPiece.y; y < bottomCheck; y++)
            if (gameMap[x][y])
                for (int i = (BLOCKS_PER_PIECE - 1); i > -1; i--)
                    if (currentPiece.blocks[i].x + currentPiece.x == x && currentPiece.blocks[i].y + currentPiece.y == y)
                        return true;

    return false;
}

void Core::rotatePiece(bool cw)
{
    // see https://medium.com/swlh/matrix-rotation-in-javascript-269cae14a124
    // convert piece to a matrix
    int size = (currentPiece.width > currentPiece.height ? currentPiece.width : currentPiece.height) + 1;

    int matrix[size][size];
    int destination[size][size];

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix[i][j] = destination[i][j] = 0;

    for (int i = 0; i < BLOCKS_PER_PIECE; i++)
        matrix[currentPiece.blocks[i].x][currentPiece.blocks[i].y] = 1;

    // recalculate measures
    currentPiece.width = currentPiece.height = 0;
    // rotate matrix
    int n = 0;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            destination[i][j] = cw ? matrix[size - j - 1][i] : matrix[j][size - i - 1];
            if (destination[i][j] == 1)
            {
                currentPiece.blocks[n++] = Point(i, j);
                if (i > currentPiece.width)
                    currentPiece.width = i;
                if (j > currentPiece.height)
                    currentPiece.height = j;
            }
        }

    // rollback rotation
    if (checkPieceCollision())
        rotatePiece(!cw);
}