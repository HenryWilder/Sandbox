#include "Unit.h"

enum class UnitColor : bool;

using namespace space;
using namespace game;

Coord CardinalDir(int i)
{
	/* Array of cardinal directions
	* [ ][0][ ]
	* [3]   [1]
	* [ ][2][ ]*/
	Coord cardinals[] = { { 0, 1 },{ 1, 0 },{ 0, -1 },{ -1, 0 } };
	return cardinals[i % 4]; // Pick the index from the array
}
Coord DiagonalDir(int i)
{
	/* Array of diagonal directions
	*[3][ ][0]
	*[ ]   [ ]
	*[2][ ][1]*/
	Coord diagonals[] = { { 1, 1 },{ 1, -1 },{ -1, -1 },{ -1, 1 } };
	return diagonals[i % 4]; // Pick the index from the array
}
Coord AllDir(int i)
{
	/* Array of all directions
	*[7][0][1]
	*[6]   [2]
	*[5][4][3]*/
	if (Even(i)) return CardinalDir(i / 2);	// Return cardinal on even input
	else return DiagonalDir((i - 1) / 2);	// Return diagonal on odd input
}
Coord KnightDir(int i)
{
	/* Array of all knight directions
	*  ==============================
	* The knight's movement array is created from a diagonal + a cardinal offset.
	*     [c0]    [c0]
	* [c3](d3)    (d0)[c1]
	*         init
	* [c3](d2)    (d1)[c1]
	*     [c2]    [c2]
	* 
	* Our index of that array will be in this order:
	*    [7]   [0]
	* [6]         [1]
	*       pos
	* [5]         [2]
	*    [4]   [3]
	* 
	* In other words:
	* Knight(0) = Diagonal(0) + Cardinal(0)
	* Knight(1) = Diagonal(0) + Cardinal(1)
	* Knight(2) = Diagonal(1) + Cardinal(1)
	* Knight(3) = Diagonal(1) + Cardinal(2)
	* Knight(4) = Diagonal(2) + Cardinal(2)
	* Knight(5) = Diagonal(2) + Cardinal(3)
	* Knight(6) = Diagonal(3) + Cardinal(3)
	* Knight(7) = Diagonal(3) + Cardinal(0)
	* 
	* Notice how the pattern for i in Cardinal(i) is the same as Diagonal(i + 1)
	* So the pattern for i in Diagonal can be thought of as floor(i / 2), while for Cardinal it would be ceil(i / 2)
	* 0: (floor(0 / 2) = 0)        + (ceil(0 / 2) = 0)       
	* 1: (floor(1 / 2) = 0.5 -> 0) + (ceil(1 / 2) = 0.5 -> 1)
	* 2: (floor(2 / 2) = 1)        + (ceil(2 / 2) = 1)       
	* 3: (floor(3 / 2) = 1.5 -> 1) + (ceil(3 / 2) = 1.5 -> 2)
	* 4: (floor(4 / 2) = 2)        + (ceil(4 / 2) = 2)       
	* 5: (floor(5 / 2) = 2.5 -> 2) + (ceil(5 / 2) = 2.5 -> 3)
	* 6: (floor(6 / 2) = 3)        + (ceil(6 / 2) = 3)       
	* 7: (floor(7 / 2) = 3.5 -> 3) + (ceil(7 / 2) = 3.5 -> 0) */

	return DiagonalDir(i / 2) + CardinalDir((i + 1) / 2);
}

bool Unit::ValidPos(const Coord testPos)
{
	if (((testPos.x >= 0) && (testPos.x < sideTileCount)) && ((testPos.y >= 0) && (testPos.y < sideTileCount))) return true;
	else return false;
}

bool Unit::NullOrEnemy(const Unit* unit)
{
	if (unit == nullptr) return true;
	else return (unit->GetColor() != GetColor());
}

bool Unit::UnitIsEnemy(const Unit* unit)
{
	return (unit != nullptr && unit->GetColor() != GetColor());
}

