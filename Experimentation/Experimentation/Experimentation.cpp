#include <iostream>
#include "Input.h"
#include <Windows.h>
#include <conio.h>
#include <chrono>
#define coolVariableName 547

using namespace std;
using namespace chrono;

namespace a
{
    void MyFunction(int input)
    {
        cout << input << '\n';
    }
    int 547 = 3;
}
namespace b
{
    int MyFunction(int input)
    {
        return input * 3;
    }
    int 547 = 57382;
}

int main()
{
    near
    a::MyFunction(a::547);
    a::MyFunction(b::547);
    a::MyFunction(b::MyFunction(a::547));


    /*
    InitKeys();
    bool keyDown = false;
    bool KeyFlop = false;
    while (true) {
        CheckInputs();
    }
    */
}
