#include "Unit.h"

enum class UnitColor : bool;

using namespace space;
using namespace game;

Coord SetOf8thTurns(int i)
{
	switch (i % 8) {
	case 0:
		// North
		return { 0,1 };
		break;
	case 1:
		// Northeast
		return { 1, 1 };
		break;
	case 2:
		// East
		return { 1,0 };
		break;
	case 3:
		// Southeast
		return { 1,-1 };
		break;
	case 4:
		// South
		return { 0,-1 };
		break;
	case 5:
		// Southwest
		return { -1,-1 };
		break;
	case 6:
		// West
		return { -1,0 };
		break;
	case 7:
		// Northwest
		return { -1,1 };
		break;
	}
}

bool Unit::ValidPos(const Coord testPos)
{
	if (((testPos.x >= 0) && (testPos.x < sideTileCount)) && ((testPos.y >= 0) && (testPos.y < sideTileCount))) return true;
	else return false;
}

bool Unit::SpaceHasNoTeammate(const Coord testPos)
{
	const Unit* unitAtPos = m_boardIAmOf->GetUnitAtPos(testPos);

	// Empty space has no teammate
	if (unitAtPos == nullptr) return true;

	// Space with enemy has no teammate
	else return (unitAtPos->GetColor() == GetEnemyColor());
}

bool Unit::PieceIsBlocking(Coord testPos, Coord* confirmedMoves, unsigned char& confirmedMoveCount)
{
	const Unit* testUnit = m_boardIAmOf->GetUnitAtPos(testPos);

	bool thereIsUnit = testUnit != nullptr;

	bool unitIsTeammate = false;

	if (thereIsUnit) unitIsTeammate = (testUnit->GetColor() == GetColor());

	if (!unitIsTeammate) {
		confirmedMoves[confirmedMoveCount++] = testPos;
	} return thereIsUnit;
}

void Unit::LineTrace(Coord* confirmedMoves, unsigned char& confirmedMoveCount, const Coord direction)
{
	for (Coord testPos = GetLocation() + direction; ValidPos(testPos); testPos = (testPos + direction))
	{
		if (PieceIsBlocking(testPos, confirmedMoves, confirmedMoveCount)) break;
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

bool PieceMoves::MoveIsValid(Coord move)
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

Piece Unit::GetPieceType()
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

//
// UNITS
//

// Pawn

Piece Pawn::GetPieceType()
{
	return Piece::Piece_Pawn;
}

void Pawn::Move(Coord newPosition)
{
	m_moved = true;
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

	possibleMoves[0] = {  0, 1 };
	possibleMoves[1] = { -1, 1 }; // Only if capturing
	possibleMoves[2] = {  1, 1 }; // Only if capturing
	possibleMoves[3] = {  0, 2 }; // First turn only

	// Pawn movement directions are team-dependent
	if (GetColor() == UnitColor::Unit_White) {
		for (Coord& move : possibleMoves) {
			move.y *= -1;
		}
	}

	Coord confirmedMoves[4];

	unsigned char confirmedMoveCount = 0;

	// Convert the relative-space template positions into absolute space
	for (unsigned char i = 0; i < 4; ++i)
	{
		possibleMoves[i] = possibleMoves[i] + GetLocation();
	}

	// Can move forward one?
	if (possibleMoves[0].y < sideTileCount && // Verify that the position is valid
		possibleMoves[0].y >= 0 && // Verify that the position is valid
		!m_boardIAmOf->IsUnitAtPos(possibleMoves[0])) // There is no unit there
	{
		confirmedMoves[confirmedMoveCount] = possibleMoves[0];
		++confirmedMoveCount;
	}

	// Can move diagonally?
	for (char i = 1; i < 3; ++i)
	{
		Unit* potentialEnemy = m_boardIAmOf->GetUnitAtPos((possibleMoves[i]));

		if (potentialEnemy != nullptr && // There is a unit at that position and
			potentialEnemy->GetColor() == GetEnemyColor()) // They are an enemy
		{
			// Note that we don't have to check if the position is valid, as no enemy will be at an invalid position.
			confirmedMoves[confirmedMoveCount] = possibleMoves[i];
			++confirmedMoveCount;
		}
	}

	// Can move forward two?
	if (m_moved == false && // First turn
		!m_boardIAmOf->IsUnitAtPos(possibleMoves[3])) // No unit there
	{ 
		// No pawn will ever be starting at a position 2 moves behind an out-of-bounds position, so we don't have to validate the position.
		confirmedMoves[confirmedMoveCount] = possibleMoves[3];
		++confirmedMoveCount;
	}

	moves->SetMoves(confirmedMoves, confirmedMoveCount);
}

// Rook

Piece Rook::GetPieceType()
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
	for (int direction = 0; direction < 7; direction = direction + 2)
	{
		LineTrace(confirmedMoves, confirmedMoveCount, SetOf8thTurns(direction));
	}

	moves->SetMoves(confirmedMoves, confirmedMoveCount);
}

// Bishop

Piece Bishop::GetPieceType()
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
	for (int direction = 1; direction < 8; direction = direction + 2)
	{
		LineTrace(confirmedMoves, confirmedMoveCount, SetOf8thTurns(direction));
	}

	moves->SetMoves(confirmedMoves, confirmedMoveCount);
}