// Used for testing the aliance of a unit at a space we have the coordinates of, but haven't yet searched for a unit in.
bool Unit::SpaceHasNoTeammate(const Coord testPos)
{
	const Unit* unitAtPos = m_boardIAmOf->GetUnitAtPos(testPos);

	return NullOrEnemy(unitAtPos);
}

bool Unit::PieceIsBlocking(Coord testPos, Coord* confirmedMoves, unsigned char& confirmedMoveCount)
{
	const Unit* testUnit = m_boardIAmOf->GetUnitAtPos(testPos);

	bool thereIsUnit = testUnit != nullptr;

	bool unitIsTeammate = false;

	if (thereIsUnit) unitIsTeammate = (testUnit->GetColor() == GetColor());

	if (!unitIsTeammate) {
		confirmedMoves[confirmedMoveCount++] = testPos; // The unit is an enemy
	} return thereIsUnit;
}

void Unit::LineTrace(Coord* confirmedMoves, unsigned char& confirmedMoveCount, const Coord direction)
{
	for (Coord testPos = GetLocation() + direction; ValidPos(testPos); testPos = (testPos + direction))
	{
		if (PieceIsBlocking(testPos, confirmedMoves, confirmedMoveCount)) break; // If there is a unit at all, break. Append blocking unit if they are an enemy.
	}
}

void PieceMoves::SetMoves(const Coord* moves, const unsigned char count)
{
	m_availableMovesCount = count;

	for (unsigned char i = 0; i < m_availableMovesCount; ++i)
	{
		m_available[i] = *(moves + i); // The next index of the moves array
	}
}

int PieceMoves::GetMoves(Coord* moves)
{
	moves = m_available;
	return (int)m_availableMovesCount;
}

bool PieceMoves::MoveIsValid(Coord move) const
{
	bool valid = false;

	for (unsigned int i = 0; i < m_availableMovesCount; ++i)
	{
		if (move == m_available[i])
		{
			valid = true;
			break;
		}
	}

	return valid;
}

void Unit::Init(Coord pos, UnitColor color, Board* boardSpawningMe, unsigned char id)
{
	m_position = pos;
	m_team = color;
	m_boardIAmOf = boardSpawningMe;
	b_hidden = false;
	m_ID = id;
}

Coord Unit::GetLocation() const
{
	return m_position;
}

UnitColor Unit::GetColor() const
{
	return m_team;
}

UnitColor Unit::GetEnemyColor() const
{
	return (m_team == UnitColor::Unit_Black ? UnitColor::Unit_White : UnitColor::Unit_Black);
}

void Unit::Move(Coord newPosition)
{
	m_position = newPosition;
}

sprite::Sprite* Unit::GetSpritePointer() {	return &sprite::unit::null;	}

void Unit::AvailableMoves(PieceMoves* moves) {}

Piece Unit::GetPieceType() const
{
	return Piece::Piece_Pawn;
}

bool Unit::IsHidden()
{
	return b_hidden;
}

void Unit::Hide()
{
	b_hidden = true;
}

void Unit::UnHide()
{
	b_hidden = false;
}

unsigned char Unit::GetID()
{
	return m_ID;
}

bool Unit::CouldITakeAt(Coord hypothetical)
{
	return false;
}

//
// UNITS
//

// Pawn

signed int Pawn::GetForward() const
{
	if (GetColor() == UnitColor::Unit_Black) return 1;
	else return -1;
}

Piece Pawn::GetPieceType() const
{
	return Piece::Piece_Pawn;
}

void Pawn::Move(Coord newPosition)
{
	m_moved = true;
	if (newPosition.y == GetLocation().y + 2 || newPosition.y == GetLocation().y - 2) en_pasant = true;
	Unit::Move(newPosition);
}

sprite::Sprite* Pawn::GetSpritePointer()
{
	return &sprite::unit::pawn;
}

