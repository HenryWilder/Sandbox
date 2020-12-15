#include "Board.h"
#include <string>
#define CURRENT_TEAM UnitColor(Even(turn))
#define TEAM_NAME(team) ((team) == UnitColor::Unit_White ? "white" : "black")
#define RESTORE_CURSOR "\x1b[u"

char NumToLetter(int value)
{
    return value + 'a';
}

int LetterToNum(char letter)
{
    return letter - 'a';
}

char CharToValue(char character)
{
    char out = character;
    if (out >= '1' && out <= '8') // A number (1-8)
        out = out - '1';
    else if (out >= 'a' && out <= 'h') // A lowercase letter (a-h)
        out = out - 'a';
    else if (out >= 'A' && out <= 'H') // an uppercase letter (A-H)
        out = out - 'A';
    else
        out = 9; // Out of bounds

    return out;
}

void ClearTextLine(int start, int count)
{
    std::cout << RESTORE_CURSOR;

    // Move to start
    for (int row = 0; row < start; ++row)
        std::cout << '\n';

    // Clear the lines by filling them with spaces
    for (int row = 0; row < count; ++row)
        std::cout << "                                                                              \n";

    std::cout << RESTORE_CURSOR;

    // Reset to start
    for (int row = 0; row < start; ++row)
        std::cout << '\n';
}

Unit* Board::GetTeamUnitAtPos(Coord pos, UnitColor team)
{
    std::vector<Unit*>* teamArray = GetTeamArray(team);

    // Look at all black units
    for (Unit* checkUnit : *teamArray)
    {
        if (checkUnit->GetLocation() == pos) {
            return checkUnit;
            break;
        }
    }

    return nullptr;
}

Unit* Board::GetUnitAtPos(Coord pos)
{
    Unit* out = GetTeamUnitAtPos(pos, UnitColor::Unit_Black);
    if (out != nullptr)
        return out;
    else
        return GetTeamUnitAtPos(pos, UnitColor::Unit_White);
}

bool Board::IsTeamAtPos(Coord pos, UnitColor team)
{
    const std::vector<Unit*>* teamArray = GetTeamArrayReadOnly(team);

    for (const Unit* checkUnit : *teamArray)
    {
        if (checkUnit->GetLocation() == pos)
        {
            return true;
            break;
        }
    }

    return false;
}

bool Board::IsUnitAtPos(Coord pos)
{
    return IsTeamAtPos(pos, UnitColor::Unit_Black) || IsTeamAtPos(pos, UnitColor::Unit_White);
}

void Board::ResetBoard(int _width, int _height)
{
    if (_height == 0) { _height = _width; }

    this->width = _width;
    this->height = _height;
    this->turn = 0;

    unsigned char unitID = 0;

    // Black units
    // Clear
    for (Unit* unit : blackUnits)
        RemoveUnit(unit);

    // Construct
    for (int x = 0; x < width; ++x)
        ConstructNewUnit({ x,1 }, Piece::Piece_Pawn, UnitColor::Unit_Black, unitID);

    BuildRoyalty(0, UnitColor::Unit_Black, unitID);

    // White units
    // Clear
    for (Unit* unit : whiteUnits)
        RemoveUnit(unit);

    // Construct
    for (int x = 0; x < width; ++x)
        ConstructNewUnit({ x,height - 2 }, Piece::Piece_Pawn, UnitColor::Unit_White, unitID);

    BuildRoyalty(height - 1, UnitColor::Unit_White, unitID);

    PrintBoard();
}

void Board::DrawBoardSpaceColored(Coord pos, COLORREF color)
{
    Unit* unit = GetUnitAtPos(pos); // Find the unit at the space

    if (unit != nullptr && // Make sure there was actually a piece
        !unit->IsHidden()) // Don't render hidden pieces
    {
        g_frameBuffer.DrawSpriteFASTWithBG( // Draw the unit sprite
            pos, // Which space
            unit->GetSpritePointer(), // The sprite to draw
            bool(unit->GetColor()), // Which team the unit is on
            color); // The color of the background
    }
    else g_frameBuffer.DrawGridSpaceFAST(pos, color);
}

