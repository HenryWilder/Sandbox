#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

void SetConsoleColor(unsigned char r, unsigned char g, unsigned char b, bool bg = false)
{
    printf("\x1b[%u;2;%#03u;%#03u;%#03um", (bg ? 48 : 38), r, g, b);
}

enum class State
{
    Code,
    Char,
    String,
    Literal,
    Comment,
    Typename,
    Keyword,
};
enum class Primitive
{
    INT
};

std::unordered_map<std::string, Primitive> g_primitives{
    { "int", Primitive::INT },
};

int main()
{
    std::ifstream file;
    file.open("program.txt");

    State state = State::Code;

    while (!file.eof())
    {
        std::string symbol;
        file >> symbol;

        // Update state
        switch (state)
        {
        case State::Code:
            if (g_primitives.find(symbol) != g_primitives.end())
            {
                state = State::Keyword;
            }
            if (symbol[0] == '\"')
            {
                state = State::String;
            }
            if (symbol[0] == '\\' && symbol[1] == '\\')
            {
                state = State::Comment;
            }
            break;
        case State::Char:
            if (symbol[0] == '\'')
            {
                state = State::Code;
            }
            break;
        case State::String:
            for ()
            if (symbol[0] == '\"')
            {
                state = State::Code;
            }
            break;
        case State::Literal:
            break;
        case State::Comment:
            if (symbol[0] == '\n')
            {
                state = State::Code;
            }
            break;
        case State::Typename:
            break;
        case State::Keyword:
            break;
        }

        // Color state
        switch (state)
        {
        case State::Code:     SetConsoleColor(255, 255, 255); break;
        case State::Char:     SetConsoleColor(255, 255, 127); break;
        case State::String:   SetConsoleColor(255, 127, 0);   break;
        case State::Literal:  SetConsoleColor(255, 255, 127); break;
        case State::Comment:  SetConsoleColor(0, 255, 127);   break;
        case State::Typename: SetConsoleColor(255, 255, 127); break;
        case State::Keyword:  SetConsoleColor(0, 127, 255);   break;
        }
        std::cout << symbol << ' ';
        switch (state)
        {
        case State::Typename:
            state = State::Code;
            break;
        case State::Keyword:
            state = State::Code;
            break;
        }
    }

    file.close();
}