void Pawn::AvailableMoves(PieceMoves* moves)
{
	// Template moves
	Coord possibleMoves[4];

	// Forward from the perspective of our pawn
	const int fw = GetForward();
	Coord p = GetLocation(); // Position

	possibleMoves[0] = { p.x,     p.y + fw };
	possibleMoves[1] = { p.x,     p.y + fw + fw }; // First turn only
	possibleMoves[2] = { p.x - 1, p.y + fw }; // Only if capturing
	possibleMoves[3] = { p.x + 1, p.y + fw }; // Only if capturing
	

	Coord confirmedMoves[6];

	unsigned char confirmedMoveCount = 0;

	// Can move forward one?
	if (ValidPos(possibleMoves[0]) && !m_boardIAmOf->IsUnitAtPos(possibleMoves[0])) confirmedMoves[confirmedMoveCount++] = possibleMoves[0];

	// Can move forward two?
	// No pawn will ever be starting at a position 2 moves behind an out-of-bounds position, so we don't have to validate the position.
	if (m_moved == false &&
		!m_boardIAmOf->IsUnitAtPos(possibleMoves[0]) && // Really weird edge-case
		!m_boardIAmOf->IsUnitAtPos(possibleMoves[1]))
	{
		confirmedMoves[confirmedMoveCount++] = possibleMoves[1];
	}

	// Can move diagonally?
	for (char i = 2; i < 4; ++i)
	{
		Unit* pEnemy = m_boardIAmOf->GetUnitAtPos((possibleMoves[i]));

		// Note that we don't have to check if the position is valid, as no enemy will be at an invalid position.
		if (UnitIsEnemy(pEnemy)) confirmedMoves[confirmedMoveCount++] = possibleMoves[i];

		//
		// En'passant
		//
		Coord passantPos = { possibleMoves[i].x, possibleMoves[i].y - fw };
		Pawn* passantPawn = dynamic_cast<Pawn*>(m_boardIAmOf->GetUnitAtPos(passantPos));

		if (UnitIsEnemy(passantPawn) && passantPawn->en_pasant) confirmedMoves[confirmedMoveCount++] = possibleMoves[i];
	}

	

	moves->SetMoves(confirmedMoves, confirmedMoveCount);
}

bool Pawn::CouldITakeAt(Coord hypothetical)
{
	int fwDirection = 1; // Default assumes black unit (because black is first in the enum definition (personal preference, does not affect code.))

	if (GetColor() == UnitColor::Unit_White) fwDirection = -1; // If the unit is found to be white, invert the forward direction.

	if (hypothetical == (m_position + Coord{ 1 /*right*/, fwDirection }) ||
		hypothetical == (m_position + Coord{ -1 /*left*/, fwDirection })) return true;
	else return false;
}

// Rook

Piece Rook::GetPieceType() const
{
	return Piece::Piece_Rook;
}

void Rook::Move(Coord newPosition)
{
	m_moved = true;
	Unit::Move(newPosition);
}

sprite::Sprite* Rook::GetSpritePointer()
{
	return &sprite::unit::rook;
}

void Rook::AvailableMoves(PieceMoves* moves)
{
	Coord confirmedMoves[sideTileCount * 2];

	unsigned char confirmedMoveCount = 0;

	Coord testPos;

	// Straight lines
	for (int direction = 0; direction < 4; ++direction)
	{
		LineTrace(confirmedMoves, confirmedMoveCount, CardinalDir(direction));
	}

	moves->SetMoves(confirmedMoves, confirmedMoveCount);
}

// We are assuming that a straight line of sight has already been established for this to be called.
bool Rook::CouldITakeAt(Coord hypothetical)
{
	if (hypothetical.x == m_position.x ||
		hypothetical.y == m_position.y) return true;
	else return false;
}

// Bishop

Piece Bishop::GetPieceType() const
{
	return Piece::Piece_Bishop;
}

sprite::Sprite* Bishop::GetSpritePointer()
{
	return &sprite::unit::bishop;
}

