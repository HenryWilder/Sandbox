#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::vector<std::string> lines;
    lines.push_back("");
    while (std::cin.good())
    {
        std::string str;
        std::cin >> str;
        bool lineEnd = (str.find(';') != str.npos);

        lines.back() += (lineEnd ? str : str + ' ');
        if (lineEnd) lines.push_back("");
    }
    for (const std::string& line : lines)
    {
        std::cout << line << std::endl;
    }
    return 0;
}
