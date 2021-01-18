#include "Serialization.h"
#include "Transistor.h"
#include "Wire.h"

std::ostream& operator<<(std::ostream& file, const Vector2& vector)
{
	file << vector.x << " " << vector.y;
	return file;
}
std::istream& operator>>(std::istream& file, Vector2& vector)
{

	file >> vector.x >> vector.y;
	return file;
}

std::ostream& operator<<(std::ostream& file, const Transistor::Type& type)
{
	switch (type)
	{
	default: case Transistor::Type::Simple:
		file << '|';
		break;
	case Transistor::Type::Invert:
		file << '!';
		break;
	}
	return file;
}
std::istream& operator>>(std::istream& file, Transistor::Type& transistorType)
{
	char transistorTypeSymbol;
	file >> transistorTypeSymbol;
	switch (transistorTypeSymbol)
	{
	default: case '|':
		transistorType = Transistor::Type::Simple;
		break;
	case '!':
		transistorType = Transistor::Type::Invert;
		break;
	}
	return file;
}

std::ostream& operator<<(std::ostream& file, const WireDirection& direction)
{
	switch (direction)
	{
	default: case WireDirection::XFirst:
		file << 'x';
		break;

	case WireDirection::YFirst:
		file << 'y';
		break;

	case WireDirection::Direct:
		file << 'd';
		break;
	}
	return file;
}
std::istream& operator>>(std::istream& file, WireDirection& direction)
{
	char wireDirectionSymbol;
	file >> wireDirectionSymbol;
	switch (wireDirectionSymbol)
	{
	default: case 'x':
		direction = WireDirection::XFirst;
		break;
	case 'y':
		direction = WireDirection::YFirst;
		break;
	case 'd':
		direction = WireDirection::Direct;
		break;
	}
	return file;
}


void Save(const std::vector<Transistor*>* fromArr)
{
	/******************************
	* 
	*	version 0
	*	t <pos.x> <pos.y> <type>\
	*	<number of inputs> <input 1 index> <input 2 index> <...> <input n index>\
	*	<number of outputs> <output 1 index> <output 1 wire direction> <output 2 index> <output 1 wire direction> <...> <output n index> <output n wire direction>
	* 
	******************************/

	std::ofstream saveFile("saved_diagram.txt", std::ofstream::out | std::ofstream::trunc);
	saveFile << "version 0\n";

	saveFile << "s " << fromArr->size() << "\n";

	for (Transistor* const& transistor : *fromArr)
	{
		saveFile << "t " << transistor->pos << " " << transistor->type << " ";

		// INPUTS
		saveFile << "i " << transistor->inputs.size() << " ";
		for (Wire* const& inputWire : transistor->inputs)
		{
			auto found = std::find(fromArr->begin(), fromArr->end(), inputWire->inTransistor); // Find the input
			size_t foundIndex = std::distance(fromArr->begin(), found); // Find the index of the located element

			saveFile << "[ " << foundIndex << " ] "; // Write the index
		}

		
		// OUTPUTS
		saveFile << "o " << transistor->outputs.size() << " ";
		for (Wire* const& outputWire : transistor->outputs)
		{
			auto found = std::find(fromArr->begin(), fromArr->end(), outputWire->outTransistor); // Find the input
			size_t foundIndex = std::distance(fromArr->begin(), found); // Find the index of the located element

			saveFile << "[ " << foundIndex << " "  << outputWire->direction << " ] "; // The wire direction will only be on the output side of the line.
		}

		saveFile << '\n';
	}

	saveFile.close();
}

void Load(std::vector<Transistor*>* toArr, std::vector<Wire*>* wireArr)
{
	std::ifstream loadFile("saved_diagram.txt", std::ifstream::in);
	// These vectors are local so that indexes can be absolute
	std::vector<Transistor*> transistors;
	std::vector<Wire*> wires;
	std::vector<size_t> linkIndexes; // TODO: Is this needed?

	// Get the number of transistors
	loadFile.ignore(256, 's');
	size_t transistorCount = 0;
	loadFile >> transistorCount;

	// Populate the transistor vector
	// This will allow us to accurately obtain pointers from indexes
	for (int i = 0; i < transistorCount; ++i)
	{
		Transistor* transistor = new Transistor(); // These transistors will be used outside this function, so we want to allocate memory for them.
		transistors.push_back(transistor);
	}

	for (Transistor* transistor : transistors)
	{
		loadFile.ignore(256, 't');
		
		loadFile >> transistor->pos;
		loadFile >> transistor->type;

		size_t inputCount = 0;
		loadFile >> inputCount;
		// INPUT LIST
		for (size_t i = 0; i < inputCount; ++i) // For each input
		{
			size_t inputTransistorIndex;
			loadFile >> inputTransistorIndex;
			Transistor*& inputTransistor = transistors[inputTransistorIndex];

			// TODO: Think through how this is going actually work sequencially. When and how will the wires be initialized and how can we know whether one is already connecting to us?
			Wire* newWire = nullptr;
			for (Wire* wire : inputTransistor->outputs)
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
				newWire->inTransistor = inputTransistor;
				newWire->outTransistor = transistor;
				transistor->inputs.push_back(newWire);
			}
		}

		// TODO
		size_t outputCount = 0;
		loadFile >> outputCount;
		for (size_t i = 0; i < outputCount; ++i) // For each output
		{
			size_t outputTransistorIndex = 0;
			loadFile >> outputTransistorIndex;
			Wire* newWire = nullptr;
			for (Wire* wire : transistors[outputTransistorIndex]->inputs)
			{
				if (wire->inTransistor == transistor) // Searching for self as input to wire
				{
					newWire = wire;
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

			loadFile >> newWire->direction;
		}
	}

	loadFile.close();

	for (Transistor* transistor : transistors)
	{
		toArr->push_back(transistor);
	}
}
