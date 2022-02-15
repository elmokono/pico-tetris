/*
T, L, I, B, S
*/
class Piece
{
    public:
    Piece();
    Piece(int type);
    bool map[4][4];
    int x, y;
};

class Core
{
    public:
    Core();
    void addPiece();
    bool checkPieceCollision(); //true if collision
    void movePiece(int direction);
    bool hasBlock(int x, int y);
    Piece getCurrentPiece(); 
private:
    bool gameMap[16][16];
    Piece currentPiece;
};
