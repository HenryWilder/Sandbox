#include <iostream>
#include <string>

int main()
{
    std::string data =
R"(struct MyType
{

};)";
    printf("%s\n", data.c_str());
    std::cout << "\x1b[3;1H    ";
    std::string input;
    std::cin >> input;

    return 0;
}
