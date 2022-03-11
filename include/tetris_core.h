#define BOARD_WIDTH 16
#define BOARD_HEIGHT 17
#define BLOCKS_PER_PIECE 4
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
    Point blocks[BLOCKS_PER_PIECE];
    int width, height;
    int type;
    int x, y;
};

class TetrisCore
{
    public:
    TetrisCore();
    void addPiece();
    void reset();
    bool checkPieceCollision(); //true if collision
    bool checkLineFull(); //true if line full
    void movePiece(int direction);
    void rotatePiece(bool cw);
    bool hasBlock(int x, int y);
    Piece getCurrentPiece(); 
    int getScore();
    bool isGameOver();
private:
    bool gameMap[BOARD_WIDTH][BOARD_HEIGHT];
    Piece currentPiece;
    void placePiece();
    int score;
    bool gameOver;
};
