#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>

using Token = unsigned long long;

enum class Primative {
    BOOL    = sizeof(bool),

    CHAR    = sizeof(char),
    SHORT   = sizeof(short),
    INT     = sizeof(int),
    LONG    = sizeof(long),
    LLONG   = sizeof(long long),

    UCHAR   = sizeof(unsigned char),
    USHORT  = sizeof(unsigned short),
    UINT    = sizeof(unsigned int),
    ULONG   = sizeof(unsigned long),
    ULLONG  = sizeof(unsigned long long),

    FLOAT   = sizeof(float),
    DOUBLE  = sizeof(double),
};

class Var {
private:
    unsigned long long bits;    // Where the memory is stored
    Primative type;             // How to inturpret the memory

public:
    Var(Primative _type) : bits(), type(_type) {}

    template<> void Set() {

    }
    static void Get() {

    }
};

class Method {

};

struct LooseType {
    std::vector<Var> memory;
    std::map<Token, int> members;
    std::map<Token, Method> methods;
};

/*
* 
*   The language:
* 
*   { = enter scope
*   } = exit scope
*   var = store variable to scope (takes whatever comes next as variable name)
*   = = set the value of a variable (must already have been named)
*   name of variable = get the value of a variable
*   print = print the value of a variable
*   ; = stop reading and evaluate the line
* 
*/

int main()
{
    std::ifstream file;
    file.open("punchcard.txt");

    std::string line;

    file >> line;



    std::cout << line;

    file.close();
}