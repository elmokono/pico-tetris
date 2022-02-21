#include <stdlib.h>
#include "core.h"

Piece::Piece()
{
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
    switch (type)
    {
    case 0: //'T':
        blocks[0] = Point(1, 0);
        blocks[1] = Point(0, 1);
        blocks[2] = Point(1, 1);
        blocks[3] = Point(2, 1);
        break;
    case 1: //'L':
        blocks[0] = Point(2, 0);
        blocks[1] = Point(1, 0);
        blocks[2] = Point(1, 1);
        blocks[3] = Point(1, 2);
        break;
    case 2: //'I':
        blocks[0] = Point(0, 0);
        blocks[1] = Point(1, 0);
        blocks[2] = Point(2, 0);
        blocks[3] = Point(3, 0);
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
    for (int i = 0; i < 4; i++)
    {
        if (blocks[i].x > width)
            width = blocks[i].x;
        if (blocks[i].y > height)
            height = blocks[i].y;
    }
}

Core::Core()
{
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 17; j++)
            gameMap[i][j] = (j == 16) ? true : false;
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
    Piece p = Piece(rand() % 7);
    p.x = 6;
    p.y = 0;
    currentPiece = p;
}

void Core::movePiece(int direction)
{
    // check if the piece can move
    switch (direction)
    {
    case 0: // x--
        if (currentPiece.x > 0)
            currentPiece.x--;
        break;
    case 1: // x++
        if (currentPiece.x + currentPiece.width < 15)
            currentPiece.x++;
        break;
    case 2: // y++
        if (!checkPieceCollision())
            currentPiece.y++;
        break;
    default:
        break;
    }
}

void Core::placePiece()
{
    for (int i = 0; i < 4; i++)
        gameMap[currentPiece.blocks[i].x + currentPiece.x][currentPiece.blocks[i].y + currentPiece.y] = true;
}

bool Core::checkPieceCollision()
{
    /*if (currentPiece.y + currentPiece.height == 16)
    {
        placePiece();
        return true;
    }
*/
    for (int x = 0; x < 16; x++)
        for (int y = 0; y < 17; y++)
            if (gameMap[x][y])
                for (int i = 0; i < 4; i++)
                    if (currentPiece.blocks[i].x + currentPiece.x == x && currentPiece.blocks[i].y + currentPiece.y + 1 == y)
                    {
                        placePiece();
                        return true;
                    }

    return false;
}