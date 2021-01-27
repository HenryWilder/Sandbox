#pragma once
#include <type_traits>
#include <typeinfo>
#include <unordered_map>

template<class Type>
class Validator
{
public:
    Validator() : b_valid(false), m_payload() {};
    Validator(const Type& payload) : b_valid(true), m_payload(payload) {};

    bool IsValid() const { return b_valid; }
    void Invalidate() { b_valid = false; }

    Type& GetPayload_Reference() { return m_payload; }
    Type GetPayload_Copy() const { return m_payload; }

    Validator& operator=(const Type& src)
    {
        if (!b_valid)
        {
            m_payload = src;
            b_valid = true;
        }
        else
        {
            printf("Overwrite attempted on in-use memory! Please invalidate and wait for references to update before overwriting.\n");
        }
        return *this;
    }

private:
    bool b_valid;
    Type m_payload;
};

typedef uint_fast32_t ID_t; // In case I decide to make it different later, I can change it here and it'll change all instances of ID_t.
enum class IDFlags : ID_t
{
    NULLID = 1 << (std::numeric_limits<ID_t>::digits - 1),
};

bool IsIDValid(ID_t id);
