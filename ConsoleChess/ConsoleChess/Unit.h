#pragma 

#include "Board.h"

class Location
{
public:
    int x;
    int y;
};

class Unit
{
public:

    enum UnitColor
    {
        UnitColor_Black = 0,
        UnitColor_White = 1,
    };

    void Setup(Location location, UnitColor color);

    Location  GetLocation();
    UnitColor GetColor();

    void Move(Board* board, Location* location);

    virtual unsigned char GetUnitAscii();
    virtual int GetPossibleMoves(Board* board, Location* locations, int locationCount);

private:
    Location m_location = {0,0};
    UnitColor m_color = UnitColor_Black;
};



class Pawn : public Unit
{
    unsigned char GetUnitAscii() override;
    int GetPossibleMoves(Board* board, Location* locations, int locationCount) override;
};

class Rook : public Unit
{
    unsigned char GetUnitAscii() override;
    int GetPossibleMoves(Board* board, Location* locations, int locationCount) override;
};

class Bishop : public Unit
{
    unsigned char GetUnitAscii() override;
    int GetPossibleMoves(Board* board, Location* locations, int locationCount) override;
};

class Knight : public Unit
{
    unsigned char GetUnitAscii() override;
    int GetPossibleMoves(Board* board, Location* locations, int locationCount) override;
};

class Queen : public Unit
{
    unsigned char GetUnitAscii() override;
    int GetPossibleMoves(Board* board, Location* locations, int locationCount) override;
};

class King : public Unit
{
    unsigned char GetUnitAscii() override;
    int GetPossibleMoves(Board* board, Location* locations, int locationCount) override;
};
