// Type your code here, or load an example.
#include <stdio.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <array>
#include <vector>
#include <fstream>
#if 0

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
#endif

volatile bool g_stop;

#include <vector>
#include <set>
#include <unordered_set>
size_t g_ops = 0ull;
size_t g_opsOF = 0ull; // Overflow

std::vector<size_t> g_vec;
std::set<size_t> g_set_o;
std::unordered_set<size_t> g_set_u;

void Waker()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    g_stop = true;
}
void Worker_Incrementer()
{
    size_t prev = 0ull;
    while (!g_stop)
    {
        prev = g_ops;
        ++g_ops;
        if (g_ops < prev) ++g_opsOF;
    }
}
void Worker_Vector()
{
    while (!g_stop)
    {
        g_vec.push_back(g_ops);
    }
}
void Worker_OrderedSet()
{
    while (!g_stop)
    {
        g_set_o.insert(g_ops);
    }
}
void Worker_UnorderedSet()
{
    while (!g_stop)
    {
        g_set_u.insert(g_ops);
    }
}

size_t g_searchlist[500000];
size_t g_finds_v = 0ull;
size_t g_finds_so = 0ull;
size_t g_finds_su = 0ull;

void Worker_Find_Vector()
{
    size_t i = 0ull;
    while (!g_stop)
    {
        size_t search = g_searchlist[i];
        for (size_t i = 0ull; i < g_vec.size(); ++i)
        {
            if (g_vec[i] == search) break;
        }
        ++g_finds_v;
        ++i;
        if (i >= _countof(g_searchlist)) i = 0ull;
    }
}
void Worker_Find_OrderedSet()
{
    size_t i = 0ull;
    while (!g_stop)
    {
        size_t search = g_searchlist[i];
        g_set_o.find(search);
        ++g_finds_so;
        ++i;
        if (i >= _countof(g_searchlist)) i = 0ull;
    }
}
void Worker_Find_UnorderedSet()
{
    size_t i = 0ull;
    while (!g_stop)
    {
        size_t search = g_searchlist[i];
        g_set_u.find(search);
        ++g_finds_su;
        ++i;
        if (i >= _countof(g_searchlist)) i = 0ull;
    }
}

size_t g_iters_v = 0ull;
size_t g_iters_so = 0ull;
size_t g_iters_su = 0ull;

void Worker_Iter_Vector()
{
    while (!g_stop)
    {
        for (size_t i : g_vec)
        {
            ++g_iters_v;
            if (g_stop) break;
        }
    }
}
void Worker_Iter_OrderedSet()
{
    while (!g_stop)
    {
        for (size_t i : g_set_o)
        {
            ++g_iters_so;
            if (!g_stop) break;
        }
    }
}
void Worker_Iter_UnorderedSet()
{
    while (!g_stop)
    {
        for (size_t i : g_set_u)
        {
            ++g_iters_su;
            if (!g_stop) break;
        };
    }
}

struct Format
{
    Format(bool isFmt, const char* src, size_t length)
    {
        b_fmt = isFmt;
        str = new char[length + 1];
        for (size_t i = 0; i < length; ++i)
        {
            str[i] = src[i];
        }
        str[length] = 0;
    }
    void Free()
    {
        delete[] str;
    }
    bool b_fmt;
    char* str;
};

