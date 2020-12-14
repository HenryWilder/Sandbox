#pragma once
#include <stdlib.h>
#include "Rendering.h"

double DistanceToBorder(AbstrPos p, AbstrPos a, AbstrPos b);

struct NoiseSpace2
{
private:
	void Init(Whole count, Whole seed);

public:
	// Creates a new noise space
	NoiseSpace2(Whole total, Whole seed = 0);

	// Used for making noise which is smaller than the total screen
	NoiseSpace2(double frequency, Whole seed = 0, Whole area = screen::area);

	NoiseSpace2();

	~NoiseSpace2();

	Whole m_size;

	AbstrPos* m_node;

	int RandomInRange(int min, int max);

	AbstrPos RandomPosition();

	void Initialize(Whole seed);
};

double Whirly(AbstrPos pos, void* data);
double Voronoi(AbstrPos pos, void* data);
double Cell(AbstrPos pos, void* data);