void Board::DrawBoardSpaceColored(Coord pos, Color color)
{
    DrawBoardSpaceColored(pos, CRef(color));
}

// Draws the space as it is without any selection or effects. Draws the unit there if there is one.
void Board::DrawBoardSpaceReset(Coord pos)
{
    DrawBoardSpaceColored(pos, g_frameBuffer.SpacePatternAtPos(pos)); // Draws the space with the background color being the color of the space
}

bool InString(const char* string, const char* substring)
{
    int i = 0;

    do {
        if (*(string + i) == *substring) // First character
        {
            bool match = true;

            int n = 0;

            do {
                char strHere = *(string + i + n);

                char subStrHere = *(substring + n);

                if (strHere != subStrHere) // Nth character
                {
                    match = false;
                    break;
                }
                ++n;

            } while (*(substring + n));

            if (match)
                return true;
        }
        ++i;

    } while (*(string + i));

    return false;
}

//
//
// GAMEPLAY
//
//

/* Made obsolete by WaitForClick()
Coord Board::TakePosInput()
{
    std::cout << " (xy): ";

    char input[5];

    std::cin >> input[0] >> input[1];

    input[4] = 0;

    if (input[0] == 'm')
    {
        std::cin >> input[2] >> input[3];
        std::cout << input << "\n";
        return Coord{ -1,-1 };
    }
    else // User commands
    {
        char inputx = CharToValue(input[0]);
        char inputy = CharToValue(input[1]);
        
        //cout << inputx << inputy << ")\n"; // Debug

        Coord out = { (int)inputx, int(height - inputy) - 1 };

        //cout << '(' << out.x << ", " << out.y << ")\n"; // Debug

        return out;
    }
}
*/

Coord Board::WaitForClick()
{
    InitInput();

    Coord mouseCoordLast = { 0,0 };
    Coord mouseCoordCurrent = { 0,0 };

    COLORREF lastSpaceColor = g_frameBuffer.Get(PixelSpace(mouseCoordLast));

    while (true)
    {
        PingInput();
        mouseCoordCurrent = g_mouse.ReadMouseHover();
        if (mouseCoordLast != mouseCoordCurrent)
        {
            DrawBoardSpaceColored(mouseCoordLast, lastSpaceColor);

            mouseCoordLast = mouseCoordCurrent;

            lastSpaceColor = g_frameBuffer.Get(PixelSpace(mouseCoordCurrent));

            DrawBoardSpaceColored(mouseCoordCurrent, RGB(127, 127, 255));
        }

        if (g_mouse.CheckMouseState()) { // Click is true

            DrawBoardSpaceColored(mouseCoordCurrent, RGB(64, 64, 255));

            SleepForMS(127);

            DrawBoardSpaceColored(mouseCoordCurrent, lastSpaceColor);

            break;
        }
        else SleepForMS(16.7); // Otherwise sleep
    }
    return g_mouse.GetMouseClickCoord();
}

void Board::MovePiece(Unit* unit, Coord moveTo)
{
    unit->Hide();
    //DrawBoardSpaceReset(unit->GetLocation()); // Sanity check

    Ghost unitGhost = Ghost(
        unit->GetLocation(),
        unit->GetSpritePointer(),
        (bool)unit->GetColor());

    unitGhost.Glide(moveTo);

    Unit* enemyToTake = GetUnitAtPos(moveTo);

    if (enemyToTake != nullptr) // The piece will have already trimmed any positions which have a teammate in them
        RemoveUnit(enemyToTake);

    unit->Move(moveTo); // Move the piece before unhiding

    unit->UnHide();

    DrawBoardSpaceReset(moveTo); // Redraw the space we've moved to now that we're no longer hidden
}

