class Point
{
    public:
    Point();
    Point(int x, int y);
    int x, y;
};

class Piece
{
    public:
    Piece();
    Piece(int type);
    Point blocks[4];
    int width, height;
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
    bool gameMap[16][17];
    Piece currentPiece;
    void placePiece();
};
