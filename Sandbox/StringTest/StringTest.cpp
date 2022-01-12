// StringTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <cstdarg>

bool Match(const char* token, const char* test)
{
    for (int i = 0; i < 4; ++i)
    {
        if (token[i] != test[i]) return false;
    }
    return true;
}

int main()
{
    char func[5];
    char args[4][5];
    scanf_s("%4s", func, (unsigned)4);
    if (Match(func, "mov"))
    {
        scanf_s("%4s %4s", args[0], (unsigned)4, args[1], (unsigned)4);
        printf("%s moved to %s", args[0], args[1]);
    }
    else if (Match(func, "and"))
    {
        scanf_s("%4s %4s %4s", args[0], (unsigned)4, args[1], (unsigned)4, args[2], (unsigned)4);
        printf("result of %s && %s stored to register %s", args[1], args[2], args[0]);
    }
    else
    {
        printf("Error");
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
