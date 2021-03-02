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

    template<IDMap& map, bool foundBool = (sizeof(KEY) > (sizeof(Map_Iter_t) / 2))> struct ID_t {};

    // This is the version which uses the deadspace in the space between the key and iterator types to determine which it is
    template<IDMap& map> struct ID_t<IDMap& map, false> {
    public:
        ID_t() : it(NullIter()) {};
        ID_t(KEY _key) : it(LongKey(_key)) {};
        ID_t(Map_Iter_t _it) : it(_it) {};

        operator bool() {
            if (found()) {
                return map.valid(key);
            }
        }

    private:
        bool found() { return (IntIter(it) >> sizeof(KEY)); }

        union {
            KEY key;
            Map_Iter_t it;
        };
    };
    // This is the version which uses a member boolean to determine whether the union is a key or an iterator (useful for when there is a negligable size difference between key and iter)
    template<IDMap& map> struct ID_t<IDMap& map, true> {
    public:
        ID_t() : key(nPos), b_found(false) {};
        ID_t(KEY _key) : key(_key), b_found(false) {};
        ID_t(Map_Iter_t _it) : it(_it), b_found(true) {};

        operator bool() {
            return
        }

    private:
        bool found() { return b_found; }

        union {
            KEY key;
            Map_Iter_t it;
        };
        bool b_found;
    };


private:
    Map_t items;
    KeyList_t invalids;
};
template<typename KEY, typename VAL> constexpr KEY IDMap<KEY, VAL>::nPos;