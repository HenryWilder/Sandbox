#include <unordered_map>
#include <map>
#include <cstdarg>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "Functions2.h"
#include "Interpreter2.h"

#include "Debugging.h"

void Log(const char* fmt, ...)
{
    FILE* file;
    fopen_s(&file, "log.txt", (fmt ? "a+" : "w"));

    if (file)
    {
        if (fmt)
        {
            va_list args;
            va_start(args, fmt);
            vfprintf(file, fmt, args);
            va_end(args);
            va_start(args, fmt);
            printf(fmt, args);
            va_end(args);
        }

        fclose(file);
    }
}

constexpr int sizeof_str(const char str[8])
{
    if (!str) return -1;
    else if (!str[0]) return 0;
    else if (!str[1]) return 1;
    else if (!str[2]) return 2;
    else if (!str[3]) return 3;
    else if (!str[4]) return 4;
    else if (!str[5]) return 5;
    else if (!str[6]) return 6;
    else if (!str[7]) return 7;
    else return 8;
}
constexpr uint64_t Pack_Str(const char str[8])
{
    switch (sizeof_str(str))
    {
    default: return 0ull;
    case 1: return (0ull |
                    ((uint64_t)str[0]));
    case 2: return (0ull |
                    ((uint64_t)str[0]) |
                    ((uint64_t)str[1] << 010));
    case 3: return (0ull |
                    ((uint64_t)str[0]) |
                    ((uint64_t)str[1] << 010) |
                    ((uint64_t)str[2] << 020));
    case 4: return (0ull |
                    ((uint64_t)str[0]) |
                    ((uint64_t)str[1] << 010) |
                    ((uint64_t)str[2] << 020) |
                    ((uint64_t)str[3] << 030));
    case 5: return (0ull |
                    ((uint64_t)str[0]) |
                    ((uint64_t)str[1] << 010) |
                    ((uint64_t)str[2] << 020) |
                    ((uint64_t)str[3] << 030) |
                    ((uint64_t)str[4] << 040));
    case 6: return (0ull |
                    ((uint64_t)str[0]) |
                    ((uint64_t)str[1] << 010) |
                    ((uint64_t)str[2] << 020) |
                    ((uint64_t)str[3] << 030) |
                    ((uint64_t)str[4] << 040) |
                    ((uint64_t)str[5] << 050));
    case 7: return (0ull |
                    ((uint64_t)str[0]) |
                    ((uint64_t)str[1] << 010) |
                    ((uint64_t)str[2] << 020) |
                    ((uint64_t)str[3] << 030) |
                    ((uint64_t)str[4] << 040) |
                    ((uint64_t)str[5] << 050) |
                    ((uint64_t)str[6] << 060));
    case 8: return (0ull |
                    ((uint64_t)str[0]) |
                    ((uint64_t)str[1] << 010) |
                    ((uint64_t)str[2] << 020) |
                    ((uint64_t)str[3] << 030) |
                    ((uint64_t)str[4] << 040) |
                    ((uint64_t)str[5] << 050) |
                    ((uint64_t)str[6] << 060) |
                    ((uint64_t)str[7] << 070));
    }
}

enum class Type : char
{
    Int = 'i',
    Flt = 'f',
};

enum class Instruction : uint64_t
{
    // &n, n
    set = Pack_Str("set"),

    // &n, n, n
    Add = Pack_Str("add"),
    Sub = Pack_Str("sub"),
    Mul = Pack_Str("mul"),
    Div = Pack_Str("div"),
    Mod = Pack_Str("mod"),

    // s
    var = Pack_Str("var"),
    jmp = Pack_Str("jmp"),
    lbl = Pack_Str("lbl"),

    PrintS = Pack_Str("PrintS"), // Print string
    PrintV = Pack_Str("PrintV"), // Print value
    Open = Pack_Str("Open"),

    // { [s:lbl], [s:lbl], ... }
    Prompt = Pack_Str("Prompt"),
};

Instruction Tokenize(const char token[8])
{
    return (Instruction)(Pack_Str(token));
}

class VarStack
{
private:
    char mem[2048]; // Where the values are stored
    std::map<std::string, short> vars; // name -> position
    short top = 0;  // Current top offset of stack (where new vars should be pushed to/popped from)

public:
    bool pull(std::string& name, int* dest)
    {
        auto it = vars.find(name);
        if (it != vars.end())
        {
            short data = it->second;
            *dest = *(int*)(mem + data);
            return true;
        }
        else return false;
    }

    bool valid(std::string& name)
    {
        return (vars.find(name) != vars.end());
    }

    void push(std::string& name)
    {
        vars[name] = top;
        top += sizeof(int);
    }

    void push(std::string& name, int value)
    {
        size_t offset;
        {
            auto it = vars.find(name);
            if (it != vars.end()) // Push value to existing
            {
                offset = it->second;
            }
            else // Push new
            {
                vars[name] = offset = top;
                top += sizeof(int);
            }
        }
        *(int*)(mem + offset) = value;
    }
    
    bool pop()
    {
        if (vars.empty()) return false;
        else
        {
            auto it = (--vars.end());
            top -= sizeof(int);
            vars.erase(it);
            return true;
        }
    }
};

