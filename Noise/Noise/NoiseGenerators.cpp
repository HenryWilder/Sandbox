#include "NoiseGenerators.h"

double DistanceToBorder(AbstrPos p, AbstrPos a, AbstrPos b)
{
	AbstrPos borderMidpoint = (a + b) / 2;

	return Distance(p, borderMidpoint);
}

double Whirly(AbstrPos pos, void* data)
{
	NoiseSpace2* space = reinterpret_cast<NoiseSpace2*>(data);

	double shortestDist = screen::diagonal; // The cutoff distance we should be checking for

	for (Whole i = 0; i < space->m_size; ++i)
	{
		if (Distance(pos, space->m_node[i]) < shortestDist)
		{
			shortestDist = Distance(pos, space->m_node[i]);
		}
	}

	return 255 - (shortestDist * (255 / 30));
}

double Voronoi(AbstrPos pos, void* data)
{
	NoiseSpace2* space = reinterpret_cast<NoiseSpace2*>(data);

	double shortestDist = screen::diagonal; // The cutoff distance we should be checking for (no line should be of greater length than the diagonal of the screen)

	Whole nearestNode = space->m_size;

	for (Whole i = 0; i < space->m_size; ++i)
	{
		AbstrPos node = space->m_node[i];

		if (Distance(pos, node) < shortestDist)
		{
			nearestNode = i;

			shortestDist = Distance(pos, node);
		}
	}

	srand(nearestNode);

	return rand();
}

double Cell(AbstrPos pos, void* data)
{
	NoiseSpace2* space = reinterpret_cast<NoiseSpace2*>(data);

	double shortestDist = screen::diagonal; // The cutoff distance we should be checking for (no line should be of greater length than the diagonal of the screen)

	AbstrPos* nearestNodes[2] = { nullptr, nullptr };

	for (Whole i = 0; i < space->m_size; ++i)
	{
		AbstrPos node = space->m_node[i];

		if (Distance(pos, node) < shortestDist)
		{
			nearestNodes[1] = nearestNodes[0];

			nearestNodes[0] = &node;

			shortestDist = Distance(pos, node);
		}
	}

	double diff = 1.0;

	if (nearestNodes[0] != nullptr && nearestNodes[1] != nullptr)
	{
		diff = abs(Distance(pos, *nearestNodes[0]) - Distance(pos, *nearestNodes[1])); // Difference between the distance from the nearest node the the second nearest
	}

	return diff * 255;
}

/// <summary>
/// Constructs the noise space
/// </summary>
/// <param name="frequency">The number of nodes per unit of area</param>
/// <param name="area">The amount of space being covered</param>

void NoiseSpace2::Init(Whole count, Whole seed)
{
	m_size = count;

	m_node = new AbstrPos[count];

	Initialize(seed);
}

NoiseSpace2::NoiseSpace2(Whole total, Whole seed)
{
	Init(total, seed);
}

NoiseSpace2::NoiseSpace2(double frequency, Whole seed, Whole area)
{
	Init((Whole)(frequency * (double)area), seed);
	// The reason we are casting area to double is so that frequency & area can multiply together cleanly before being truncated to an Whole.
}

NoiseSpace2::NoiseSpace2()
{
	Init(8, 0);
}

NoiseSpace2::~NoiseSpace2()
{
	delete [] m_node;
}

int NoiseSpace2::RandomInRange(int min, int max)
{
	int v = rand();
	return v % (max - min) + min;
}

AbstrPos NoiseSpace2::RandomPosition()
{
	AbstrPos pos;
	pos.x = RandomInRange(0, screen::width);
	pos.y = RandomInRange(0, screen::height);

	return pos;
}

void NoiseSpace2::Initialize(Whole seed)
{
	srand(seed);

	for (Whole i = 0; i < m_size; ++i)
	{
		m_node[i] = RandomPosition();
	}
}