// Knight

Piece Knight::GetPieceType()
{
	return Piece::Piece_Knight;
}

sprite::Sprite* Knight::GetSpritePointer()
{
	return &sprite::unit::knight;
}

void Knight::AvailableMoves(PieceMoves* moves)
{
	// Template moves
	Coord possibleMoves[8] = {
		{ -1,-2 }, { 1,-2 },
		{ 2,-1 }, { 2,1 },
		{ 1,2 }, { -1,2 },
		{ -2,1 }, { -2,-1 } };

	Coord confirmedMoves[8];

	unsigned char confirmedMoveCount = 0;

	// Relative space
	for (unsigned char i = 0; i < 8; ++i)
	{

		Coord testPos = possibleMoves[i] + GetLocation(); // testPos is the offset location

		if (testPos.x >= 0 && testPos.x < sideTileCount && // On the board
			testPos.y >= 0 && testPos.y < sideTileCount) // On the board
		{
			if (!m_boardIAmOf->IsUnitAtPos(testPos) || // Either there is no unit at the position or
				m_boardIAmOf->GetUnitAtPos(testPos)->GetColor() == GetEnemyColor()) // The unit at the position is an enemy
			{
				confirmedMoves[confirmedMoveCount] = testPos;

				++confirmedMoveCount;
			}
		}
	}
	
	moves->SetMoves(confirmedMoves, confirmedMoveCount);
}

// Queen

Piece Queen::GetPieceType()
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
		LineTrace(confirmedMoves, confirmedMoveCount, SetOf8thTurns(direction));
	}

	moves->SetMoves(confirmedMoves, confirmedMoveCount);
}

// King

Piece King::GetPieceType()
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

	if (unit != nullptr && unit->GetColor() != GetColor()) return false; // There is an enemy

	else return true; // Empty / teammate
}

Unit* King::LineTraceSafety(Coord startPos, Coord direction)
{
	Unit* unit = nullptr;

	for (Coord testPos = startPos + direction; ValidPos(testPos); testPos = (testPos + direction))
	{
		m_boardIAmOf->DrawBoardSpaceColored(testPos, RGB(0,255,0)); // Debug

		unit = TestMoveSafety(testPos);

		if (unit != nullptr) // Space is empty
		{
			if (unit->GetColor() == GetColor()) // The unit is a teammate
			{
				//m_boardIAmOf->DrawBoardSpaceColored(testPos, RGB(0, 0, 255)); // Debug
				// Stop and return "false"
				break;
			}
			else // The unit is an enemy
			{
				// Stop and return the enemy
				//m_boardIAmOf->DrawBoardSpaceColored(testPos, RGB(255, 0, 0)); // Debug
				return unit;
				break;
			}
		}
	}
	// Stop. We have left the board.
	return nullptr;
}

