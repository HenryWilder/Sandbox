#include "Serialize.h"
#include <fstream>
#include <unordered_map>
#include "Thread.h"
#include "Notecard.h"
#include "CBButton.h"

std::string g_activeBoard;

/*
* v00.000.001
* c 2 t 1
* 
* x 23.364 y 58.324 r 255 g 255 b 255 ### Title ``` Content ```
* x 753.17777 y 266.2432 r 255 g 0 b 0 ###  ```  ```
* 
* s 0 d 1 r 100 g 140 b 140
* 
*/
void SaveBoard(std::string filename)
{
	std::unordered_map<Notecard*, int> cardNumber;

	std::ofstream destFile = std::ofstream(filename);

	destFile << "v00.000.001\n"; // Version number
	destFile << "c " << g_cards.size(); // Number of cards
	destFile << "t" << g_threads.size() << '\n'; // Number of threads

	destFile << '\n';

	int i = 0;
	for (Notecard* card : g_cards)
	{
		cardNumber.emplace(card, i);
		destFile
			<< "x " << card->position.x << " y " << card->position.y // Position
			<< " r " << card->color.r << " g " << card->color.g << " b " << card->color.b // Color
			<< " ### " << card->title << " ``` " << card->content << " ```\n"; // Content
		++i;
	}

	destFile << '\n';

	for (Thread* thread : g_threads)
	{
		destFile
			<< "s " << cardNumber.find(thread->start)->second // Source
			<< " d " << cardNumber.find(thread->end)->second // Destination
			<< " r " << thread->color.r << " g " << thread->color.g << " b " << thread->color.b << '\n'; // Color
	}

	destFile.close();
}

/*
* v%i.%i.%i
* c %i t %i
* 
* x %f y %f r %i g %i b %i ### %s ``` %s ```
* x %f y %f r %i g %i b %i ### %s ``` %s ```
* 
* s %i d %i r %i g %i b %i
* 
*/
void LoadBoard(std::string filename)
{
	g_cards.clear();
	g_threads.clear();

	std::ifstream srcFile = std::ifstream(filename);

	std::string code;
	if (srcFile >> code; code != "v00.000.001") goto Error;

	int cardsCount;
	int threadsCount;

	// Card count
	if (srcFile >> code; code != "c") goto Error;
	srcFile >> cardsCount;

	// Thread count
	if (srcFile >> code; code != "t") goto Error;
	srcFile >> threadsCount;
	g_threads.reserve(threadsCount);

	// Load cards
	for (int i = 0; i < cardsCount; ++i)
	{
		Vector2 position{0,0}; // Position
		Color color{0,0,0,255}; // Color
		std::string title, content; // Text

		// Position
		if (srcFile >> code; code != "x") goto Error;
		srcFile >> position.x;
		if (srcFile >> code; code != "y") goto Error;
		srcFile >> position.y;
		if (srcFile >> code; code != "r") goto Error;
		srcFile >> color.r;
		if (srcFile >> code; code != "g") goto Error;
		srcFile >> color.g;
		if (srcFile >> code; code != "b") goto Error;
		srcFile >> color.b;
		if (srcFile >> code; code != "###") goto Error;
		std::getline(srcFile, title, '`');
		if (srcFile >> code; code != "``") goto Error;
		std::getline(srcFile, content, '`');
		if (srcFile >> code; code != "``") goto Error;

		Notecard* card = new Notecard(position, color, title, content);
		g_cards.push_back(card);
	}

	// Load threads
	for (int i = 0; i < cardsCount; ++i)
	{
		int sourceIndex, destinationIndex;
		Color color{0,0,0,255};

		if (srcFile >> code; code != "s") goto Error;
		srcFile >> sourceIndex;
		if (srcFile >> code; code != "d") goto Error;
		srcFile >> destinationIndex;

		if (sourceIndex == destinationIndex) goto Error;

		if (srcFile >> code; code != "r") goto Error;
		srcFile >> color.r;
		if (srcFile >> code; code != "g") goto Error;
		srcFile >> color.g;
		if (srcFile >> code; code != "b") goto Error;
		srcFile >> color.b;

		Notecard* source = g_cards[sourceIndex];
		Notecard* destination = g_cards[destinationIndex];
		Thread* thread = new Thread(color, source, destination);
		source->AddThreadConnection(thread);
		destination->AddThreadConnection(thread);
		g_threads.push_back(thread);
	}

	goto Close;

Error:
	printf("File corrupted");
Close:
	srcFile.close();
}
