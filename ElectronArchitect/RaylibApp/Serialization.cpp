#include <fstream>
#include <raylib.h>
#include <raymath.h>
#include "CustomTypes.h"
#include "Globals.h"
#include "Transistor.h"
#include "Serialization.h"

std::ostream& operator<<(std::ostream& file, const Int2& vector)
{
	file << vector.x << " " << vector.y;
	return file;
}
std::istream& operator>>(std::istream& file, Int2& vector)
{
	file >> vector.x >> vector.y;
	return file;
}

std::ostream& operator<<(std::ostream& file, const Transistor::Gate& type)
{
	file << GateToSymbol(type);
	return file;
}
std::istream& operator>>(std::istream& file, Transistor::Gate& transistorType)
{
	char transistorTypeSymbol;
	file >> transistorTypeSymbol;
	transistorType = SymbolToGate(transistorTypeSymbol);
	return file;
}

std::ostream& operator<<(std::ostream& file, const Transistor::Connection::Shape& direction)
{
	switch (direction)
	{
	default: case Transistor::Connection::Shape::XFirst:
		file << 'x';
		break;

	case Transistor::Connection::Shape::YFirst:
		file << 'y';
		break;

	case Transistor::Connection::Shape::DiagStart:
		file << 's';
		break;
	case Transistor::Connection::Shape::DiagEnd:
		file << 'e';
		break;
	}
	return file;
}
std::istream& operator>>(std::istream& file, Transistor::Connection::Shape& direction)
{
	char wireDirectionSymbol;
	file >> wireDirectionSymbol;
	switch (wireDirectionSymbol)
	{
	default: case 'x':
		direction = Transistor::Connection::Shape::XFirst;
		break;
	case 'y':
		direction = Transistor::Connection::Shape::YFirst;
		break;
	case 's':
		direction = Transistor::Connection::Shape::DiagStart;
		break;
	case 'e':
		direction = Transistor::Connection::Shape::DiagEnd;
		break;
	}
	return file;
}

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

void Save(const std::vector<Transistor*>& fromArr)
{
	const int versionNumber = 1;
	std::ofstream saveFile("saved_diagram.txt", std::ofstream::out | std::ofstream::trunc);
	saveFile << "version 1\nc " << fromArr.size() << "\n";
	for (Transistor* const& transistor : fromArr)
	{
		saveFile << "t " << transistor->GetPos() << " " << transistor->GetGateType() << " " <<
			"[ "<< FindIndex(fromArr, transistor->output[0].connector) << " " << transistor->output[0].shape << " ] " <<
			"[ "<< FindIndex(fromArr, transistor->output[1].connector) << " " << transistor->output[1].shape << " ]\n";
	}

	saveFile.close();
}

void Load(std::vector<Transistor*>& toArr)
{
	std::ifstream loadFile("saved_diagram.txt", std::ifstream::in);
	std::vector<Transistor*> transistors; // Local copy for relative pointers

	// Get the number of transistors
	loadFile.ignore(256, 'c');
	size_t transistorCount;
	loadFile >> transistorCount;

	transistors.reserve(transistorCount);
	toArr.reserve(toArr.size() + transistorCount);
	//Transistor::s_allTransistors.reserve(s_allTransistors.size() + transistorCount);

	// Populate the transistor vector
	// This will allow us to accurately obtain pointers from indexes
	// This NEEDS to happen BEFORE the next loop so that the connections can be linked correctly! Stop trying to optimize it out, dumbass!!
	for (int i = 0; i < transistorCount; ++i)
	{
		Transistor* transistor = new Transistor(); // These transistors will be used outside this function, so we want to allocate memory for them.
		transistors.push_back(transistor);
		toArr.push_back(transistor); // Push the transistor to the output array; we've already got it allocated now, anyway.
	}

	for (Transistor* thisLineTransistor : transistors) // The transistor on the line being read
	{
		size_t outputCount;
		loadFile.ignore(256, 't') >> thisLineTransistor->pos  >> thisLineTransistor->type >> outputCount;

		// Connections
		for (int i = 0; i <= 1; ++i)
		{
			size_t outputTransistorIndex;
			loadFile.ignore(256, '[') >> outputTransistorIndex;
			if (outputTransistorIndex != transistorCount)
			{
				Transistor* connectedTransistor = transistors[outputTransistorIndex]; // Being used pretty much as just an alias. Saves the extra step/confusion of having the "[...]" in two places.
				thisLineTransistor->SolderOutput(connectedTransistor); // TODO: We can make this better
				loadFile >> thisLineTransistor->output[i].shape;
			}
		}
	}

	loadFile.close();
}