bool King::CanIBeTaken(const Coord position, const Coord direction)
{
	Unit* potentialEnemy = LineTraceSafety(position, direction);

	/*
	* ~ASSUMPTIONS BEING MADE IN THIS FUNCTION BY CURRENT USAGE:~
	* 
	* 1. Any unit being taken as an input will be an enemy. Teammates return nullptr through the "LineTraceSafety()" function, 
	* they do not need to be tested for team.
	* // NOTE: ASSUMPTION 1 IS NO LONGER VALID! THE CODE HAS BEEN MODIFIED TO MATCH. //
	* 
	* 2. There is no piece blocking the way between our king and this unit,
	* so any input of "nullptr" means we reached the end of the board in this direction without being blocked by any enemy units.
	*
	* 3. The enemy will always be on a straight/diagonal-line path,
	* therefore any piece which returns false to the test "x == x || y == y" is on a path which is certain to be diagonal.
	* 
	* 4. Knights cannot, under any circumstance (by the rules of this version of chess), ever take a piece which is in a straight/diagonal line.
	* Knights will always return false out of this function.
	*/

	//m_boardIAmOf->DrawBoardSpaceColored(position, RGB(0,255,0)); // Debugging

	if (potentialEnemy != nullptr && potentialEnemy->GetColor() != GetColor()) // Does the enemy exist? An empty space cannot take us.
	{
		Piece enemyClass = potentialEnemy->GetPieceType();

		Coord enemyLocation = potentialEnemy->GetLocation();

		//m_boardIAmOf->DrawBoardSpaceColored(enemyLocation, RGB(255, 0, 0)); // Debugging

		switch (enemyClass)
		{
		case Piece::Piece_Pawn: // Only forward-diagonal, color matters here.

			switch (GetEnemyColor())
			{
			case UnitColor::Unit_Black:
				if (enemyLocation == (position + Coord{ 1, 1 }) || enemyLocation == (position + Coord{ -1, 1 })) return true;
				else return false;
				break;

			case UnitColor::Unit_White:
				if (enemyLocation == (position + Coord{ 1, -1 }) || enemyLocation == (position + Coord{ -1, -1 })) return true;
				else return false;
				break;

			default: // This is just so the compiler will be happy
				return false;
				break;
			}
			break;

		case Piece::Piece_Rook:
			return (
				(position.x == enemyLocation.x) ||
				(position.y == enemyLocation.y) );
				break;

		case Piece::Piece_Knight: // See assumption #4
			return false; 
			break;

		case Piece::Piece_Bishop: // See assumption #3
			return (
				(position.x != enemyLocation.x) &&
				(position.y != enemyLocation.y) );
			break;

		case Piece::Piece_Queen: // Any queen found via line trace, by assumptions #1-3, is certain to be putting us in check.
			return true;
			break;

		case Piece::Piece_King: // @TODO
			return false;
			break;

		default: // Once again, just appeasing the compiler.
			return false;
			break;
		}
	}
	else return false;// There was no piece/they were a teammate
}

bool King::CheckSafetyDirectional(Coord position)
{
	for (int i = 0; i < 8; ++i) {
		m_boardIAmOf->DrawBoardSpaceColored(position, RGB(0, 0, 255)); // Debug
		if (CanIBeTaken(position, SetOf8thTurns(i))) return false; // If i can be taken, return false.
		m_boardIAmOf->DrawBoardSpaceColored(position, RGB(0, 0, 255)); // Debug
		m_boardIAmOf->PrintBoard(); // Debug
	}
	return true;
}

bool King::CheckSafetyRing(Coord position)
{
	if (TestMoveSafetyValidate(position + Coord{  1, -2 }) &&
		TestMoveSafetyValidate(position + Coord{  2, -1 }) &&
		TestMoveSafetyValidate(position + Coord{  2,  1 }) &&
		TestMoveSafetyValidate(position + Coord{  1,  2 }) &&
		TestMoveSafetyValidate(position + Coord{ -1,  2 }) &&
		TestMoveSafetyValidate(position + Coord{ -2,  1 }) &&
		TestMoveSafetyValidate(position + Coord{ -2, -1 }) &&
		TestMoveSafetyValidate(position + Coord{ -1, -2 })) {
		return true;
	}
	else return false;
}

bool King::SpaceIsSafeFromCheck(Coord ifIWasHere)
{
	Coord fromPerspective = ifIWasHere;

	// Must be converted to a Coord to use == on position
	if (ifIWasHere == Coord{ -1,-1 }) fromPerspective = GetLocation(); // If the input is still the default value, set it to our current position (test if we are in check)
		
	if (CheckSafetyDirectional(fromPerspective) && CheckSafetyRing(fromPerspective)) return true; // Safe
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
		const Coord testPos = GetLocation() + SetOf8thTurns(i); // testPos is the offset location

		if (ValidPos(testPos) && // Check that the position is on the board
			SpaceHasNoTeammate(testPos) && // The unit at the position, be there one at all, is not a teammate
			SpaceIsSafeFromCheck(testPos)) // Would that space be safe from putting me in check?
		{
			confirmedMoves[confirmedMoveCount++] = testPos; // Append the current array of confirmed moves with the move we are currently checking
		}
	}

	moves->SetMoves(confirmedMoves, confirmedMoveCount); // Write to the output
}
