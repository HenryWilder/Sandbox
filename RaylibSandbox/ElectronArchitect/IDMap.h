#pragma once
#include <list>
#include <unordered_map>

template<typename KEY, typename VAL>
class IDMap
{
public:
    using Map_t = std::unordered_map<KEY, VAL>;
    using Map_Iter_t = typename std::unordered_map<KEY, VAL>::iterator;
    using Map_CIter_t = typename std::unordered_map<KEY, VAL>::const_iterator;
    using KeyList_t = std::list<KEY>;

    static constexpr KEY nPos = (KEY)(0);

    IDMap() : items({ nPos, VAL() }) {};

    bool valid(Map_CIter_t elem) { return !(elem == items.end()); }
    bool valid(KEY key) { return !((key == nPos) || (items.find(key) == items.end())); }

    Map_Iter_t find(KEY id) {
        auto it = items.find(id);
        if (it != items.end()) return it;
        else return Map_Iter_t{};
    }

    KEY push(VAL val)
    {
        KEY id;

        if (!invalids.empty()) { // First check if there's an invalid key we can use
            id = invalids.front();
            invalids.pop_front();
        }
        else { // Create a new id if the list of invalid keys is empty
            id = (int)items.size();
        }

        if (id != nPos) items[id] = val;
        return id;
    }

    void erase(KEY id) {
        if (valid(id)) {
            items.erase(id);
            invalids.push_back(id);
        }
    }

    auto begin() { return items.begin(); }
    auto end() { return items.end(); }
    auto size() { return items.size(); }

    Map_Iter_t NullIter() { return reinterpret_cast<Map_Iter_t>(0ull); }
    Map_Iter_t LongKey(KEY key) { return (NullIter() | reinterpret_cast<Map_Iter_t>(key)); }
    Map_Iter_t IntIter(Map_Iter_t it) { return reinterpret_cast<unsigned long long>(it); }

private:
    Map_t items;
    KeyList_t invalids;
};
template<typename KEY, typename VAL> constexpr KEY IDMap<KEY, VAL>::nPos;