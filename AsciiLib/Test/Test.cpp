// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "graphics.h"

int main()
{
    InitBuffer(32, 28);
    ClearBackground();

    unsigned char tex[16 * 8];
    for (size_t y = 0; y < 8; ++y)
    {
        for (size_t x = 0; x < 16; ++x)
        {
            tex[y * 16 + x] = (unsigned char)(((float)x / 16.0f) * 255.0f);
        }
    }
    Texture texture0 =
    {
        16u,8u,
        tex
    };
    DrawTextureEx(texture0, { 1.0f,1.0f }, { 0.0f,0.0f }, { 0.0f, 0.0f });
    DrawTextureEx(texture0, { 2.0f,2.0f }, { 0.0f,0.0f }, { 0.0f, 9.0f });
    DrawBuffer();
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
