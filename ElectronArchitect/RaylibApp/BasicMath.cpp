#include "BasicMath.h"

int Round(float val)
{
	return (int)(SignScalar(val) * (Abs(val) + 0.5f));
}

int Round(double val)
{
	return (int)(SignScalar(val) * (Abs(val) + 0.5));
}