bool Board::SelectPhase(const UnitColor team, Coord& input, Unit* unit, PieceMoves* moves)
{
    ClearTextLine(0);

    std::cout << "Which piece would you like to move?";

    input = WaitForClick();

    if (input.x > width || input.y > height) // Out of bounds
    {
        std::cout << "OUT OF BOUNDS";
        return false;
    }
    unit = GetUnitAtPos(input); // No unit at selection
    if (unit == nullptr)
    {
        std::cout << "That space is empty.";
        return false;
    }

    if (unit->GetColor() != team) // Wrong team
    {
        std::cout << "You are on team " << TEAM_NAME(team) << ".\n" <<
            "Please choose a " << TEAM_NAME(team) << " piece.";
        return false;
    }

    return true;
}
bool Board::HandleSelection(Coord input, Unit* unit, PieceMoves* moves)
{
    unit->AvailableMoves(moves);

    if (moves->m_availableMovesCount == 0)
    {
        std::cout << "That piece is stuck.";
        return false;
    }
    // Draw the selection square
    DrawBoardSpaceColored(input, sprite::PaletteColor(sprite::Pltt::Select_Unit));

    // Draw the available positions
    for (unsigned char i = 0; i < moves->m_availableMovesCount; ++i)
    {
        Coord potentialMove = moves->m_available[i]; // The move we are currently looking at/drawing

        sprite::Pltt spaceColoration = sprite::Pltt::Select_Available; // Default with selection color

        Unit* potentialEnemy = GetUnitAtPos(potentialMove);

        if (potentialEnemy != nullptr && (potentialEnemy->GetColor() == unit->GetEnemyColor()))
        {
            spaceColoration = sprite::Pltt::Select_TakePiece; // Take-piece color
        }

        DrawBoardSpaceColored(potentialMove, sprite::PaletteColor(spaceColoration)); // Color the space
    }

    return true;
}
int Board::MovePhase(Coord& input, Coord& output, PieceMoves* moves, const UnitColor team)
{
    // Clear the input area
    ClearTextLine(2, 6);

    ClearTextLine(2);
    std::cout << "Which space would you like to move to?";

    output = WaitForClick();

    if (output.x > width || output.y > height) // Out of bounds
    {
        std::cout << "OUT OF BOUNDS\n";
        return 0;
    }

    if (GetUnitAtPos(output)->GetColor() == team) // Change selection
    {
        input = output;
        return 2;
    }

    if (!(moves->MoveIsValid(output))) // Not a legal move
    {
        std::cout << "This is not an available position.\n";
        return 0;
    }

    return 1;
}
bool Board::WrapUpTurn(Coord& input, Coord& output, Unit* unit, PieceMoves* moves)
{
    // Reset the potential positions
    for (unsigned char i = 0; i < moves->m_availableMovesCount; ++i) {
        DrawBoardSpaceReset(moves->m_available[i]);
    }

    MovePiece(unit, output); // Move the piece

    DrawBoardSpaceReset(input); // Clear the old space
    DrawBoardSpaceReset(output); // Draw the piece at the new space

    // Clear the input area
    ClearTextLine(0, 6);

    IncrementTurn();

    return true;
}

void Board::PlayBoard()
{
    const UnitColor team = CURRENT_TEAM;
    Coord input; // User input for start position
    Coord output; // User input for end position
    Unit* unit = nullptr; // Unit at the user-input coorinates
    PieceMoves moves{}; // What moves can be made by the unit
    bool hasMoved = false;

    enum class Phase {
        Select = 0,
        HandleSelect = 1,
        Move = 2,
        Wrapup = 3,
    };

    Phase turnPhase = Phase::Select;

    // State machine
    while (!hasMoved)
    {
        switch (turnPhase) {
        default: // Select
            if (SelectPhase(team, input, unit, &moves)) turnPhase = Phase::Move;
            else break;

        case Phase::HandleSelect:
            if (HandleSelection(input, unit, &moves)) turnPhase = Phase::Move;
            else turnPhase = Phase::Select;

        case Phase::Move:
            switch (MovePhase(output, &moves, team)) {
            default: // Failed
                break;
            case 1: // Went well
                turnPhase = Phase::Wrapup;
                break;
            case 2: // Player selected a different piece on their team; reset to Select phase
                turnPhase = Phase::Select;
                break;
            }
            break;

        case Phase::Wrapup:
            if (WrapUpTurn(input, output, unit, &moves)) hasMoved = true;
        }
    }
}

void Board::PrintBoard()
{
    Coord space;
    // Cycle through all rows
    for (space.y = 0; space.y < space::game::sideTileCount; ++space.y)
    {
        // Cycle through all columns
        for (space.x = 0; space.x < space::game::sideTileCount; ++space.x)
        {
            DrawBoardSpaceReset(space);
        }
    }

    g_frameBuffer.Draw(); // Render the board
}

