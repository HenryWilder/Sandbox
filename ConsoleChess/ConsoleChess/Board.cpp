#include "Board.h"
#include "Unit.h"
#include <iostream>

/*
"\x1b[<n>A" // cursor up <n>
"\x1b[<n>B" // cursor down <n>
"\x1b[<n>C" // cursor right <n>
"\x1b[<n>D" // cursor left <n>

"\x1b[<y>;<x>H" // cursor position (<y>,<x>)
*/

#define CURSOR_MOVE(y,x) "\x1b[" << y << ";" << x << "f"

#define CURSOR_SAVE "\x1b[s" // Save cursor
#define CURSOR_LOAD "\x1b[u" // Load cursor

// Reset colors
#define FG_RESET "\x1b[39;0m"
#define BG_RESET "\x1b[49;0m"
// Custom colors
#define FG_BLACK "\x1b[38;2;20;20;20;1m"
#define BG_BLACK "\x1b[48;2;236;167;95;1m"
#define FG_WHITE "\x1b[38;2;256;256;256;1m"
#define BG_WHITE "\x1b[48;2;252;219;166;1m"
// Selection colors
#define FG_SELECT "\x1b[38;2;44;200;37;1m"
#define BG_SELECT "\x1b[48;2;155;235;135;1m"
// Edge colors
#define FG_SUB "\x1b[38;2;100;100;100;1m"

#define max(a, b) (a > b ? a : b)

bool isEven(int a) { return (a == (2 * (a / 2))); }

char NumToLetter(int value)
{
    // this means if value is 0, it will be 'a'. if 1 it will be 'b' and so on...
    return value + 'a';
}

int LetterToNum(char letter)
{
    return int(letter - 'a');
}

Unit* Board::GetUnit(Location location)
{
    // look at black units
    for (int i = 0; i < blackUnits.size(); ++i)
    {
        Unit* u = blackUnits[i];
        Location loc = u->GetLocation();

        if (loc.x == location.x &&
            loc.y == location.y)
        {
            return u;
        }
    }

    // look at white units
    for (int i = 0; i < whiteUnits.size(); ++i)
    {
        Unit* u = whiteUnits[i];
        Location loc = u->GetLocation();

        if (loc.x == location.x &&
            loc.y == location.y)
        {
            return u;
        }
    }


    // @TODO
    return nullptr;
}

