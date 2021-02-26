#pragma once
#include <vector>

template<typename T>
std::vector<T>& EraseMulti(std::vector<T>& vec, std::vector<unsigned int>& invalids)
{
    if (!invalids.empty())
    {
        unsigned int passed = 0;
        for (unsigned int i = invalids[0]; i < (vec.size() - invalids.size()); ++i)
        {
            if ((i + passed) == invalids[passed]) ++passed;
            vec[i] = vec[i + passed];
        }
        vec.resize(vec.size() - invalids.size());
    }
    return vec;
}