/*
* 
//
// MODIFIED NOVEMBER 28
// WAS NOT WORKING BEFORE, PROBABLY NOT WORKING STILL. HAS BEEN COMMENTED FOR CLARIFICATION.
//

// @Possible issue due to this not being in the .h file? but it gave errors when it WAS in there...
namespace Render
{
    // This makes working with colors SOOO much easier
    struct ConsoleColor
    {
    public: // Constructors

        // Empty color
        ConsoleColor()
        {
            char temp[21] = {
                '\x1b', '[', '3', '8', ';', '2', ';', // Escape & color declairation
                '0', '0', '0', ';', // Red (7-9)
                '0', '0', '0', ';', // Green (11-13)
                '0', '0', '0', ';', // Blue (15-17)
                'm', '\0'
            };

            for (unsigned int i = 0; i < 21; ++i)
            {
                m_colorCode[i] = temp[i];
            }
        }

        // Construct a color from RGB inputs
        ConsoleColor(int r, int g, int b, bool b_isBackground = true)
        {
            ConsoleColor(); // Not sure if this is a bad way to do it...
            Set(r, g, b); // I hope this is allowed
            SetColorLayer(b_isBackground); // If the line above works then this should too
        }

    private:
        // Color layout
        char m_colorCode[21];

        // Convert an input int value to a char[3] and apply it to the appropriate color component
        // This function is private to keep it from being used by outside functions that might not know the correct startIndex values.
        // It should only be used internally by the ConsoleColor type.
        void SetRGBComp(int value, int startIndex)
        {
            m_colorCode[startIndex] = '0' + ((value / 100) % 10);
            m_colorCode[startIndex + 1] = '0' + ((value / 10) % 10);
            m_colorCode[startIndex + 2] = '0' + value % 10;
        }
        
    public:
        // Changes whether this is a background color or a foreground color
        void SetColorLayer(bool b_isBackground)
        {
            m_colorCode[2] = (b_isBackground ? '4' : '3');
        }

        // Convert the color to background
        void BG()
        {
            SetColorLayer(1);
        }

        // Convert the color to foreground
        void FG()
        {
            SetColorLayer(0);
        }

        // Set the color
        void Set(int r, int g, int b)
        {
            SetRGBComp(r, 7);
            SetRGBComp(g, 11);
            SetRGBComp(b, 15);
        }

        // Return the charset
        const char* Get()
        {
            return reinterpret_cast<const char*>(&m_colorCode);
        }

        // Apply the color to the console
        void Apply()
        {
            std::cout << Get();
        }
    };

    // Reset colors
    const char* BG_RESET = "\x1b[49;0m";
    const char* FG_RESET = "\x1b[39;0m";

    // Resets both the foreground & background color at once
    void ResetColor()
    {
        std::cout << "\x1b[49;0m\x1b[39;0m";
    }

    // Custom colors
    ConsoleColor BG_BLACK = ConsoleColor(20, 20, 20); // Board space
    ConsoleColor FG_BLACK = ConsoleColor(236, 167, 95, false); // Game piece

    ConsoleColor BG_WHITE = ConsoleColor(256, 256, 256); // Board space
    ConsoleColor FG_WHITE = ConsoleColor(252, 219, 166, false); // Game piece

    // Space/piece selection colors
    ConsoleColor BG_SELECT = ConsoleColor(44, 200, 37);
    ConsoleColor FG_SELECT = ConsoleColor(155, 235, 135, false); 

    // Edge coordinate text color
    ConsoleColor FG_COORD = ConsoleColor(100, 100, 100, false);
}

using namespace Render; // We're about to use this namespace a LOT


void Board::PrintBoard() // @TODO: Something is severely wrong with this function, as nothing is being rendered. I'm not sure what the issue is though.
{
    // Cycle through all rows
    for (int y = 0; y < height; ++y)
    {
        // Cycle through all columns
        for (int x = 0; x < width; ++x)
        {
            Unit* found = GetUnitAtPos({ x,y }); // Does exactly as the name implies

            ResetColor(); // @TODO: Fixes a weird bug that happens sometimes. Something to look in to.

            ((((x + y) % 2) == 0) ? BG_WHITE : BG_BLACK).Apply(); // Space coloration

            std::cout << " "; // Empty space for cursor

            {
                char unitSymbol = ' '; // Defaults as empty until overwritten

                // Find any pieces occupying the space
                if (found != nullptr) // "If there is a piece here"
                {
                    (bool(found->GetColor()) ? FG_WHITE : FG_BLACK).Apply(); // Set space foreground to match unit color

                    unitSymbol = found->GetSymbol(); // Overwrite the empty space with the unit symbol
                }

                std::cout << unitSymbol; // Draws whatever unit (or lack of a unit) is in the space
            }
        }

        ResetColor(); // @TODO: Again, we should really look into why this is needed

        FG_COORD.Apply(); // Coord text coloration

        std::cout << " " << height - y << '\n'; // Print y-coord and newline
    }

    ResetColor(); // @TODO: omfg this is upsetting

    FG_COORD.Apply(); // Coord text coloration

    // At this point we are below the bottom of the board, so we create a new loop for the x-coordinates
    for (int x = 0; x < width; ++x)
    {
        std::cout << " " << NumToLetter(x) << " "; // Print x-coord
    }

    ResetColor(); // Ok this one is actually on purpose. At this point we've reached the end of the chessboard and we want the user input to be the regular colors.

    std::cout << '\n'; // Go to the next line (to make it look nice when the player is promted for input)
}
*/
//
// End of November 28 modification & clarification.
//
//

