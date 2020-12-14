#include "Input.h"

using namespace std;

SHORT keyInputs[5]; // The set of keys being pressed (down, left, right, rotate counterclockwise, rotate clockwise)

void InitKeys()
{
    keyInputs[0] = true;
    keyInputs[1] = true;
    keyInputs[2] = true;
    keyInputs[3] = true;
    keyInputs[4] = true;
}

void KeyPress(SHORT key)
{
    switch (key)
    {
    case VK_LEFT:
        cout << "left";
        break;
    case VK_UP:
        cout << "up";
        break;
    case VK_RIGHT:
        cout << "right";
        break;
    case VK_DOWN:
        cout << "down";
        break;
    case VK_CONTROL:
        cout << "ctrl";
        break;
    default:
        break;
    }
}

void CheckInputs()
{
    if (keyInputs[0] == GetKeyState(VK_LEFT))
    {
        keyInputs[0] = !keyInputs[0];
        KeyPress(VK_LEFT);
    }
    if (keyInputs[1] == GetKeyState(VK_UP))
    {
        keyInputs[1] = !keyInputs[1];
        KeyPress(VK_UP);
    }
    if (keyInputs[2] == GetKeyState(VK_RIGHT))
    {
        keyInputs[2] = !keyInputs[2];
        KeyPress(VK_RIGHT);
    }
    if (keyInputs[3] == GetKeyState(VK_DOWN))
    {
        keyInputs[3] = !keyInputs[3];
        KeyPress(VK_DOWN);
    }
    if (keyInputs[4] == GetKeyState(VK_CONTROL))
    {
        keyInputs[4] = !keyInputs[4];
        KeyPress(VK_CONTROL);
    }
}