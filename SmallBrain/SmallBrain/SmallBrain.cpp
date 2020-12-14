#include <iostream>
#include "Neuron.h"

int main()
{
    for (unsigned char c = 0; c < 254; ++c)
    std::cout << int(c) << " " << c << "\n";
}