void Bishop::AvailableMoves(PieceMoves* moves)
{
	Coord confirmedMoves[sideTileCount * 3];

	unsigned char confirmedMoveCount = 0;

	Coord testPos;

	// Diagonals
	for (int direction = 0; direction < 4; ++direction)
	{
		LineTrace(confirmedMoves, confirmedMoveCount, DiagonalDir(direction));
	}

	moves->SetMoves(confirmedMoves, confirmedMoveCount);
}

// We are assuming that a straight line of sight has already been established for this to be called.
bool Bishop::CouldITakeAt(Coord hypothetical)
{
	if (hypothetical.x != m_position.x &&
		hypothetical.y != m_position.y) return true;
	else return false;
}

// Knight

Piece Knight::GetPieceType() const
{
	return Piece::Piece_Knight;
}

sprite::Sprite* Knight::GetSpritePointer()
{
	return &sprite::unit::knight;
}

void Knight::AvailableMoves(PieceMoves* moves)
{
	Coord confirmedMoves[8];

	unsigned char confirmedMoveCount = 0;

	// Relative space
	for (unsigned char i = 0; i < 8; ++i)
	{
		const Coord testPos = GetLocation() + KnightDir(i); // testPos is the offset location

		if (ValidPos(testPos)) // On the board
		{
			const Unit* testUnit = m_boardIAmOf->GetUnitAtPos(testPos);
			if (NullOrEnemy(testUnit)) confirmedMoves[confirmedMoveCount++] = testPos;
		}
	}
	
	moves->SetMoves(confirmedMoves, confirmedMoveCount);
}

bool Knight::CouldITakeAt(Coord hypothetical)
{
	// Find the hypothetical position in the list of possible positions
	for (int i = 0; i < 8; ++i)
	{
		Coord testPos = GetLocation() + KnightDir(i);
		if (hypothetical == testPos) return true;
	}
	return false;
}

// Queen

Piece Queen::GetPieceType() const
{
	return Piece::Piece_Queen;
}

sprite::Sprite* Queen::GetSpritePointer()
{
	return &sprite::unit::queen;
}

void Queen::AvailableMoves(PieceMoves* moves)
{
	Coord confirmedMoves[sideTileCount * 4];

	unsigned char confirmedMoveCount = 0;

	Coord testPos;

	// All directions
	for (int direction = 0; direction < 8; ++direction)
	{
		LineTrace(confirmedMoves, confirmedMoveCount, AllDir(direction));
	}

	moves->SetMoves(confirmedMoves, confirmedMoveCount);
}

// We are assuming that a straight line of sight has already been established for this to be called.
bool Queen::CouldITakeAt(Coord hypothetical)
{
	return true; // In the case of the queen, the answer is yes.
}

// King

Piece King::GetPieceType() const
{
	return Piece::Piece_King;
}

void King::Move(Coord newPosition)
{
	m_moved = true;
	Unit::Move(newPosition);
}

sprite::Sprite* King::GetSpritePointer()
{
	return &sprite::unit::king;
}

Unit* King::TestMoveSafety(Coord testPos)
{
	Unit* unit = m_boardIAmOf->GetUnitAtPos(testPos);

	if (unit != nullptr) return unit;
	return nullptr; // Nobody is there
}

bool King::TestMoveSafetyValidate(Coord position)
{
	Unit* unit = TestMoveSafety(position);
	return (UnitIsEnemy(unit));
}

Unit* King::LineTraceSafety(Coord startPos, Coord direction)
{
	Unit* unit = nullptr;

	for (Coord testPos = startPos + direction; ValidPos(testPos); testPos = (testPos + direction))
	{
		unit = TestMoveSafety(testPos);

		if (unit != nullptr /*Space is not empty*/ && unit != this /*Ignore self*/)
		{
			if (unit->GetColor() == GetColor()) break; // The unit is a teammate, we cannot take them but they block our movement.
			else return unit; // The unit is an enemy
		}
	}
	// Stop. We have left the board.
	return nullptr;
}

