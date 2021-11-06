// FakeAssembly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

//#define LOG

constexpr uint32_t CharsToInt(const char* str)
{
    return
        (str[0] == 0 ? 0 : ((uint32_t)str[0]) |
            (str[1] == 0 ? 0 : ((uint32_t)str[1] << 010) |
                (str[2] == 0 ? 0 : ((uint32_t)str[2] << 020) |
                    (str[3] == 0 ? 0 : ((uint32_t)str[3] << 030)))));
}

enum Instruction : uint32_t
{
    MOV = CharsToInt("mov"),    // Move
    MVN = CharsToInt("mvn"),    // Move inverted

    ADD = CharsToInt("add"),    // Add
    ADC = CharsToInt("adc"),    // Add and carry
    SUB = CharsToInt("sub"),    // Subtract
    SBC = CharsToInt("sbc"),    // Subtract and carry
    RSB = CharsToInt("rsb"),    // Reverse subtract (n-Rn instead of Rn-n)
    RSC = CharsToInt("rsc"),    // Reverse subtract and carry

    AND = CharsToInt("and"),    // Bitwise AND
    ORR = CharsToInt("orr"),    // Bitwise OR
    EOR = CharsToInt("eor"),    // Bitwise XOR
    BIC = CharsToInt("bic"),    // Bitclear (a & ~b)

    CMP = CharsToInt("cmp"),    // Compare
    CMN = CharsToInt("cmn"),    // Compare negative
    TST = CharsToInt("tst"),    // Test a bit in a 32-bit value
    TEQ = CharsToInt("teq"),    // Test for equality

    MUL = CharsToInt("mul"),    // Multiply
    MLA = CharsToInt("mla"),    // Multiply and accumulate

    SAY = CharsToInt("say"),    // Reads off the value at the location
};

template<typename T, size_t size>
class RegStack
{
    T data[size];

public:
    int Get(size_t i)
    {
        return data[i];
    }
    void Set(size_t i, T value)
    {
        data[i] = value;
    }
};
using TheStack = RegStack<int, 2048>;

int Pull(TheStack* stack, const std::string& p)
{
    int value = INT_MAX;
    if (p[0] >= '0' && p[0] <= '9' || p[0] == '-')
    {
        value = std::stoi(p);
#ifdef LOG
        std::cout << value;
#endif // LOG
    }
    else if (p[0] == '[')
    {
        size_t n = std::stoull(p.substr(1, p.find(']')));
#ifdef LOG
        std::cout << '[' << n << ']';
#endif // LOG

        value = stack->Get(n);
#ifdef LOG
        std::cout << "(" << value << ')';
#endif // LOG
    }
    return value;
}

void Store(TheStack* stack, const std::string& p, int value)
{
    if (p[0] == '[')
    {
        size_t n = std::stoull(p.substr(1, p.find(']')));
#ifdef LOG
        std::cout << '[' << n << "]{";
#endif // LOG

        stack->Set(n, value);
#ifdef LOG
        std::cout << stack->Get(n) << '}';
#endif // LOG
    }
}

