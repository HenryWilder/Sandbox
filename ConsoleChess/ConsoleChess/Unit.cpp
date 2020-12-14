#include "Unit.h"


void Unit::Setup(Location location, UnitColor color)
{
    m_location = location ;
    m_color = color;
}

Location Unit::GetLocation()
{
    return m_location;
}

Unit::UnitColor Unit::GetColor()
{
    return m_color;
}

void Unit::Move(Board* board, Location* location)
{
    // @TODO
}


//
// Unit specialization
//
//
//

unsigned char Unit::GetUnitAscii()
{
    return 0;
}

int Unit::GetPossibleMoves(Board* board, Location* locations, int locationCount)
{
    // Unit is the base class. It is only defined so that other units can
    // inheret from it. Therefore, we don't need to return any possible moves
    // for the base class "Unit".
    return 0;
}

// Pawns
unsigned char Pawn::GetUnitAscii()
{
    return 'P';
}

int Pawn::GetPossibleMoves(Board* board, Location* locations, int locationCount)
{

    return 0;
}

// Rooks
unsigned char Rook::GetUnitAscii()
{
    return 'R';
}

int Rook::GetPossibleMoves(Board* board, Location* locations, int locationCount)
{
    // @TODO
    return 0;
}

// Bishops
unsigned char Bishop::GetUnitAscii()
{
    return 'B';
}

int Bishop::GetPossibleMoves(Board* board, Location* locations, int locationCount)
{
    // @TODO
    return 0;
}

// Knights
unsigned char Knight::GetUnitAscii()
{
    return 'K';
}

int Knight::GetPossibleMoves(Board* board, Location* locations, int locationCount)
{
    // @TODO
    return 0;
}

// Queens
unsigned char Queen::GetUnitAscii()
{
    return 'Q';
}

int Queen::GetPossibleMoves(Board* board, Location* locations, int locationCount)
{
    // @TODO
    return 0;
}

// Kings
unsigned char King::GetUnitAscii()
{
    return '@';
}

int King::GetPossibleMoves(Board* board, Location* locations, int locationCount)
{
    // @TODO
    return 0;
}
