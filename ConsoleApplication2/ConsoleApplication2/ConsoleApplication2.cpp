// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <array>

int main()
{
    std::array<int, 4> objects[8] = {};

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < objects->size(); ++x)
        {
            objects[x][y] = rand()%10;
        }
    }
    std::cout << objects->data() << '\n';
    for (int y = 0; y < objects->size(); ++y)
    {
        std::cout << y << ":";
        for (int x = 0; x < 4; ++x)
        {
            std::cout << "[" << (objects[x][y]) << "]";
        }
        std::cout << '\n';
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
