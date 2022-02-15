#include <stdlib.h>
#include "core.h"

Piece::Piece(int type)
{
    switch (type)
    {
    case 0: //'T':
        map[0][0] = true;
        map[1][0] = true;
        map[2][0] = true;
        map[1][1] = true;
        break;
    case 1: //'L':
        map[0][0] = true;
        map[0][1] = true;
        map[0][2] = true;
        map[1][2] = true;
        break;
    case 2: //'I':
        map[0][0] = true;
        map[1][0] = true;
        map[2][0] = true;
        map[3][0] = true;
        break;
    case 3: //'B':
        map[0][0] = true;
        map[1][0] = true;
        map[0][1] = true;
        map[1][1] = true;
        break;
    case 4: //'S':
        map[0][0] = true;
        map[0][1] = true;
        map[1][1] = true;
        map[2][2] = true;
        break;
    default:
        break;
    }
}

bool Core::hasBlock(int x, int y)
{
    return gameMap[x][y];
}

/*
@brief: add a piece to the game map
*/
void Core::addPiece()
{
    // start position
    Piece p = Piece(rand() % 5);
    p.x = 6;
    p.y = 0;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (p.map[i][j])
                gameMap[p.x + i][p.y + j] = true;

    currentPiece = p;
}

void Core::movePiece(int direction)
{
    int pieceWidth = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (currentPiece.map[i][j])
                pieceWidth++;

    // check if the piece can move
    switch (direction)
    {
    case 0: // x--
        if (currentPiece.x > 0)
            currentPiece.x--;
        break;
    case 1: // x++
        if (currentPiece.x + pieceWidth < 16)
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

bool Core::checkPieceCollision()
{
    for (int x = 0; x < 15; x++)
        for (int y = 0; y < 15; y++)
        {
            for (int i = 3; i > 0; i--)
                for (int j = 3; j > 0; j--)
                {
                    if (currentPiece.map[i][j])
                    {
                        if (gameMap[currentPiece.x + i][currentPiece.y + j + 1]) // overlaps with the next row
                        {
                            for (int pi = 0; pi < 3; pi++)
                                for (int pj = 0; pj < 3; pj++)
                                {
                                    if (currentPiece.map[pi][pj])
                                        gameMap[currentPiece.x + pi][currentPiece.y + pj] = true;
                                }
                            return true;
                        }
                    }
                }
        }

    return false;
}