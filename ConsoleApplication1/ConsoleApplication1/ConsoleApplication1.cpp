// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string> 
#include <vector>
#include <windows.h>

#define COLOR_BLACK "\x1b[30;1m"
#define COLOR_RED "\x1b[31;1m"
#define COLOR_BLUE "\x1b[34;1m"
#define COLOR_RESET "\x1b[37;0m"

bool b_inPlay[9] {};
bool b_ownedByX[9] {};
int turn = 0;

std::string spaceOwnership(int pos)
{
	std::string owner;
	switch ((b_inPlay[pos] == true) * (1 + (b_ownedByX[pos] == true)))
	{
		case 0:
			owner.append(std::to_string(pos));
			break;
		case 1:
			owner.append("O");
			break;
		case 2:
			owner.append("X");
			break;
	}
	return owner;
}

int boardElement(int n, int i)
{
	switch (n)
	{
		case 0:
			std::cout << " ";
			break;
		case 6:
			std::cout << '\n';
			break;
		case 7:
			std::cout << "---+---+---\n";
			break;
	}
	if (n == 2 || n == 4)
		std::cout << " | ";
	if (n == 1 || n == 3 || n == 5) {
		if (spaceOwnership(i) == "X" || spaceOwnership(i) == "O") {
			std::cout << ((spaceOwnership(i) == "O") ? COLOR_BLUE : COLOR_RED) << spaceOwnership(i) << COLOR_RESET;
		}
		else {
			std::cout << COLOR_BLACK << spaceOwnership(i) << COLOR_RESET;
		}
		return 1;
	}
	else { return 0; }
}

void drawBoard()
{
	int index = 0; // Only elements which are coordinates will change this value.
	for(int element = 0; element<=22; element++) // There are a total of 23 elements. I counted.
	{
		index = index + boardElement(element % 8, index); // index is the coordinate on the board while element includes the board parts themselves.
	}
}

bool even(int n) { return ((2 * (n / 2)) == n); } // Because the values are integers, division by two will be truncated and return a value different (usually the original value minus 1) from the original value when multiplied back.

std::string whoseTurn()
{
	return even(turn + 1) ? "O" : "X";
}

bool checkWin()
{
	bool OwnedByPlayer[3][3] {}; // This array changes every turn depending on whose turn it is. That's why we can't initialize it outside of this function and just save it for later.
	for (int i = 0; i < 9; i++) {
		if (b_inPlay[i] == true) {
			if (b_ownedByX[i] == (whoseTurn() == "X")) { // If the ownership is equal to the player whose turn we're checking, do the following
				OwnedByPlayer[i/3][i%3] = true;
			}
		}
	}
	bool b_won = false;
	if (OwnedByPlayer[1][1]) // Check the center space first, as it is the most likely to be first picked by a given player.
	{
		if ((OwnedByPlayer[0][0] && OwnedByPlayer[2][2]) || (OwnedByPlayer[0][2] && OwnedByPlayer[2][0]) || (OwnedByPlayer[1][0] && OwnedByPlayer[1][2]) || (OwnedByPlayer[0][1] && OwnedByPlayer[2][1]))
			b_won = true;
	}
	if (OwnedByPlayer[2][2]) // Next check the bottom right, since checking just the top would be a waste since we already tested the center vertical. 
	{
		if ((OwnedByPlayer[2][1] && OwnedByPlayer[2][0]) || (OwnedByPlayer[0][2] && OwnedByPlayer[1][2]))
			b_won = true;
	}
	if (OwnedByPlayer[0][0]) // And finally, check the top left. It's the only remaining possibility. 
	{
		if ((OwnedByPlayer[0][1] && OwnedByPlayer[0][2]) || (OwnedByPlayer[1][0] && OwnedByPlayer[2][0]))
			b_won = true;
	}
	return b_won; // It can only be true if it was changed by the code above.
}

void CheckForWin(int* board, int playerValue = even(turn))
{
	bool b_won = false;
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
	bool b_win = false;
	int posInput = 0; // This is the position used by the computer to change the actual bits around--that is, it is the index for the bool arrays above.
	for(turn = 0; turn <10; turn++)
	{
		system("CLS");
		drawBoard();
		std::cout << (even(turn) ? COLOR_RED:COLOR_BLUE) << "it is " << whoseTurn() << "'s turn.\n" << COLOR_RESET;
		posInput = -1;
		do
		{
			if (posInput != -1)
			{
				std::cout << "\nthat position is already owned.\n";
			}
			std::cout << whoseTurn()<<" to position: ";
			std::cin >> posInput;
		} while (b_inPlay[posInput] == true);
		b_inPlay[posInput] = true;
		// This is where the winner is calculated
		if (whoseTurn() == "X") { b_ownedByX[posInput] = true; }
		if (turn >= 4) {
			if (checkWin()) {
				system("CLS");
				drawBoard();
				std::cout << (even(turn) ? COLOR_RED : COLOR_BLUE) << "\n" << whoseTurn() << " WINs!!\n";
				b_win = true;
				break;
			}
		}
	}
	if (!b_win) // If we reach this part of the code and someone still hasn't won
	{
		std::cout << "\nStalemate\n";
	}
	int none = 0;
	std::cin >> none;
}

// std::cin >> take input
// std::cout << give output

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
