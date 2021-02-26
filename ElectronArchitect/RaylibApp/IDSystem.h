#pragma once
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <list>
#include "stl_extended.h"

template<typename KEY, typename VAL>
class IDMap
{
public:
    using Map_t     = std::unordered_map<KEY, VAL>;
    using KeyList_t = std::list<KEY>;

    static constexpr KEY nPos = (KEY)(0);

    bool valid(typename Map_t::const_iterator elem) { return !(elem == items.end()); }
    bool valid(KEY key) { return !((key == nPos) || (items.find(key) == items.end())); }

    typename Map_t::iterator find(KEY id) {
        auto it = items.find(id);
        if (it != items.end()) return it;
        else return typename Map_t::iterator{};
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

    auto begin() {
        return items.begin();
    }
    auto end() {
        return items.end();
    }
    auto size() {
        return items.size();
    }

    IDMap() {
        items[nPos] = VAL();
        invalids = {};
    };

private:
    Map_t items;
    KeyList_t invalids;
};
template<typename KEY, typename VAL> constexpr KEY IDMap<KEY,VAL>::nPos;
