#include <fstream>
#include "Serialization.h"
#include "Globals.h"
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

std::ostream& operator<<(std::ostream& file, const TransistorType& type)
{
	file << GateToSymbol(type);
	return file;
}
std::istream& operator>>(std::istream& file, TransistorType& transistorType)
{
	char transistorTypeSymbol;
	file >> transistorTypeSymbol;
	transistorType = SymbolToGate(transistorTypeSymbol);
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

	case WireDirection::DiagStart:
		file << 's';
		break;
	case WireDirection::DiagEnd:
		file << 'e';
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
	case 's':
		direction = WireDirection::DiagStart;
		break;
	case 'e':
		direction = WireDirection::DiagEnd;
		break;
	}
	return file;
}

void Save(const std::vector<Transistor*>* fromArr)
{
	std::ofstream saveFile("saved_diagram.txt", std::ofstream::out | std::ofstream::trunc);
	saveFile << "version 0\n";

	saveFile << "c " << fromArr->size() << "\n";

	for (Transistor* const& transistor : *fromArr)
	{
		saveFile << "t " << transistor->pos << " " << transistor->type << " ";

		/*********************************
		* 
		*	Connections are output only.
		* 
		*	a--->b--->c
		*	-a(b)
		*	-b(c)
		*	-c
		*	
		*	a--.
		*		\
		*		 :-->c
		*		/
		*	b--'
		*	-a(c)
		*	-b(c)
		*	-c
		* 
		*		 .-->b
		*		/
		*	a--:
		*		\
		*		 '-->c
		*	-a(b,c)
		*	-b
		*	-c
		* 
		*********************************/

		saveFile << transistor->outputs.size() << " ";
		for (Wire* const& outputWire : transistor->outputs)
		{
			size_t foundIndex = FindIndex(*fromArr, (outputWire->outTransistor));
			saveFile << "[ " << foundIndex << " "  << outputWire->direction << " ] "; // The wire direction will only be on the output side of the line.
		}

		saveFile << '\n'; // Newline for next transistor
	}

	saveFile.close();
}

void Load(std::vector<Transistor*>* toArr, std::vector<Wire*>* wireArr)
{
	std::ifstream loadFile("saved_diagram.txt", std::ifstream::in);
	// These vectors are local so that indexes can be relative. We don't know how many wires/transistors may already be in the parameter vectors.
	// Then again, I could probably just store an offset. Idk. I don't wanna risk it.
	std::vector<Transistor*> transistors;
	std::vector<Wire*> wires;

	// Get the number of transistors
	loadFile.ignore(256, 'c');
	size_t transistorCount;
	loadFile >> transistorCount;

	// Populate the transistor vector
	// This will allow us to accurately obtain pointers from indexes
	// This NEEDS to happen BEFORE the next loop so that the connections can be linked correctly! Stop trying to optimize it out, dumbass!!
	for (int i = 0; i < transistorCount; ++i)
	{
		Transistor* transistor = new Transistor(); // These transistors will be used outside this function, so we want to allocate memory for them.
		transistors.push_back(transistor);
		toArr->push_back(transistor); // Push the transistor to the output array; we've already got it allocated now, anyway.
	}

	for (Transistor* thisLineTransistor : transistors) // The transistor on the line being read
	{
		size_t outputCount;
		loadFile.ignore(256, 't') >> thisLineTransistor->pos  >> thisLineTransistor->type >> outputCount;

		// Connections
		for (size_t i = 0; i < outputCount; ++i) // For each output
		{
			size_t outputTransistorIndex;
			loadFile.ignore(256, '[') >> outputTransistorIndex;
			Transistor* connectedTransistor = transistors[outputTransistorIndex]; // Being used pretty much as just an alias. Saves the extra step/confusion of having the "[...]" in two places.

			// Each connection will have a 1:1 wire to connection ratio. If a connection is listed, then it's our first encounter with it.
			Wire* newWire = new Wire();
			wireArr->push_back(newWire); // Now that its been allocated and given a pointer, it can be pushed to the output array without any issue.

			newWire->inTransistor = thisLineTransistor;
			thisLineTransistor->outputs.push_back(newWire);
			newWire->outTransistor = connectedTransistor;
			connectedTransistor->inputs.push_back(newWire);

			loadFile >> newWire->direction;
		}
	}

	loadFile.close();
}