int Eval(const char* file)
{
    std::ifstream source(file);

    TheStack registers;
    int pVal[8];
    std::string pStr;

    std::string line;
    while (std::getline(source, line))
    {
        std::istringstream lineStream(line);

        std::string command;
        lineStream >> command;

        Instruction ins = (Instruction)CharsToInt(command.c_str());

        switch (ins)
        {
        case MOV:
#ifdef LOG
            std::cout << "Moving ";
#endif // LOG

            lineStream >> pStr;
            pVal[0] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " to ";
#endif // LOG

            lineStream >> pStr;
            Store(&registers, pStr, pVal[0]);

            break;

        case MVN:
#ifdef LOG
            std::cout << "Moving inverted ";
#endif // LOG
            // Todo
            break;

        case ADD:
#ifdef LOG
            std::cout << "Adding ";
#endif // LOG

            lineStream >> pStr;
            pVal[0] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " to ";
#endif // LOG

            lineStream >> pStr;
            pVal[1] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " and storing result at ";
#endif // LOG

            lineStream >> pStr;
            Store(&registers, pStr, pVal[0] + pVal[1]);

            break;

        case ADC:
#ifdef LOG
            std::cout << "Adding and carrying ";
#endif // LOG
            // Todo
            break;

        case SUB:
#ifdef LOG
            std::cout << "Subtracting ";
#endif // LOG

            lineStream >> pStr;
            pVal[0] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " from ";
#endif // LOG

            lineStream >> pStr;
            pVal[1] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " and storing result at ";
#endif // LOG

            lineStream >> pStr;
            Store(&registers, pStr, pVal[1] - pVal[0]);

            break;

        case SBC:
#ifdef LOG
            std::cout << "Subtracting and carrying ";
#endif // LOG
            // Todo
            break;

        case RSB:
#ifdef LOG
            std::cout << "Reverse subtracting ";
#endif // LOG
            // Todo
            break;

        case RSC:
#ifdef LOG
            std::cout << "Reverse subtract and carrying ";
#endif // LOG
            // Todo
            break;

        case AND:
#ifdef LOG
            std::cout << "Performing bitwise-and with ";
#endif // LOG

            lineStream >> pStr;
            pVal[0] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " against ";
#endif // LOG

            lineStream >> pStr;
            pVal[1] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " and storing result at ";
#endif // LOG

            lineStream >> pStr;
            Store(&registers, pStr, pVal[0] & pVal[1]);

            break;

        case ORR:
#ifdef LOG
            std::cout << "Performing bitwise-or with ";
#endif // LOG

            lineStream >> pStr;
            pVal[0] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " against ";
#endif // LOG

            lineStream >> pStr;
            pVal[1] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " and storing result at ";
#endif // LOG

            lineStream >> pStr;
            Store(&registers, pStr, pVal[0] | pVal[1]);

            break;

        case EOR:
#ifdef LOG
            std::cout << "Performing bitwise-xor with ";
#endif // LOG

            lineStream >> pStr;
            pVal[0] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " against ";
#endif // LOG

            lineStream >> pStr;
            pVal[1] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " and storing result at ";
#endif // LOG

            lineStream >> pStr;
            Store(&registers, pStr, pVal[0] ^ pVal[1]);

            break;

        case BIC:
#ifdef LOG
            std::cout << "Performing bitclear on ";
#endif // LOG
            // Todo
            break;

        case CMP:
#ifdef LOG
            std::cout << "Comparing ";
#endif // LOG
            // Todo
            break;

        case CMN:
#ifdef LOG
            std::cout << "Comparing negative ";
#endif // LOG
            // Todo
            break;

        case TST:
#ifdef LOG
            std::cout << "Testing for bit ";
#endif // LOG
            // Todo
            break;

        case TEQ:
#ifdef LOG
            std::cout << "Testing equality of ";
#endif // LOG

            lineStream >> pStr;
            pVal[0] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " against ";
#endif // LOG

            lineStream >> pStr;
            pVal[1] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " and storing result at ";
#endif // LOG

            lineStream >> pStr;
            Store(&registers, pStr, pVal[0] == pVal[1]);

            break;

        case MUL:
#ifdef LOG
            std::cout << "Multiplying ";
#endif // LOG

            lineStream >> pStr;
            pVal[0] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " by ";
#endif // LOG

            lineStream >> pStr;
            pVal[1] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " and storing result at ";
#endif // LOG

            lineStream >> pStr;
            Store(&registers, pStr, pVal[0] * pVal[1]);

            break;

        case MLA:
#ifdef LOG
            std::cout << "Multiplying and accumulating ";
#endif // LOG

            lineStream >> pStr;
            pVal[0] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " with ";
#endif // LOG

            lineStream >> pStr;
            pVal[1] = Pull(&registers, pStr);
#ifdef LOG
            std::cout << " and storing result at ";
#endif // LOG

            lineStream >> pStr;
            Store(&registers, pStr, Pull(&registers, pStr) + pVal[0] * pVal[1]);

            break;

        case SAY:
#ifdef LOG
            std::cout << "Printing ";
#endif // LOG
            lineStream >> pStr;
            Pull(&registers, pStr);
            break;

        default:
#ifdef LOG
            std::cout << "Unknown";
#endif // LOG
            goto Exit;
        }
#ifdef LOG
        std::cout << ".\n";
#endif // LOG
    }

Exit:
    source.close();
    return 0;
}

void Gen(const char* file, size_t len)
{
    std::ofstream source(file);
    source.clear();

    std::vector<size_t> initd;

    auto RandomReg = [&source]() {
        int n = rand() % 2048;
        source << '[' << n << ']';
        return n;
    };
    auto RandomIReg = [&source, &initd]() {
        int n = initd[rand() % initd.size()];
        source << '[' << n << ']';
        return n;
    };
    auto RandomLit = [&source]() {
        int n = rand() - (RAND_MAX / 2);
        source << n;
        return n;
    };
    auto RandomRoL = [&source, &RandomReg, &RandomLit]() {
        if (rand() & 1) return RandomReg();
        else return RandomLit();
    };
    auto RandomIRoL = [&source, &initd, &RandomIReg, &RandomLit]() {
        if (!initd.empty() && rand() & 1) return RandomIReg();
        else return RandomLit();
    };

    static const char* ins[] = {
        "say",
        "mov",

        "add",
        "sub",

        "and",
        "orr",
        "eor",

        "teq",

        "mul",
        "mla",
    };

    for (size_t i = 0; i < len; ++i)
    {
        size_t pic = rand() % _countof(ins);
        source << ins[pic] << ' ';

        if (pic == 0) // Say has 1 parameter
        {
            RandomIRoL();
        }
        else if (pic == 1) // Mov has 2 parameters
        {
            RandomIRoL();
            source << ' ';
            initd.push_back(RandomReg());
        }
        else // Everything else has 3 parameters
        {
            RandomIRoL();
            source << ' ';
            RandomIRoL();
            source << ' ';
            initd.push_back(RandomReg());
        }

        source << '\n';
    }

    source.close();
}

void Score()
{

}

int main()
{
    srand(clock()); // Set the random seed to the clock cycle of the CPU
    // (Makes randomness not repeat between program instances)

    const char* filename = "code.fasm";
    for (size_t i = 0; i < 10; ++i)
    {
        Gen(filename, 10); // Generate 10 lines of code
        Eval(filename); // Evaluate the genereated code
#ifdef LOG
        std::cout << '\n';
#endif // LOG
    }

    return 0;
}