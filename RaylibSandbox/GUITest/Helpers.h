#pragma once
#include <stdlib.h> 

bool& Toggle	(bool& b) { return (b = !b);	}
bool& Falsify	(bool& b) { return (b = false); }
bool& Truify	(bool& b) { return (b = true);	}

int& IncrementRanged(int& i, int max) { return ((i < max) ? (++i) : (i)); }
int& IncrementWrapped(int& i, int max) { return (i = ((i + 1) % max)); }
int& IncrementWrapped(int& i, int min, int max) { int wrap = ((i + 1) % max); return (i = ((!wrap) ? (min) : (wrap))); }

#ifdef RAND_MAX
bool Chance(int total, int chance) { return ((rand() % total) < chance); }
#else
bool Chance(int rand, int total, int chance) { return ((rand % total) < chance); }
#endif