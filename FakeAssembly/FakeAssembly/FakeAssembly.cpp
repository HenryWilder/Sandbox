// FakeAssembly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

enum class Instruction
{
    mov,    // Move
    mvn,    // Move inverted

    add,    // Add
    adc,    // Add and carry
    sub,    // Subtract
    sbc,    // Subtract and carry
    rsb,    // Reverse subtract (n-Rn instead of Rn-n)
    rsc,    // Reverse subtract and carry

    AND,    // Bitwise AND
    ORR,    // Bitwise OR
    EOR,    // Bitwise XOR
    BIC,    // Bitclear (a & ~b)

    cmp,    // Compare
    cmn,    // Compare negative
    tst,    // Test a bit in a 32-bit value
    teq,    // Test for equality

    mul,    // Multiply
    mla,    // Multiply and accumulate
};
int Parameters(Instruction of)
{
    switch (of)
    {
    case Instruction::mov: return 2;
    case Instruction::mvn: return 2;

    case Instruction::add: return 3;
    case Instruction::adc: return 3;
    case Instruction::sub: return 3;
    case Instruction::sbc: return 3;
    case Instruction::rsb: return 3;
    case Instruction::rsc: return 3;

    case Instruction::AND: return 3;
    case Instruction::ORR: return 3;
    case Instruction::EOR: return 3;
    case Instruction::BIC: return 3;

    case Instruction::cmp: return 2;
    case Instruction::cmn: return 2;
    case Instruction::tst: return 2;
    case Instruction::teq: return 2;

    case Instruction::mul: return 3;
    case Instruction::mla: return 4;

    default: return 0;
    }
}

std::unordered_map<std::string, Instruction> g_cmd
{
    { "mov", Instruction::mov },
    { "mvn", Instruction::mvn },

    { "add", Instruction::add },
    { "adc", Instruction::adc },
    { "sub", Instruction::sub },
    { "sbc", Instruction::sbc },
    { "rsb", Instruction::rsb },
    { "rsc", Instruction::rsc },
   
    { "and", Instruction::AND },
    { "orr", Instruction::ORR },
    { "eor", Instruction::EOR },
    { "bic", Instruction::BIC },
  
    { "cmp", Instruction::cmp },
    { "cmn", Instruction::cmn },
    { "tst", Instruction::tst },
    { "teq", Instruction::teq },
  
    { "mul", Instruction::mul },
    { "mla", Instruction::mla },
};

typedef void (*move)(int& Rd, int n);
typedef void (*math)(int& Rd, int& Rn, int n);
typedef void (*bitw)(int& Rd, int& Rn, int n);
typedef void (*comp)(int& Rd, int n);
typedef void (*mult)(int& Rd, int& Rm, int& Rs);
typedef void (*mula)(int& Rd, int& Rm, int& Rs, int& Rn);

int main()
{
    std::ifstream source("code");

    int registers[2048];

    std::string line;
    while (std::getline(source, line))
    {
        std::istringstream lineStream(line);

        std::string command;
        lineStream >> command;

    }

    source.close();
}