bool nParam(std::ifstream& stream, VarStack* stack, int* dest)
{
    std::string symbol;
    std::getline(stream, symbol, ' '); // I don't want '-' getting delimited

    if (symbol.find_first_of("0123456789.+-") == 0ull) // The first character is a numeric character
    {
        // Literal
        *(int*)dest = std::stoi(symbol);
        Log("Read literal integer value %i\n", *(int*)dest);
        return true;
    }
    else
    {
        if (stack->pull(symbol, dest))
        {
            Log("Read stored value %i\n", *(int*)dest);
            return true;
        }
        else
        {
            Log("Could not read value!\n");
            return false;
        }
    }
}

bool sParam(std::ifstream& stream, VarStack* stack, std::string* dest)
{
    stream.ignore(64, '\"');
    if (std::getline(stream, *dest, '\"'))
    {
        Log("Read string parameter \" %s \"\n", dest->c_str());
        return true;
    }
    else
    {
        Log("Could not locate string parameter.\n");
        return false;
    }
}

// Retrieves a "variable reference" (the name of a variable)
bool vParam(std::ifstream& stream, VarStack* stack, std::string* dest)
{
    if (std::getline(stream, *dest, ' '))
    {
        return stack->valid(*dest);
    }
    else return false;
}

constexpr int g_thisVersion = 0;


void Interpret(const char* file)
{
    size_t start;
#if 0
    // Version guard
    {
        int version;
        FILE* _file;
        fopen_s(&_file, file, "r");
        fscanf_s(_file, "version %i", &version);
        start = ftell(_file);
        fclose(_file);
        if (version != g_thisVersion) return; // TODO: Handle backwards compatibility
    }
#endif

    std::unordered_map<std::string, std::streampos> labels; // TODO: Preliminary readthrough of document to find label positions (with tellg!)
    

    std::ifstream code(file);
    {
        start = code.tellg();
        std::string token;
        while (code.good())
        {
            code >> token;
            if (token == "lbl")
            {
                std::string name;
                code >> name;
                labels.insert({ name, code.tellg() });
            }
        }
    }

    VarStack rsp; // stack

    code.seekg(start);

#define vParam(x) vParam(code, &rsp, x)
#define nParam(x) nParam(code, &rsp, x)
#define sParam(x) sParam(code, &rsp, x)

    while (!code.eof())
    {
        std::string tkn; // Token
        code >> tkn;
        // Obtain parameters
        Instruction ins = Tokenize(tkn.c_str());
        Log("Read token \"%s\"\n", tkn.c_str());
        switch (ins)
        {
        case Instruction::set:
        {
            std::string var;
            int val;
            vParam(&var);
            nParam(&val);
            rsp.push(var, val);
        }   break;

        case Instruction::Add: case Instruction::Sub:
        case Instruction::Div: case Instruction::Mul:
        case Instruction::Mod:
        {
            std::string o;
            int a, b;
            vParam(&o); // sParam() for variable name
            nParam(&a);
            nParam(&b);
            int result;
            // Evaluate
            switch (ins)
            {
            case Instruction::Add: result = a + b; Log("Added");       break;
            case Instruction::Sub: result = a - b; Log("Subtracted");  break;
            case Instruction::Mul: result = a * b; Log("Multiplied");  break;
            case Instruction::Div: result = a / b; Log("Divided");     break;
            case Instruction::Mod: result = a % b; Log("Moduloed");    break;
            }
            Log(" int of values a = %i and b = %i to produce the result %i\n", a, b, result);
            rsp.push(o, result);
        }   break;

        case Instruction::var:
        {
            std::string name;
            code >> name;
            rsp.push(name); break;
            Log("Creating variable named %s\n", name.c_str());
        }   break;
            
        case Instruction::Prompt:
        {
            code.ignore(16, '{');
            std::string args;
            std::unordered_map<std::string, std::string> inputJump;
            if (std::getline(code, args, '}'))
            {
                std::stringstream arglist(args);
                while (arglist.good())
                {
                    std::string arg;
                    std::string lbl;
                    arglist.ignore('\"');
                    std::getline(arglist, arg, '\"');
                    code.ignore(256, ':');
                    arglist >> lbl;
                    inputJump[arg] = lbl;
                }
            }
            std::string input;
            std::cin >> input;
            auto it = inputJump.find(input);
            if (it != inputJump.end())
            {
                auto ot = labels.find(it->second);
                if (ot != labels.end())
                {
                    code.seekg(ot->second);
                    Log("Jumped to label %s at position %s\n", ot->first, ot->second);
                }
            }
        }   break;

        case Instruction::lbl:  case Instruction::Open:
        case Instruction::jmp:  case Instruction::PrintS:
        {
            std::string str;
            sParam(&str);
            switch (ins)
            {
            case Instruction::PrintS: printf(str.c_str()); break;
            case Instruction::Open:   Open(str);           break;
            case Instruction::jmp:
            {
                auto it = labels.find(str);
                if (it != labels.end()) code.seekg(it->second);
                Log("Jumped to label %s at position %s\n", it->first, it->second);
            }   break;

            case Instruction::lbl: break;
            }
        }   break;

        case Instruction::PrintV:
        {
            int val;
            nParam(&val);
            printf("%i", val);
        }   break;

        default: goto Exit; break;
        }
    }

Exit:

    code.close();
}