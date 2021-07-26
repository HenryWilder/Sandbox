#include <time.h>
#include "random.h"


void Random::Init()
{
#if RANDOM_HEADER_WORKING
	s_RandomEngine.seed(std::random_device()());
#else
	srand((unsigned int)clock());
#endif
}
float Random::Float()
{
#if RANDOM_HEADER_WORKING
	return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
#else
	return (float)rand() / (float)RAND_MAX;
#endif
}