void PrintArrayf(const char* elementfmt, void* arr, size_t elemSize, size_t arrSize)
{
    std::vector<Format> formats;

    // Find formatting
    {
        const char* lstart = elementfmt;
        size_t llength = 0;
        for (const char* c = elementfmt; *c; ++c)
        {
            if (*c == '%')
            {
                const char* fstart = c;
                size_t flength = 2;
                ++c;
                ++llength;
                if (*c != '%')
                {
                    Format lit(false, lstart, llength);
                    formats.push_back(lit);
                    while (*c && !(*c >= 'a' && *c <= 'z'))
                    {
                        ++c;
                        ++flength;
                    }
                    Format fmt(true, fstart, flength);
                    formats.push_back(fmt);
                    lstart = ++c;
                    llength = 1;
                }
            }
            else
            {
                ++llength;
            }
        }
        if (llength)
        {
            Format lit(false, lstart, llength);
            formats.push_back(lit);
        }
    }
    // Print
    {
        printf("{ ");
        for (size_t i = 0; i < arrSize; ++i)
        {
            const char* e = ((const char*)arr + (i * elemSize));
            size_t bytes = 0ull;
            for (const Format& fmt : formats)
            {
                if (fmt.b_fmt)
                {
                    const char* c = fmt.str;
                    while (*c) { ++c; }
                    --c;
                    switch (*c)
                    {
                    case 's': printf(fmt.str, *(const char**)(e));      e += sizeof(const char*);   break;
                    case 'c': printf(fmt.str, *(const char*)(e));       e += sizeof(char);          break;
                    case 'i': printf(fmt.str, *(const int*)(e));        e += sizeof(int);           break;
                    case 'u': printf(fmt.str, *(const unsigned*)(e));   e += sizeof(unsigned);      break;
                    case 'f': printf(fmt.str, *(const float*)(e));      e += sizeof(float);         break;
                    case 'p': printf(fmt.str, *(const void**)(e));      e += sizeof(void*);         break;
                    default:
                        break;
                    }
                }
                else
                {
                    printf(fmt.str);
                }
            }
            if (i + 1 < arrSize) printf(", ");
        }
        printf(" }");
    }
    // Cleanup
    {
        for (Format& fmt : formats)
        {
            fmt.Free();
        }
    }
}

struct StrVal
{
    const char* str;
    float val;
    StrVal* ptr;
};

class CstmVector
{
    size_t size;
    StrVal* arr;

public:
    CstmVector() : arr(nullptr), size(0ull) {}
    ~CstmVector() { Clear(); }

    void Reserve(size_t size)
    {
        if (arr)
        {
            StrVal* temp = arr;
            arr = new StrVal[size];
            memcpy(arr, temp, std::min(size * sizeof(StrVal), this->size * sizeof(StrVal)));
            delete[] temp;
        }
        else
        {
            arr = new StrVal[size];
        }
        this->size = size;
    }
    void Clear()
    {
        if (size) delete[] arr;
    }
    size_t Size()
    {
        return size;
    }

    StrVal* begin()
    {
        return arr;
    }
    StrVal* end()
    {
        return arr + size;
    }
};

#include "Header.h"
#include <type_traits>

template<typename T, bool B = std::is_integral<T>::value>
class Foo
{
public:
    Foo(T value) : m_value(value) {}

private:
    T m_value;
};
template<typename T>
class Foo<T,false>
{
public:
    Foo(T value) : m_value(value) {}
public:
    const T& GetValue() const
    {
        return m_value;
    }
private:
    T m_value;
};
template<typename T>
class Foo<T,true>
{
public:
    Foo(T value) : m_value(value) {}
public:
    void SetValue(const T& newVal)
    {
        m_value = newVal;
    }
private:
    T m_value;
};

template<typename T, bool Is_Integral = std::is_integral<T>::value, bool VALID = std::is_arithmetic<T>::value>
class RangeController {};

template<typename T, bool B>
class RangeController<T, B, false>
{
public:
    RangeController()
    {
        static_assert(false);
    }
};

template<typename T>
class RangeController<T, false, true>
{
public:
    RangeController() :
        m_value(),
        m_min(), m_max()
    {}
    RangeController(T* value, T min, T max) :
        m_value(value),
        m_min(min), m_max(max)
    {}

    T GetValue() const
    {
        return *m_value;
    }
    T GetRangeMin() const
    {
        return m_min;
    }
    T GetRangeMax() const
    {
        return m_max;
    }

    void SetValue(float newValue)
    {
        *m_value = Clamp(newValue, m_min, m_max);
    }
    void SetVariable(float* newVar)
    {
        m_value = newVar;
    }
    void SetValueRange(float min, float max)
    {
        m_min = min;
        m_max = max;
    }

protected:
    T* m_value;
    T m_min, m_max;
};
template<typename T>
class RangeController<T, true, true>
{
public:
    RangeController() :
        m_value(),
        m_min(), m_max()
    {}
    RangeController(T* value, T min, T max) :
        m_value(value),
        m_min(min), m_max(max)
    {}

    T GetValue() const
    {
        return *m_value;
    }
    T GetRangeMin() const
    {
        return m_min;
    }
    T GetRangeMax() const
    {
        return m_max;
    }

