#include <vector>

class Unit;
class Location;

#pragma once
class Board
{
public:
    Unit* GetUnit(Location location);


    void ResetBoard(int width, int height);
    void PlayBoard();
    void PrintBoard();


    void AddUnit(Unit* unit);
    void RemoveUnit(Unit* unit);
private:
    int width = 0;
    int height = 0;

    std::vector<Unit*> whiteUnits;
    std::vector<Unit*> blackUnits;
};

