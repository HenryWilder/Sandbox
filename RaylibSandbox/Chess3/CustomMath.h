#pragma once
namespace cstm
{
    template<typename T>
    struct Pair
    {
        const T& first;
        const T& second;
    };

    template<typename T>
    T Min(const T a, const T b)
    {
        if (a < b) return a;
        else return b;
    }
    template<typename T>
    T Max(const T a, const T b)
    {
        if (a > b) return a;
        else return b;
    }
    template<typename T>
    Pair<T> MinMax(const T a, const T b)
    {
        if (a < b) return { a,b };
        else return { b,a };
    }
    //template<typename T>
    //T Width(const T a, const T b)
    //{
    //
    //}

    //template<typename T>
    //T Sign(const T val) { return (val < (T)(0) ? (T)(-1) : (T)(1)); }
    template<typename T>
    T Abs(const T val) { return (val < (T)(0) ? -val : val); }
    //template<typename T>
    //Pair<T> SignAbs(const T val)
    //{
    //    if (val < (T)(0)) return { (T)(-1), -val };
    //    else return { (T)(1), val };
    //}

    //template<typename T>
    //T Round(const T val) {
    //    auto set = SignAbs(val);
    //    set
    //    return (set.first);
    //}
}