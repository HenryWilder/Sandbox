#pragma once

template<typename Type>
struct SmrtPtr
{
    SmrtPtr() : ptr(nullptr), valid(false) {};
    template<typename...args> // Construction arguments
    SmrtPtr(args...) : ptr(new Type(args...)), valid(true) {};
    ~SmrtPtr() { delete ptr; valid = false; }

    Type* ptr;
    bool valid;
};

template<typename Type>
struct SmrtPtr
{
    SmrtPtr() : ptrs(nullptr), size(0), valid(false) {};
    SmrtPtr(size_t _size) : ptrs(new Type()[_size]), size(_size), valid(true) {};
    ~SmrtPtr() { delete[] ptrs; size = 0; valid = false; }

    Type* ptrs;
    size_t size;
    bool valid;
};