bool King::CanIBeTaken(const Coord position, const Coord direction)
{
	Unit* potentialEnemy = LineTraceSafety(position, direction);

	// Does the enemy exist? An empty space cannot take us.
	if (potentialEnemy != nullptr && potentialEnemy->GetColor() != GetColor()) return potentialEnemy->CouldITakeAt(position);
	else return false; // There was no piece/they were a teammate
}

bool King::CheckSafetyDirectional(Coord position)
{
	for (int i = 0; i < 8; ++i) {
		if (CanIBeTaken(position, AllDir(i))) return false; // If i can be taken, return false.
	}
	return true; // Nobody was found who could take us
}

bool King::CheckSafeAgainstKnight(Coord position)
{
	Unit* unit;

	for (int i = 0; i < 8; ++i)
	{
		const Coord testPos = position + KnightDir(i);

		unit = m_boardIAmOf->GetUnitAtPos(testPos);

		//m_boardIAmOf->DrawBoardSpaceColored(testPos, RGB(127, 127, 127)); // Debug

		if (UnitIsEnemy(unit) &&
			unit->GetPieceType() == Piece::Piece_Knight)
		{
			//m_boardIAmOf->DrawBoardSpaceColored(testPos, RGB(255, 127, 127)); // Debug
			return  false;
		}
	}
	return true;
}

bool King::SpaceIsSafeFromCheck(Coord ifIWasHere)
{
	Coord fromPerspective = ifIWasHere;

	if (ifIWasHere == Coord{ -1,-1 }/*The default vaule*/) fromPerspective = GetLocation(); // If the input is still the default value, set it to our current position (test if we are in check)
		
	if (CheckSafetyDirectional(fromPerspective) && CheckSafeAgainstKnight(fromPerspective)) return true; // Safe
	else return false; // Not safe to move to this space
}

void King::AvailableMoves(PieceMoves* moves)
{
	/*The array of moves we know are accurate. Starts empty, so that we can append new moves when we confirm them.
	* In this way, the confirmed moves are at the beginning of the array, and invalid moves are not in the list,
	* making it so that no fancy "add this move but then skip this one" is needed when writing to the output moveset. */
	Coord confirmedMoves[8];

	/*The total number of moves at a given instant = the index of the current last move + 1 (or the index of a brand new move).
	* This is how we fake a dynamic array in an array of constant size. */
	unsigned char confirmedMoveCount = 0;

	for (unsigned char i = 0; i < 8; ++i)
	{
		const Coord testPos = GetLocation() + AllDir(i); // testPos is the offset location

		if (ValidPos(testPos) && // Check that the position is on the board
			SpaceHasNoTeammate(testPos) && // The unit at the position, be there one at all, is not a teammate
			SpaceIsSafeFromCheck(testPos)) // Would that space be safe from putting me in check?
		{
			confirmedMoves[confirmedMoveCount++] = testPos; // Append the current array of confirmed moves with the move we are currently checking
		}
	}

	moves->SetMoves(confirmedMoves, confirmedMoveCount); // Write to the output
}

void King::UnAvailableMoves(PieceMoves* checks)
{
	Coord confirmedMoves[8];

	unsigned char confirmedMoveCount = 0;

	for (unsigned char i = 0; i < 8; ++i)
	{
		const Coord testPos = GetLocation() + AllDir(i); // testPos is the offset location

		if (ValidPos(testPos) && // Check that the position is on the board
			SpaceHasNoTeammate(testPos) && // The unit at the position, be there one at all, is not a teammate
			!SpaceIsSafeFromCheck(testPos)) // Would that space be safe from putting me in check?
		{
			confirmedMoves[confirmedMoveCount++] = testPos; // Append the current array of confirmed moves with the move we are currently checking
		}
	}

	checks->SetMoves(confirmedMoves, confirmedMoveCount); // Write to the output
}

bool King::CouldITakeAt(Coord hypothetical)
{
	return false; // @TODO
}
