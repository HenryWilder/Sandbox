#pragma once
#include "Universal.h"
#include <iostream>
#include <vector>
#include "Unit.h"
#include "Rendering.h"
#include "Input.h"

struct Unit;
enum class Piece;
enum class UnitColor : bool;

char NumToLetter(int value);

int LetterToNum(char letter);

char CharToValue(char character);

void ClearTextLine(int start = 0, int count = 1);

class Board
{
private:
	int width = 0;
	int height = 0;
	int turn = 0;
	std::vector<Unit*> whiteUnits;
	std::vector<Unit*> blackUnits;

	const std::vector<Unit*>* GetTeamArrayReadOnly(UnitColor team);
	std::vector<Unit*>* GetTeamArray(UnitColor team);

	void BuildRoyalty(int y, UnitColor col, unsigned char& unitID);
	void ConstructNewUnit(Coord pos, Piece type, UnitColor color, unsigned char& unitID);

public:
	Unit* GetTeamUnitAtPos(Coord pos, UnitColor team);
	Unit* GetUnitAtPos(Coord pos);
	bool IsTeamAtPos(Coord pos, UnitColor team);
	bool IsUnitAtPos(Coord pos);

	void ResetBoard(int _width, int _height = 0);

	void DrawBoardSpaceColored(Coord pos, COLORREF color); // Draws the space on the board with the input color and the piece at that space
	void DrawBoardSpaceColored(Coord pos, Color color); // Draws the space on the board with the input color and the piece at that space
	void DrawBoardSpaceReset(Coord pos);

private: // Helper functions
	Coord WaitForClick();
	Coord TakePosInput();
	void MovePiece(Unit* unit, Coord moveTo);

public:
	void PlayBoard();
	void PrintBoard();

	void AddUnit(Unit* unit);
	void RemoveUnit(Unit* unit);
	void IncrementTurn();
};