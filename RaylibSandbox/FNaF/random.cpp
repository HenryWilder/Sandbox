#include <stdlib.h>
#include "random.h"
#include <chrono>
int Random()
{
    srand(time(0));
    return rand();
}