std::vector<Unit*>* Board::GetTeamArray(UnitColor team)
{
    if (team == UnitColor::Unit_Black)
        return &blackUnits;
    else
        return &whiteUnits;
}

const std::vector<Unit*>* Board::GetTeamArrayReadOnly(UnitColor team)
{
    const std::vector<Unit*>* teamArray = GetTeamArray(team);
    return teamArray;
}

void Board::BuildRoyalty(int y, UnitColor col, unsigned char& unitID)
{
    for (int x = 0; x < width; ++x)
    {
        Piece type;
        type = Piece();
        int i = (x > (width / 2) ? (width)-x - 1 : x);
        switch (i) {
        default:
            type = Piece::Piece_Rook;
            break;
        case 1:
            type = Piece::Piece_Knight;
            break;
        case 2:
            type = Piece::Piece_Bishop;
            break;
        case 3:
            type = Piece::Piece_Queen;
            break;
        case 4:
            type = Piece::Piece_King;
            break;
        }
        ConstructNewUnit({ x,y }, type, col, unitID);
    }
}

void Board::ConstructNewUnit(Coord pos, Piece type, UnitColor color, unsigned char& unitID)
{
    Unit* newUnit;
    switch (type)
    {
    default: // Default is pawn
        newUnit = new Pawn();
        break;

    case Piece::Piece_Knight:
        newUnit = new Knight();
        break;

    case Piece::Piece_Rook:
        newUnit = new Rook();
        break;

    case Piece::Piece_Bishop:
        newUnit = new Bishop();
        break;

    case Piece::Piece_Queen:
        newUnit = new Queen();
        break;

    case Piece::Piece_King:
        newUnit = new King();
        break;
    }
    newUnit->Init(pos, color, this, unitID++);
    AddUnit(newUnit);
}

void Board::AddUnit(Unit* unit)
{
    GetTeamArray(unit->GetColor())->push_back(unit);
}

void Board::RemoveUnit(Unit* unit)
{
    unsigned char id = unit->GetID();

    unsigned int i = 0;

    Coord unitSpace = unit->GetLocation(); // For resetting the space after we've destroyed the unit

    std::vector<Unit*>& teamArray = *GetTeamArray(unit->GetColor());

    for (i; i < teamArray.size(); ++i)
        if (teamArray[i]->GetID() == id)
            break;

    teamArray.erase(teamArray.begin() + i);

    DrawBoardSpaceReset(unitSpace);
}

void Board::IncrementTurn()
{
    // @TODO: Store an array of all pieces on the board so that we can look through the game like a flipbook

    ++turn;
}
