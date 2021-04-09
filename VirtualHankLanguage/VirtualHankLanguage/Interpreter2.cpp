#include <unordered_map>
#include <map>
#include <cstdarg>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "Functions2.h"
#include "Interpreter2.h"

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
        }

        fclose(file);
    }
}

constexpr int sizeof_str(const char str[8])
{
    if (!str[0]) return 0;
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
    Str = 's',
};

enum class Instruction : uint64_t
{
    // t, s
    var = Pack_Str("var"),

    // &i, i
    iSet = Pack_Str("iSet"),

    // &f, f
    fSet = Pack_Str("fSet"),

    // &s, s
    sSet = Pack_Str("sSet"),

    // &i, i, i
    iAdd = Pack_Str("iAdd"),
    iSub = Pack_Str("iSub"),
    iMul = Pack_Str("iMul"),
    iDiv = Pack_Str("iDiv"),
    iMod = Pack_Str("iMod"),

    // &f, f, f
    fAdd = Pack_Str("fAdd"),
    fSub = Pack_Str("fSub"),
    fMul = Pack_Str("fMul"),
    fDiv = Pack_Str("fDiv"),

    // s
    Print = Pack_Str("Print"),
    Open = Pack_Str("Open"),
    jmp  = Pack_Str("jmp"),
    lbl  = Pack_Str("lbl"),

    // { [s:lbl], [s:lbl], ... }
    Prompt = Pack_Str("Prompt"),
};

Instruction Tokenize(const char token[8])
{
    return (Instruction)(Pack_Str(token));
}

// Tells how to interpret the memory
struct VarData
{
    Type ty;        // How to interpret the var
    short pt;       // Var offset from stack
    size_t size;    // How many bytes the block is in size
};
class VarStack
{
private:
    char mem[2048]; // Where the values are stored
    std::map<std::string, VarData> vars; // name -> { type, position }
    short top = 0;  // Current top offset of stack (where new vars should be pushed to/popped from)

public:
    bool pull(std::string& name, void* dest)
    {
        auto it = vars.find(name);
        if (it != vars.end())
        {
            VarData data = it->second;
            for (size_t i = 0; i < data.size; i++)
            {
                ((char*)dest)[i] = mem[data.pt + i];
            }
            return true;
        }
        else return false;
    }
    void push(std::string& name, Type ty, size_t bytes, void* src = nullptr)
    {
        size_t offset;
        {
            auto it = vars.find(name);
            if (it != vars.end()) // Push value to existing
            {
                bytes = std::min(bytes, it->second.size);
                offset = it->second.pt;
            }
            else // Push new
            {
                vars.insert({ name, { ty, top, bytes } });
                offset = top;
                top += bytes;
            }
        }
        if (src)
        {
            for (size_t i = 0; i < bytes; i++)
            {
                mem[offset + i] = ((char*)src)[i];
            }
        }
    }
    bool pop()
    {
        if (vars.empty()) return false;
        else
        {
            auto it = (--vars.end());
            top -= it->second.size;
            vars.erase(it);
            return true;
        }
    }
};

bool nParam(std::ifstream& stream, VarStack* stack, void* dest, Type ty)
{
    std::string symbol;
    std::getline(stream, symbol, ' '); // I don't want '-' getting delimited

    if (symbol.find_first_of("0123456789.+-") == 0ull) // The first character is a numeric character
    {
        // Literal
        switch (ty)
        {
        case Type::Int: *(int*)dest   = std::stoi(symbol); Log("Read literal integer value %i\n", *(int*)dest); break;
        case Type::Flt: *(float*)dest = std::stof(symbol); Log("Read literal float value %i\n", *(float*)dest); break;
        default: Log("ERROR: Undefined type!\n"); return false;
        }
        return true;
    }
    else
    {
        if (stack->pull(symbol, dest))
        {
            Log("Read stored value %i OR %f\n", *(int*)dest, *(float*)dest);
            return true;
        }
        else
        {
            Log("Read stored value %i OR %f\n", *(int*)dest, *(float*)dest);
            return false;
        }
    }
}

bool sParam(std::ifstream& stream, VarStack* stack, std::string& dest)
{
    stream.ignore(64, '\"');
    if (std::getline(stream, dest, '\"'))
    {
        Log("Read string parameter \" %s \"\n", dest.c_str());
        return true;
    }
    else
    {
        Log("Could not locate string parameter.\n");
        return false;
    }
}

constexpr int g_thisVersion = 0;

void Interpret(const char* file)
{
    size_t start;
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

    std::unordered_map<std::string, std::streampos> labels; // TODO: Preliminary readthrough of document to find label positions (with tellg!)
    

    std::ifstream code(file);
    {
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

    while (code.good())
    {
        std::string tkn; // Token
        code >> tkn;
        // Obtain parameters
        Instruction ins = Tokenize(tkn.c_str());
        Log("Read token \"%s\"\n", tkn.c_str());
        switch (ins)
        {
        case Instruction::iAdd: case Instruction::iSub:
        case Instruction::iDiv: case Instruction::iMul:
        case Instruction::iMod:
        {
            std::string o; int a, b;
            sParam(code, &rsp, o); // sParam() for variable name
            nParam(code, &rsp, &a, Type::Int);
            nParam(code, &rsp, &b, Type::Int);
            int result;
            // Evaluate
            switch (ins)
            {
            case Instruction::iAdd: result = a + b; Log("Added");       break;
            case Instruction::iSub: result = a - b; Log("Subtracted");  break;
            case Instruction::iMul: result = a * b; Log("Multiplied");  break;
            case Instruction::iDiv: result = a / b; Log("Divided");     break;
            case Instruction::iMod: result = a % b; Log("Moduloed");    break;
            }
            Log(" int of values a = %i and b = %i to produce the result %i\n", a, b, result);
            rsp.push(o, Type::Int, sizeof(int), &result);
        }   break;

        case Instruction::fAdd: case Instruction::fSub:
        case Instruction::fDiv: case Instruction::fMul:
        {
            std::string o; float a, b;
            sParam(code, &rsp, o); // sParam() for variable name
            nParam(code, &rsp, &a, Type::Flt);
            nParam(code, &rsp, &b, Type::Flt);
            float result;
            // Evaluate
            switch (ins)
            {
            case Instruction::fAdd: result = a + b; Log("Added");       break;
            case Instruction::fSub: result = a - b; Log("Subtracted");  break;
            case Instruction::fMul: result = a * b; Log("Multiplied");  break;
            case Instruction::fDiv: result = a / b; Log("Divided");     break;
            }
            Log(" floats of values a = %f and b = %f to produce the result %f\n", a, b, result);
            rsp.push(o, Type::Flt, sizeof(float), &result);
        }   break;

        case Instruction::var:
        {
            char ty;
            std::string name;
            code >> ty >> name;
            rsp.push(name, (Type)ty, 4);
            Log("Creating variable of type %c named %s\n", ty, name.c_str());
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
        case Instruction::jmp:  case Instruction::Print:
        {
            std::string str;
            sParam(code, &rsp, str);
            switch (ins)
            {
            case Instruction::Print: printf(str.c_str()); break;
            case Instruction::Open: Open(str); break;
            case Instruction::jmp:
            {
                auto it = labels.find(str);
                if (it != labels.end()) code.seekg(it->second);
                Log("Jumped to label %s at position %s\n", it->first, it->second);
            }   break;

            case Instruction::lbl: break;
            }
        }   break;
        default: goto Exit; break;
        }
    }

Exit:

    code.close();
}