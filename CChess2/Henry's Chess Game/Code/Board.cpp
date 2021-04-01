#include "Board.h"
#include <string>
#define CURRENT_TEAM UnitColor(Even(turn))
#define OPPONENT_TEAM UnitColor(!Even(turn))
#define TEAM_NAME(team) ((team) == UnitColor::Unit_White ? "white" : "black")
#define RESTORE_CURSOR "\x1b[u"
#define CURSOR_TO_POSITION(x,y) ('\x1b','[',#x,';',#y,'f')

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

    std::cout << CURSOR_TO_POSITION(0, 0);

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

void Board::DrawBoardSpaceColored(Coord pos, COLORREF color, bool effect)
{
    Unit* unit = GetUnitAtPos(pos); // Find the unit at the space

    if (unit != nullptr && // Make sure there was actually a piece
        !unit->IsHidden()) // Don't render hidden pieces
    {
        g_frameBuffer.DrawSpriteFASTWithBG( // Draw the unit sprite
            pos, // Which space
            unit->GetSpritePointer(), // The sprite to draw
            bool(unit->GetColor()), // Which team the unit is on
            color, // The color of the background
            effect); // Whether to use the ghost effect
    }
    else g_frameBuffer.DrawGridSpaceFAST(pos, color);
}

void Board::DrawBoardSpaceColored(Coord pos, Color color, bool effect)
{
    DrawBoardSpaceColored(pos, CRef(color), effect);
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

Coord Board::WaitForClick(Phase turnPhase, const PieceMoves* pMoves, const sprite::Sprite* sprite, bool team)
{
    InitInput();

    Coord mouseCoordLast = { 0,0 };
    Coord mouseCoordCurrent = { 0,0 };

    COLORREF lastSpaceColor = g_frameBuffer.Get(PixelSpace(mouseCoordLast));

    while (true)
    {
        PingInput(); // Update the mouse location

        mouseCoordCurrent = g_mouse.ReadMouseHover();

        if (mouseCoordLast != mouseCoordCurrent) // If the mouse location has changed
        {
            if (ValidPos(mouseCoordLast))
                DrawBoardSpaceColored(mouseCoordLast, lastSpaceColor); // Reset the old space's color

            mouseCoordLast = mouseCoordCurrent; // Update the temporary mouse position variable

            if (ValidPos(mouseCoordCurrent))
            {
                lastSpaceColor = g_frameBuffer.Get(PixelSpace(mouseCoordCurrent)); // Store the color of the new space before we recolor it

                // Draw transparent image of our piece at the move we are hovering over
                if (turnPhase == Phase::Move && pMoves != nullptr && pMoves->MoveIsValid(mouseCoordCurrent))
                    g_frameBuffer.DrawSpriteFASTWithBG(mouseCoordCurrent, sprite, team, lastSpaceColor, true);

                else DrawBoardSpaceColored(mouseCoordCurrent, RGB(127, 127, 255)); // Color the new space
            }
        }

        if (g_mouse.CheckMouseState()) // Click is true
       {
            //DrawBoardSpaceColored(mouseCoordCurrent, RGB(64, 64, 255)); // Click color

            //SleepForMS(127); // Delay

            DrawBoardSpaceColored(mouseCoordCurrent, lastSpaceColor); // Reset space color

            break;
        }
        //else SleepForMS(16.7); // Otherwise sleep (feels less responsive)
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

    Pawn* pawn = dynamic_cast<Pawn*>(unit);

    if (pawn != nullptr) // Is a pawn
    {
        Coord passantPos = { moveTo.x, moveTo.y - pawn->GetForward() }; // Subtract because we are looking for the *other pawn's* direction
        Pawn* passantPawn = dynamic_cast<Pawn*>(GetUnitAtPos(passantPos));
        if (passantPawn != nullptr && passantPawn->GetColor() != pawn->GetColor() && passantPawn->en_pasant)
        {
            RemoveUnit(passantPawn);
            DrawBoardSpaceReset(passantPos);
        }

        if (unit->GetLocation().y == space::game::sideTileCount || unit->GetLocation().y == 0) // On the other side of the board
        {
            unsigned char id = unit->GetID();
            ConstructNewUnit(unit->GetLocation(), Piece::Piece_Queen, unit->GetColor(), id);
            RemoveUnit(unit);
        }
    }

    DrawBoardSpaceReset(moveTo); // Redraw the space we've moved to now that we're no longer hidden
}

void Board::DrawPossibleMoves(PieceMoves* moves, const UnitColor team)
{
    // Draw the available positions
    for (unsigned char i = 0; i < moves->m_availableMovesCount; ++i)
    {
        Coord pMove = moves->m_available[i]; // The move we are currently looking at/drawing
        Unit* pEnemy = GetUnitAtPos(pMove); // The (potential) enemy at that space

        sprite::Pltt spaceColor = sprite::Pltt::Select_Available; // Default with selection color

        if (pEnemy != nullptr)
        {
            if (pEnemy->GetColor() != team) // Unit exists at the space and is an enemy
                spaceColor = sprite::Pltt::Select_TakePiece; // Change the color to "TakePiece"
            else
                spaceColor = sprite::Pltt::NoSelect_Teammate; // Change the color to "TakePiece"
        }
        
        DrawBoardSpaceColored(pMove, sprite::PaletteColor(spaceColor)); // Color the space
    }
}

void Board::UnDrawPossibleMoves(PieceMoves* moves)
{
    // Reset the potential positions
    for (unsigned char i = 0; i < moves->m_availableMovesCount; ++i) {
        DrawBoardSpaceReset(moves->m_available[i]);
    }
}

bool Board::SelectPhase(const UnitColor team, Coord& input, Unit*& unit, PieceMoves* moves)
{
    //ClearTextLine(0);

    //std::cout << "Which piece would you like to move?\n";

    input = WaitForClick(Phase::Select);

    //ClearTextLine(1);

    if (input.x > width || input.y > height) // Out of bounds
    {
        //std::cout << "OUT OF BOUNDS";
        return false;
    }
    else
    {
        unit = GetUnitAtPos(input); 

        if (unit == nullptr) // No unit at selection
        {
            //std::cout << "That space is empty.";
            return false;
        }
        else if (unit->GetColor() != team) // Wrong team
        {
            //std::cout << "You are on team " << TEAM_NAME(team) << ".\n" << "Please choose a " << TEAM_NAME(team) << " piece.";
            return false;
        }
        else return true; // Unit exists & is correct team
    }
}
bool Board::HandleSelection(Coord input, Unit*& unit, PieceMoves* moves)
{
    unit->AvailableMoves(moves);

    if (moves->m_availableMovesCount == 0)
    {
        //std::cout << "That piece is stuck.";
        return false;
    }
    else
    {
        // Draw the selection square
        DrawBoardSpaceColored(input, sprite::PaletteColor(sprite::Pltt::Select_Unit));

        // Draw all legal moves that can be made by this piece
        DrawPossibleMoves(moves, unit->GetColor());

        // Clear the input area before continuing
        //ClearTextLine(0, 6);

        return true;
    }
}
int Board::MovePhase(Coord& input, Coord& output, Unit*& unit, PieceMoves* moves, const UnitColor team)
{
    //ClearTextLine(0);

    //std::cout << "Which space would you like to move to?";

    output = WaitForClick(Phase::Move, moves, unit->GetSpritePointer(), (bool)team);

    if (output.x > width || output.y > height) // Out of bounds
    {
        //std::cout << "OUT OF BOUNDS\n";
        return 0;
    }
    else
    {
        Unit* checkUnit = GetUnitAtPos(output);

        if (checkUnit != nullptr && checkUnit->GetColor() == team) // Change selection
        {
            DrawBoardSpaceReset(input);
            UnDrawPossibleMoves(moves);
            input = output;
            unit = checkUnit;
            return 2;
        }
        else if (!(moves->MoveIsValid(output))) // Not a legal move
        {
            //std::cout << "This is not an available position.\n";
            return 0;
        }
        else return 1;
    }
}
bool Board::WrapUpTurn(Coord& input, Coord& output, Unit*& unit, PieceMoves* moves)
{
    UnDrawPossibleMoves(moves); // Clear the move highlighting

    DrawBoardSpaceReset(input); // Clear the old space

    MovePiece(unit, output); // Move the piece

    // Clear the input area
    //ClearTextLine(0, 6);

    return true;
}

Unit* Board::FindKingFromTeam(UnitColor team)
{
    const std::vector<Unit*>* teamArray = GetTeamArray(team);

    // Find the king in the team array
    for (Unit* unit : *teamArray) {
        if (unit->GetPieceType() == Piece::Piece_King) return unit;
    }

    return nullptr; // Defaults to current position
}

void Board::PlayBoard()
{
    const UnitColor team = CURRENT_TEAM;
    Coord input; // User input for start position
    Coord output; // User input for end position
    Unit* unit = nullptr; // Unit at the user-input coorinates
    PieceMoves moves{}; // What moves can be made by the unit
    bool hasMoved = false;

    Phase turnPhase = Phase::Select;

    while (!hasMoved)
    {
        // State machine
        switch (turnPhase) {
        default: // Select
            if (SelectPhase(team, input, unit, &moves)) turnPhase = Phase::Move;
            else break; // Failed, retry

        case Phase::HandleSelect:
            if (HandleSelection(input, unit, &moves)) turnPhase = Phase::Move;
            else { // HandleSelect phase cannot reconsile on its own and must resort to retrying the Select phase.
                turnPhase = Phase::Select;
                break;
            }

        case Phase::Move:
            switch (MovePhase(input, output, unit, &moves, team))
            {
            default: // Failed (0)
                break;
            case 1: // Went well
                turnPhase = Phase::Wrapup;
                break;
            case 2: // Player changed their selection
                turnPhase = Phase::HandleSelect;
                break;
            }
            if (turnPhase != Phase::Wrapup) break; // We want to break out of the state machine if the return was unsuccessful

        case Phase::Wrapup:
            WrapUpTurn(input, output, unit, &moves);
            hasMoved = true;
            break;
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

void Board::ResetEnPasant()
{
    std::vector<Unit*>* teamArray = GetTeamArray(CURRENT_TEAM);
    
    for (Unit* unit : *teamArray)
    {
        Pawn* pawn = dynamic_cast<Pawn*>(unit);
        if (pawn != nullptr && pawn->en_pasant)
        {
            pawn->en_pasant = false;
            break; // Only one pawn can have moved in a turn
        }
    }
}

void Board::StoreBoardState()
{
    for (int y = 0; y < space::game::sideTileCount; ++y) {
        for (int x = 0; x < space::game::sideTileCount; ++x)
        {
            Unit* unit = GetUnitAtPos({ x,y });
            m_history[turn].SetStateSpace(x, y, MakeUnitData(unit));
        }
    }
}

bool Board::IncrementTurn()
{
    King* king = dynamic_cast<King*>(FindKingFromTeam(CURRENT_TEAM));

    if (king == nullptr) // Game over
    {
        return true;
    }
    else
    {
        if (!king->SpaceIsSafeFromCheck()) DrawBoardSpaceColored(king->GetLocation(), RGB(255, 80, 80)); // King in check
        else  DrawBoardSpaceReset(king->GetLocation()); // King not in check
        
        StoreBoardState();

        ++turn;
        
        // Start the next turn

        ResetEnPasant(); 

        king = dynamic_cast<King*>(FindKingFromTeam(CURRENT_TEAM));

        if (king == nullptr) // Game over
        {
            return true;
        }
        else
        {
            if (!king->SpaceIsSafeFromCheck()) DrawBoardSpaceColored(king->GetLocation(), RGB(255, 80, 80)); // King in check
            else  DrawBoardSpaceReset(king->GetLocation()); // King not in check
            return false; // Game is not over
        }
    }
}

int BoardStateMemory::Index(int x, int y) const
{
    return (space::game::sideTileCount) * y + x;
}

char MakeUnitData(Unit* unit)
{
    char data = 0b00000000; // Space is empty by default

    if (unit != nullptr)
    {
        data = char(unit->GetPieceType());
        if (unit->GetColor() == UnitColor::Unit_White) data |= 0b1000;
    }

    return data;
}

const char BoardStateMemory::GetUnitData(int x, int y) const
{
    return m_stateData[Index(x, y)];
}

UnitColor BoardStateMemory::GetUnitData_Color(const char data) const
{
    if (data & 0b1000) return UnitColor::Unit_White;
    else return UnitColor::Unit_Black;
}

UnitColor BoardStateMemory::GetUnitData_Color(int x, int y) const
{
    return GetUnitData_Color(GetUnitData(x, y));
}

Piece BoardStateMemory::GetUnitData_Piece(const char data) const
{
    char check = data;
    if (check >= 8) check -= 8;
    for (char test = 1; test < 7; ++test)
    {
        if (check == test) return Piece(test);
    }
    return Piece::Piece_NULL;
}

Piece BoardStateMemory::GetUnitData_Piece(int x, int y) const
{
    return GetUnitData_Piece(GetUnitData(x, y));
}

void BoardStateMemory::SetStateSpace(int x, int y, const char data)
{
    m_stateData[Index(x, y)] = data; // Set
}

void Board::DrawBoardState(int state)
{
    system("CLS");
    std::cout << "\x1b[?25l"; // Hide the cursor

    BoardStateMemory& gameState = m_history[state];

    for (int y = 0; y < space::game::sideTileCount; ++y) {
        for (int x = 0; x < space::game::sideTileCount; ++x)
        {
            Coord space = { x, y };

            UnitColor color;
            color = gameState.GetUnitData_Color(x, y);

            Piece piece = gameState.GetUnitData_Piece(x, y);

            if (piece != Piece::Piece_NULL)
            {
                sprite::Sprite* sprite;
                switch (piece)
                {
                case Piece::Piece_Pawn: // Default is pawn
                    sprite = &sprite::unit::pawn;
                    break;

                case Piece::Piece_Knight:
                    sprite = &sprite::unit::knight;
                    break;

                case Piece::Piece_Rook:
                    sprite = &sprite::unit::rook;
                    break;

                case Piece::Piece_Bishop:
                    sprite = &sprite::unit::bishop;
                    break;

                case Piece::Piece_Queen:
                    sprite = &sprite::unit::queen;
                    break;

                case Piece::Piece_King:
                    sprite = &sprite::unit::king;
                    break;

                default: // Null
                    sprite = &sprite::unit::null;
                    break;
                }
                g_frameBuffer.DrawSpriteFASTWithBG(space, sprite,(bool)color, g_frameBuffer.SpacePatternAtPos(space));
            }
            else
            {
                g_frameBuffer.DrawBoardPattern1SpaceFAST(space);
            }
        }
    }
}

int Board::FlipbookWFClick(int state)
{
    // Initialization
    Coord arrowPosR;
    arrowPosR.x = space::game::sideTileCount / 2;
    arrowPosR.y = space::game::sideTileCount + 1;
    Coord arrowPosL = arrowPosR;
    arrowPosL.x -= 1;

    bool b_lArrow = false;
    bool b_rArrow = false;

    if (state > 0)
    {
        b_lArrow = true;
        g_frameBuffer.DrawSymbolSkippingBuffer(arrowPosL, '<', 0);
    }
    if (state < this->turn - 1)
    {
        b_rArrow = true;
        g_frameBuffer.DrawSymbolSkippingBuffer(arrowPosR, '>', 0);
    }

    InitInput();

    Coord mouseCoordLast = { 0,0 };
    Coord mouseCoordCurrent = { 0,0 };

    while (true)
    {
        PingInput(); // Update the mouse location

        mouseCoordCurrent = g_mouse.ReadMouseHover();

        if (mouseCoordLast != mouseCoordCurrent) // If the mouse location has changed
        {
            // Draw transparent image of our piece at the move we are hovering over
            if (b_lArrow)
            {
                if (mouseCoordCurrent == arrowPosL) g_frameBuffer.DrawSymbolSkippingBuffer(arrowPosL, '<', 1);
                if (mouseCoordLast == arrowPosL) g_frameBuffer.DrawSymbolSkippingBuffer(arrowPosL, '<', 0);
            }
            if (b_rArrow)
            {
                if (mouseCoordCurrent == arrowPosR) g_frameBuffer.DrawSymbolSkippingBuffer(arrowPosR, '>', 1);
                if (mouseCoordLast == arrowPosR) g_frameBuffer.DrawSymbolSkippingBuffer(arrowPosR, '>', 0);
            }
            mouseCoordLast = mouseCoordCurrent; // Update the temporary mouse position variable
        }

        if (g_mouse.CheckMouseState()) break; // Click is true
    }
    // Click on button confirmed
    Coord whereClicked = g_mouse.GetMouseClickCoord();
    if (whereClicked == arrowPosL) return -1;
    else if (whereClicked == arrowPosR) return 1;
    else return 0;
}

void Board::GameFlipbook()
{
    int state = 0;
    while (true)
    {
        DrawBoardState(state);

        int update = 0;
        
        while (update == 0 && ((state + update) >= 0) && ((state + update) <= turn)) {
            update = FlipbookWFClick(state);
        }
        state += update;
    }
}