void Board::ResetBoard(int width, int height)
{
    this->width = width;
    this->height = height;

    // Black's pieces are all added simultaniously with White's. This reduces the number of iterations. --Amy

    // Pawns:
    for (int i = 0; i < width; ++i)
    {
        // White
        {
            // Allocate memory for the pawn and default construct the object
            Unit* newPawn = new Pawn();
            Location loc;
            loc.x = i;
            loc.y = height - 2; // second row from the bottom

            // We use the "->" arrow operator to access memember of an object through a pointer
            newPawn->Setup(loc, Unit::UnitColor_White);

            AddUnit(newPawn);
        }

        // Black
        {
            // Allocate memory for the pawn and default construct the object
            Unit* newPawn = new Pawn();
            Location loc;
            loc.x = i;
            loc.y = 1; // second row from the top

            // We use the "->" arrow operator to access memember of an object through a pointer
            newPawn->Setup(loc, Unit::UnitColor_Black);

            AddUnit(newPawn);
        }

    }

    // Rooks
    {
        Unit* rook[] =
        {
            new Rook(),
            new Rook(),
            new Rook(),
            new Rook(),
        };
        rook[0]->Setup({ 0,0 }, Unit::UnitColor_Black);
        rook[1]->Setup({ width - 1,0 }, Unit::UnitColor_Black);
        rook[2]->Setup({ 0,height - 1 }, Unit::UnitColor_White);
        rook[3]->Setup({ width - 1,height - 1 }, Unit::UnitColor_White);
        AddUnit(rook[0]);
        AddUnit(rook[1]);
        AddUnit(rook[2]);
        AddUnit(rook[3]);
    }
    // I sure hope this (the curly brackets in non-conditionals) doesn't cause issues. I just wanted it to take up less space.

    // Knights
    {
        Unit* knight[4] =
        {
            new Knight(),
            new Knight(),
            new Knight(),
            new Knight(),
        };
        knight[0]->Setup({ 1,0 }, Unit::UnitColor_Black);
        knight[1]->Setup({ width - 2,0 }, Unit::UnitColor_Black);
        knight[2]->Setup({ 1,height - 1 }, Unit::UnitColor_White);
        knight[3]->Setup({ width - 2,height - 1 }, Unit::UnitColor_White);
        AddUnit(knight[0]);
        AddUnit(knight[1]);
        AddUnit(knight[2]);
        AddUnit(knight[3]);
    }

    // Bishops
    {
        Unit* bishop[4] =
        {
            new Bishop(),
            new Bishop(),
            new Bishop(),
            new Bishop(),
        };
        bishop[0]->Setup({ 2,0 }, Unit::UnitColor_Black);
        bishop[1]->Setup({ width - 3,0 }, Unit::UnitColor_Black);
        bishop[2]->Setup({ 2,height - 1 }, Unit::UnitColor_White);
        bishop[3]->Setup({ width - 3,height - 1 }, Unit::UnitColor_White);
        AddUnit(bishop[0]);
        AddUnit(bishop[1]);
        AddUnit(bishop[2]);
        AddUnit(bishop[3]);
    }

    // Queens
    {
        Unit* queen[] =
        {
            new Queen(),
            new Queen(),
        };
        queen[0]->Setup({ 3,0 }, Unit::UnitColor_Black);
        queen[1]->Setup({ 3,height - 1 }, Unit::UnitColor_White);
        AddUnit(queen[0]);
        AddUnit(queen[1]);

        // Kings
        Unit* king[] =
        {
            new King(),
            new King(),
        };
        king[0]->Setup({ 4,0 }, Unit::UnitColor_Black);
        king[1]->Setup({ 4,height - 1 }, Unit::UnitColor_White);
        AddUnit(king[0]);
        AddUnit(king[1]);
    }

}

void Board::PlayBoard()
{
    // @TODO: Handle input and diplay the board to the players.
    // Each player takes turns going until one player's
    // king has been killed.

    std::cout << "Select piece: ";
    char inputX = 'a';
    int inputY = 1;
    std::cin >> inputX >> inputY;

    inputY = height - inputY;
    int intputX = LetterToNum(inputX);

    Location search = { intputX, inputY };
    Unit* found = GetUnit(search);
    char unitAscii = ' ';
    if (found != nullptr)
    {
        unitAscii = found->GetUnitAscii();
    }
    std::cout << CURSOR_SAVE << CURSOR_MOVE(inputY, intputX) << FG_SELECT << " # " << FG_RESET << CURSOR_LOAD;

    std::cout << "Select move: ";
    char cmoveX = 'a';
    int moveY = 1;
    std::cin >> cmoveX >> moveY;

    moveY = height - moveY;
    int moveX = LetterToNum(cmoveX);

}

void Board::PrintBoard()
{
    // Feel free to make a much better printing format
    // that the one above as I don't really like it bth.

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Location search;
            search.x = x;
            search.y = y;
            Unit* found = GetUnit(search);

            std::cout << BG_RESET << (((x + y) % 2 == 0) ? BG_WHITE : BG_BLACK) << " "; // Space coloration

            if (found != nullptr)
            {
                if (found->GetColor() == Unit::UnitColor_White)
                {
                    std::cout << FG_WHITE << found->GetUnitAscii();
                }
                else
                {
                    std::cout << FG_BLACK << found->GetUnitAscii();
                }
            }
            else // space is empty
            {
                std::cout << " ";
            }
            std::cout << " ";
        }
        std::cout << BG_RESET << FG_RESET << FG_SUB << " " << height - y << '\n';
    }
    std::cout << BG_RESET << FG_RESET << FG_SUB;
    for (int x = 0; x < width; ++x) {
        std::cout << " " << NumToLetter(x) << " ";
    }
    std::cout << FG_RESET << '\n';
}

void Board::AddUnit(Unit* unit)
{
    if (unit->GetColor() == Unit::UnitColor_Black)
    {
        blackUnits.push_back(unit);
    }
    else
    {
        whiteUnits.push_back(unit);
    }
}

void Board::RemoveUnit(Unit* unit)
{
    // @TODO
}
