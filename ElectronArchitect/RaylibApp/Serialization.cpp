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
		default: case Transistor::Type::Simple:
			transistorTypeSymbol = '|';
			break;
		case Transistor::Type::Invert:
			transistorTypeSymbol = '!';
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
	std::vector<Wire*> wires;
	std::vector<size_t> linkIndexes;

	// Get file size
	//loadFile.seekg(0, std::ifstream::end);
	//std::streampos fileSize = loadFile.tellg();
	//loadFile.seekg(0, std::ifstream::beg);

	// Get number of transistors
	loadFile.ignore(256, 's');
	size_t transistorCount = 0;
	loadFile >> transistorCount;

	for (int i = 0; i < transistorCount; ++i)
	{
		Transistor* transistor = new Transistor();
		transistors.push_back(transistor);
	}

	for (Transistor* transistor : transistors)
	{
		loadFile.ignore(256, 't');
		
		loadFile >> transistor->pos.x >> transistor->pos.y;

		char transistorTypeSymbol;
		loadFile >> transistorTypeSymbol;
		switch (transistorTypeSymbol)
		{
		default: case '|':
			transistor->type = Transistor::Type::Simple;
			break;
		case '!':
			transistor->type = Transistor::Type::Invert;
			break;
		}

		size_t inputCount = 0;
		loadFile >> inputCount;
		for (size_t i = 0; i < inputCount; ++i)
		{
			size_t inputTransistorIndex = 0;
			loadFile >> inputTransistorIndex;

			// TODO: Think through how this is going actually work sequencially. When and how will the wires be initialized and how can we know whether one is already connecting to us?
			Wire* newWire = nullptr;
			for (Wire* wire : transistors[inputTransistorIndex]->outputs)
			{
				if (wire->outTransistor == transistor) // Searching for self as output to wire
				{
					newWire = wire;
					break;
				}
			}
			if (!newWire) // If an existing wire was not found, create one.
			{
				newWire = new Wire();
				newWire->inTransistor = transistors[inputTransistorIndex];
				newWire->outTransistor = transistor;
				transistor->inputs.push_back(newWire);
			}

			// TODO: Probably not necessary if the wire has already been previously initialized!
			char wireDirectionSymbol;
			loadFile >> wireDirectionSymbol;
			switch (wireDirectionSymbol)
			{
			default: case 'x':
				newWire->direction = WireDirection::XFirst;
				break;
			case 'y':
				newWire->direction = WireDirection::YFirst;
				break;
			case 'd':
				newWire->direction = WireDirection::Direct;
				break;
			}
		}

		// TODO
		size_t outputCount = 0;
		loadFile >> outputCount;
		for (size_t i = 0; i < outputCount; ++i)
		{
			size_t outputTransistorIndex = 0;
			loadFile >> outputTransistorIndex;

			Wire* newWire = nullptr;
			for (Wire* wire : transistors[outputTransistorIndex]->inputs)
			{
				if (wire->inTransistor == transistor) // Searching for self as input to wire
				{
					newWire = wire;
					wire->outTransistor = transistor;
					break;
				}
			}
			if (!newWire) // If an existing wire was not found, create one.
			{
				newWire = new Wire();
				newWire->inTransistor = transistor;
				newWire->outTransistor = transistors[outputTransistorIndex];
				transistor->inputs.push_back(newWire);
			}

			char wireDirectionSymbol;
			loadFile >> wireDirectionSymbol;
			switch (wireDirectionSymbol)
			{
			default: case 'x':
				newWire->direction = WireDirection::XFirst;
				break;
			case 'y':
				newWire->direction = WireDirection::YFirst;
				break;
			case 'd':
				newWire->direction = WireDirection::Direct;
				break;
			}
		}
	}

	loadFile.close();

	for (Transistor* transistor : transistors)
	{
		toArr->push_back(transistor);
	}
}
