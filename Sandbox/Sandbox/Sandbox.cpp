#include <iostream>
#include <string>
#include <vector>
#define C_RED  "\033[31m"
#define C_BLUE "\033[34m"
#define C_DEFAULT "\033[0m"

void CheckForWin(int* board,int playerValue)
{
	int winState[][3]
	{
		// Horizontal
		{0,1,2},
		{3,4,5},
		{6,7,8},

		// Vertical
		{0,3,6},
		{1,4,7},
		{2,5,8},

		// diagonal
		{0,4,8},
		{6,4,2}
	};

	for (int i = 0; i < 8; ++i) {
		if (board[winState[1][0]] == playerValue &&
			board[winState[1][1]] == playerValue &&
			board[winState[1][2]] == playerValue)
		{
			std::cout << (playerValue == 1 ? "X" : "O") << " has won!\n"; // X wins
		}
	}
}
int main()
{


	int main()
	{
		printf(C_RED"This is red\n");
		printf(C_BLUE"This is blue\n");
		printf(C_DEFAULT"This is default\n");

		return 0;
	}
}