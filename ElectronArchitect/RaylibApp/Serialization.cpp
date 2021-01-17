#include "Serialization.h"
#include "Transistor.h"
#include "Wire.h"

void Save(std::vector<Transistor*>* fromArr)
{
	/******************************
	* 
	*	version 0
	*	t <pos.x> <pos.y> <type>\
	*	<number of inputs> <input 1 index> <input 1 wire direction> <input 2 index> <input 2 wire direction> <...> <input n index> <input n wire direction>\
	*	<number of outputs> <output 1 index> <output 1 wire direction> <output 2 index> <output 1 wire direction> <...> <output n index> <output n wire direction>
	* 
	******************************/

	std::ofstream saveFile("saved_diagram.txt", std::ofstream::out | std::ofstream::trunc);
	saveFile << "version 0\n";

	saveFile << "s " << fromArr->size() << "\n";

	for (Transistor* transistor : *fromArr)
	{
		saveFile << "t ";
		saveFile << transistor->pos.x << " " << transistor->pos.y << " ";

		char transistorTypeSymbol;
		switch (transistor->type)
		{
		case Transistor::Type::Invert:
			transistorTypeSymbol = '!';
			break;

		default: case Transistor::Type::Simple:
			transistorTypeSymbol = '|';
			break;
		}
		saveFile << transistorTypeSymbol << " ";

		saveFile << transistor->inputs.size() << " ";

		for (Wire* inputWire : transistor->inputs)
		{
			for (size_t searchIndex = 0; searchIndex < fromArr->size(); ++searchIndex)
			{
				if (fromArr->at(searchIndex) == inputWire->inTransistor)
				{
					saveFile << searchIndex << " ";

					char directionSymbol;
					switch (inputWire->direction)
					{
					default: case WireDirection::XFirst:
						directionSymbol = 'x';
						break;

					case WireDirection::YFirst:
						directionSymbol = 'y';
						break;

					case WireDirection::Direct:
						directionSymbol = 'd';
						break;
					}
					saveFile << directionSymbol << " ";

					break; // Break out of search, not input array
				}
			}
		}

		saveFile << transistor->outputs.size() << " ";

		for (Wire* outputWire : transistor->outputs)
		{
			for (size_t searchIndex = 0; searchIndex < fromArr->size(); ++searchIndex)
			{
				if (fromArr->at(searchIndex) == outputWire->outTransistor)
				{
					saveFile << searchIndex << " ";

					char directionSymbol;
					switch (outputWire->direction)
					{
					default: case WireDirection::XFirst:
						directionSymbol = 'x';
						break;

					case WireDirection::YFirst:
						directionSymbol = 'y';
						break;

					case WireDirection::Direct:
						directionSymbol = 'd';
						break;
					}
					saveFile << directionSymbol << " ";

					break; // Break out of search, not output array
				}
			}
		}

		saveFile << '\n';
	}

	saveFile.close();
}

void Load(std::vector<Transistor*>* toArr, std::vector<Wire*>* wireArr)
{
	std::ifstream loadFile("saved_diagram.txt", std::ifstream::in);
	std::vector<Transistor*> transistors;
	std::vector<size_t> linkIndexes;

	loadFile.ignore(256, 's');
	size_t size = 0;
	loadFile >> size;
	while (loadFile.tellg() < size)
	{
		Transistor* transistor = new Transistor;
		transistors.push_back(transistor);

		loadFile.ignore(256,'t');
		loadFile >> transistor->pos.x >> transistor->pos.y;

		char transistorTypeSymbol;
		loadFile >> transistorTypeSymbol;
		switch (transistorTypeSymbol)
		{
		case '!':
			transistor->type = Transistor::Type::Invert;
			break;

		default: case '|':
			transistor->type = Transistor::Type::Simple;
			break;
		}

		// TODO

		//for (Wire* inputWire : transistor->inputs)
		//{
		//	saveFile << transistor->inputs.size() << " ";
		//	for (size_t searchIndex = 0; searchIndex < fromArr->size(); ++searchIndex)
		//	{
		//		if (fromArr->at(searchIndex) == inputWire->inTransistor)
		//		{
		//			saveFile << searchIndex << " ";
		//
		//			char directionSymbol;
		//			switch (inputWire->direction)
		//			{
		//			default: case WireDirection::XFirst:
		//				directionSymbol = 'x';
		//				break;
		//
		//			case WireDirection::YFirst:
		//				directionSymbol = 'y';
		//				break;
		//
		//			case WireDirection::Direct:
		//				directionSymbol = 'd';
		//				break;
		//			}
		//			saveFile << directionSymbol << " ";
		//
		//			break; // Break out of search, not input array
		//		}
		//	}
		//}
		//
		//for (Wire* outputWire : transistor->outputs)
		//{
		//	saveFile << transistor->outputs.size() << " ";
		//	for (size_t searchIndex = 0; searchIndex < fromArr->size(); ++searchIndex)
		//	{
		//		if (fromArr->at(searchIndex) == outputWire->outTransistor)
		//		{
		//			saveFile << searchIndex << " ";
		//
		//			char directionSymbol;
		//			switch (outputWire->direction)
		//			{
		//			default: case WireDirection::XFirst:
		//				directionSymbol = 'x';
		//				break;
		//
		//			case WireDirection::YFirst:
		//				directionSymbol = 'y';
		//				break;
		//
		//			case WireDirection::Direct:
		//				directionSymbol = 'd';
		//				break;
		//			}
		//			saveFile << directionSymbol << " ";
		//
		//			break; // Break out of search, not output array
		//		}
		//	}
		//}
	}

	loadFile.close();

	for (Transistor* transistor : transistors)
	{
		toArr->push_back(transistor);
	}
}
