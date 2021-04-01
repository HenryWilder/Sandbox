#include "Universal.h"
#include "Input.h"
#include "Rendering.h"
#include "Board.h"
#include "Unit.h"

using namespace std;

extern FrameBuffer g_frameBuffer;

bool PromptReplay()
{
	cout << "Want to play again? (y/n) ";
	char input = 'y';
	cin >> input;
	return (input == 'y');
}



int main(void)
{
	cout << "\x1b]2;Console Chess 2.0\x07"; // Renames the window
	std::cout << "\x1b[?25l"; // Hide the cursor
	Board playSpace;

	bool replay = true;
	system("CLS"); // Reset the newlines
	//std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"; // Move the cursor where we want it

	//std::cout << "\x1b[s"; // Save the cursor position

	while (replay)
	{
		playSpace.ResetBoard(space::game::sideTileCount);

		bool gameOver = false;

		while (!gameOver)
		{
			playSpace.PlayBoard();
			gameOver = playSpace.IncrementTurn();
		}
		playSpace.GameFlipbook();

		//replay = PromptReplay();
	}

	ReleaseDC(window, hdc);
	

	return 0;
}