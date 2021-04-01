// Type your code here, or load an example.
#include <stdio.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <array>
#include <vector>
#include <fstream>

std::atomic_int g_position = -1;
std::atomic_int g_searches = 0;

std::array<int, 1000> g_searchlist;

void Search(int wanted, int start, int end)
{
    for (int i = start; (i < end) && (g_position == -1); ++i, g_searches.store(g_searches.load() + 1))
    {
        if (g_searchlist[i] == wanted) g_position.store(i);
    }
}

bool AllCanJoin(std::thread (&thread)[10])
{
    for (int i = 0; i < 10; ++i)
    {
        if (!thread[i].joinable()) return false;
    }
    return true;
}

#define YAY(success) static_assert(success, "Test successful!"), success

double division(int a, int b) {
    if (b == 0)
    {
        throw "Division by zero condition!";
    }
    return (a / b);
}

double division2(int a, int b) {
    if (b == 0)
    {
        return 0;
    }
    return (a / b);
}
using namespace std::chrono_literals;

const char* Indent(const char* str, int amount = 1)
{
    const size_t indent = 4 * amount;
    size_t length = 0;
    for (size_t pos = 0; str[pos]; ++pos)
    {
        if (str[pos] == '\n') length += indent;
        else ++length;
    }
    char* out = new char[length];
    for (size_t pos = 0, opos = 0; pos < length; ++pos, ++opos)
    {
        if (str[pos] == '\n')
        {
            for (size_t offs = 0; offs < indent; ++offs)
            {
                out[opos + offs] = ' ';
            }
            opos += indent;
        }
        else out[opos] = str[pos];
    }
    return out;
}

const char* VectorToStringTall(std::vector<const char*>& vec)
{
    size_t size = 2; // " }" is the same length as the dropped ", " so it is not factored in.
    for (const char* elem : vec)
    {
        size_t c = 0;
        while (elem[c]) { ++c; }
        size += c;
    }
    size += vec.size() * 6;
    char* const str = new char[size + 1](); // + 1 for the null-terminating char
    str[0] = '{';
    str[1] = '\n';
    size_t pos = 2;
    for (size_t i = 0; i < vec.size() && pos < size; ++i)
    {
        for (const size_t epos = pos + 4; pos < epos;) { str[pos++] = ' '; }
        for (size_t c = 0; vec[i][c] && (pos < size - 2); ++c)
        {
            str[pos++] = vec[i][c];
        }
        str[pos++] = ',';
        str[pos++] = '\n';
    }
    str[size - 2] = '\n';
    str[size - 1] = '}';
    str[size] = '\0'; // Null-terminator
    return str;
}
const char* VectorToStringWide(std::vector<const char*>& vec)
{
    size_t size = 2; // " }" is the same length as the dropped ", " so it is not factored in.
    for (const char* elem : vec)
    {
        size_t c = 0;
        while (elem[c]) { ++c; }
        size += c;
    }
    size += vec.size() * 2;
    char* const str = new char[size + 1](); // + 1 for the null-terminating char
    str[0] = '{';
    str[1] = ' ';
    size_t pos = 2;
    for (size_t i = 0; i < vec.size() && pos < size; ++i)
    {
        for (size_t c = 0; vec[i][c] && (pos < size - 2); ++c)
        {
            str[pos++] = vec[i][c];
        }
        str[pos++] = ',';
        str[pos++] = ' ';
    }
    str[size - 2] = ' ';
    str[size - 1] = '}';
    str[size] = '\0'; // Null-terminator
    return str;
}

int main()
{
    {
        std::vector<const char*> intro{ "Hi there!", "Welcome to the Aperture Science C++ Testing Facility", "Or ASCTF", "If you have any questions...", "...Black Mesa can kiss my ass!", };
        printf("VectorToStringWide()\n%s\n\nVectorToStringTall()\n%s\n",
               VectorToStringWide(intro),
               VectorToStringTall(intro));
    }
    return 0;
    {
        std::ofstream file;
        file.open("binary.txt");

        const char binary[]{
            44, 2, 7, 72, 2, 88, 8, 32, 1, 9, 24, 2, 80, 5,     61, 7, 9,  9, 4, 80, 8, 81, 1, 4, 00, 7, 28, 1,
            84, 2, 8, 07, 5, 57, 6, 60, 9, 8, 66, 0, 22, 7,     58, 4, 7, 22, 5, 02, 0, 19, 4, 6, 67, 7,  9, 7,
            70, 0, 3, 44, 0, 72, 8, 78, 5, 1, 46, 1, 91, 4,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,

            35, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,
            34, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,
            79, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,

            96, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,
            31, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,
            81, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,

            76, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,
            53, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,
            72, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,
            17, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,

            99, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,
            69, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,
            50, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,

            27, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,
            52, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,
            77, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,     00, 0, 0, 00, 0, 00, 0, 00, 0, 0, 00, 0, 00, 0,

            11, 9, 5, 25, 0, 16, 4, 64, 1, 7, 50, 0, 20, 4,     79, 0, 9, 00, 0, 33, 1, 39, 7, 6, 57, 6, 68, 5,
                                                                                                   6, 30, 2026,
        };
        file.write(binary, sizeof(binary));

        file.close();
    }
    return 0;
}