    void SetValue(float newValue)
    {
        *m_value = Clamp(newValue, m_min, m_max);
    }
    void SetVariable(float* newVar)
    {
        m_value = newVar;
    }
    void SetValueRange(float min, float max)
    {
        m_min = min;
        m_max = max;
    }

protected:
    T* m_value;
    T m_min, m_max;
};

int main()
{
    
    {
        RangeController<int> foo;
    }
    return 0;
    {
        Foo<float> a(2.4f);
        Foo<int> b(3);
        b.SetValue(a.GetValue());
    }
    return 0;
    {
        SomeTestFunction();
    }
    return 0;
    {
        std::vector<int> numbers{ 1,2,3,4,5,8 };
        PrintArrayf("%#03i", &numbers[0], sizeof(numbers[0]), numbers.size());
        printf("\n");
        StrVal a{ "apple", 0.25f, nullptr };
        a.ptr = &a;
        StrVal b{ "orange", 1.8f, nullptr };
        b.ptr = &b;
        std::vector<StrVal> vec{ a, b };
        PrintArrayf("%s : %f (%p)", &vec[0], sizeof(StrVal), vec.size());
        printf("\n");
    }
    return 0;
    {
        {
            g_ops = 0ull;
            std::thread waker(Waker);
            std::thread workerI(Worker_Incrementer);
            std::thread workerV(Worker_Vector);
            std::thread workerSo(Worker_OrderedSet);
            std::thread workerSu(Worker_UnorderedSet);
            waker.join();
            workerV.join();
            workerSo.join();
            workerSu.join();
            workerI.join();
        }
        printf("~~~In 1 second~~~\n");
        printf("\nInsertions:\n");
        printf("Size_t increments:  %#16zu (+ %zu * %zu overflow)\n", g_ops, g_opsOF, SIZE_MAX);
        printf("Vector:             %#16zu\n", g_vec.size());
        printf("Ordered set:        %#16zu\n", g_set_o.size());
        printf("Unordered set:      %#16zu\n", g_set_u.size());

        for (size_t i = 0ull; i < 500000; ++i)
        {
            g_searchlist[i] = rand();
        }
        g_vec.clear();
        for (size_t i = 0ull; i < 500000; ++i)
        {
            g_vec.push_back(i);
        }
        g_set_o.clear();
        for (size_t i = 0ull; i < 500000; ++i)
        {
            g_set_o.insert(i);
        }
        g_set_u.clear();
        for (size_t i = 0ll; i < 500000; ++i)
        {
            g_set_u.insert(i);
        }
        {
            g_stop = false;
            g_ops = 0ull;
            std::thread waker(Waker);
            std::thread workerI(Worker_Incrementer);
            std::thread workerV(Worker_Find_Vector);
            std::thread workerSo(Worker_Find_OrderedSet);
            std::thread workerSu(Worker_Find_UnorderedSet);
            waker.join();
            workerV.join();
            workerSo.join();
            workerSu.join();
            workerI.join();
        }
        printf("\nSearches:\n");
        printf("Size_t increments:  %#16zu (+ %zu * %zu overflow)\n", g_ops, g_opsOF, SIZE_MAX);
        printf("Vector:             %#16zu\n", g_finds_v);
        printf("Ordered set:        %#16zu\n", g_finds_so);
        printf("Unordered set:      %#16zu\n", g_finds_su);

        {
            g_stop = false;
            g_ops = 0ull;
            std::thread waker(Waker);
            std::thread workerI(Worker_Incrementer);
            std::thread workerV(Worker_Iter_Vector);
            std::thread workerSo(Worker_Iter_OrderedSet);
            std::thread workerSu(Worker_Iter_UnorderedSet);
            waker.join();
            workerV.join();
            workerSo.join();
            workerSu.join();
            workerI.join();
        }
        printf("\nIterations:\n");
        printf("Size_t increments:  %#16zu (+ %zu * %zu overflow)\n", g_ops, g_opsOF, SIZE_MAX);
        printf("Vector:             %#16zu\n", g_iters_v);
        printf("Ordered set:        %#16zu\n", g_iters_so);
        printf("Unordered set:      %#16zu\n", g_iters_su);
    }
    return 0;
    {
        int a = 1234;
        int b = 5678;
        int& ar = a;
        int& br = b;
        std::swap(ar,br);
        printf("a=%i\nb=%i\nar=%i\nbr=%i\n",a,b,ar,br);
    }
    return 0;
#if 0
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
#endif
}