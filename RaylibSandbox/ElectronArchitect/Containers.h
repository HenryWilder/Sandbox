#pragma once
#include <raylib.h>
#include <list>
#include <unordered_map>
#include <array>
#include "Main.h"

struct Int3 {
    constexpr Int3() : x(), y(), z() {};
    constexpr Int3(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {};

    Int3(Vector3 pos)
        : x(Round(pos.x)), y(Round(pos.y)), z(Round(pos.z)) {};

    Int3(Vector2 pos, float layer)
        : x(Round(pos.x)), y(Round(pos.y)), z(Round(layer)) {};

    Int3(Vector2 pos, int layer)
        : x(Round(pos.x)), y(Round(pos.y)), z(layer) {};    

	int x, y, z;
};

using GateID_t = uint32_t;
struct Gate;
class IDMap_Gate
{
public:
    using Key_t         = GateID_t;
    using Value_t       = Gate*;

    using Map_t         = std::unordered_map<Key_t, Value_t>;
    using Map_Iter_t    = typename Map_t::iterator;
    using Map_CIter_t   = typename Map_t::const_iterator;
    using KeyList_t     = std::list<Key_t>;

    static constexpr Key_t nPos = (Key_t)(0);

    IDMap_Gate() {
        items[nPos] = nullptr;
    };

    bool valid(Map_CIter_t elem) { return !(elem == items.end()); }
    bool valid(Key_t key) { return !((key == nPos) || (items.find(key) == items.end())); }

    Map_Iter_t find(Key_t id) {
        auto it = items.find(id);
        if (it != items.end()) return it;
        else return Map_Iter_t{};
    }

    Key_t push(Value_t val) {
        Key_t id;

        // First check if there's an invalid key we can use
        if (!invalids.empty()) {
            id = invalids.front();
            invalids.pop_front();
        }
        // Create a new id if the list of invalid keys is empty
        else {
            id = (int)items.size();
        }

        if (id != nPos) items[id] = val;
        return id;
    }

    void erase(Key_t id) {
        if (valid(id)) {
            items.erase(id);
            invalids.push_back(id);
        }
    }

    auto begin() { return items.begin(); }
    auto end()   { return items.end();   }
    auto size()  { return items.size();  }

private:
    Map_t items;
    KeyList_t invalids;
};

using UnitID_t = uint16_t;
struct Unit;
class IDMap_Unit
{
public:
    using Key_t         = UnitID_t;
    using Value_t       = Unit*;

    using Map_t         = std::unordered_map<Key_t, Value_t>;
    using Map_Iter_t    = typename Map_t::iterator;
    using Map_CIter_t   = typename Map_t::const_iterator;
    using KeyList_t     = std::list<Key_t>;

    static constexpr Key_t nPos = (Key_t)(0);

    IDMap_Unit() {
        items[nPos] = nullptr;
    };

    bool valid(Map_CIter_t elem) { return !(elem == items.end()); }
    bool valid(Key_t key) { return !((key == nPos) || (items.find(key) == items.end())); }

    Map_Iter_t find(Key_t id) {
        auto it = items.find(id);
        if (it != items.end()) return it;
        else return Map_Iter_t{};
    }

    Key_t push(Value_t val) {
        Key_t id;

        // First check if there's an invalid key we can use
        if (!invalids.empty()) {
            id = invalids.front();
            invalids.pop_front();
        }
        // Create a new id if the list of invalid keys is empty
        else {
            id = (int)items.size();
        }

        if (id != nPos) items[id] = val;
        return id;
    }

    void erase(Key_t id) {
        if (valid(id)) {
            items.erase(id);
            invalids.push_back(id);
        }
    }

    auto begin() { return items.begin(); }
    auto end() { return items.end(); }
    auto size() { return items.size(); }

private:
    Map_t items;
    KeyList_t invalids;
};

// A connection for routing signals between objects of different types. Specifically gates/units.
struct ComponentID {
    ComponentID() : tag(State_Null), gateID(IDMap_Gate::nPos) {};
    ComponentID(GateID_t _gateID) : tag(State_GateID), gateID(_gateID) {};
    ComponentID(UnitID_t _unitID) : tag(State_UnitID), unitID{ _unitID } {};

    ComponentID& operator=(GateID_t _gateID) {
        tag = State_GateID;
        gateID = _gateID;
    };
    ComponentID& operator=(UnitID_t _unitID) {
        tag = State_UnitID;
        unitID = _unitID;
    };

    enum Tag {
        State_Null,   // Null
        State_GateID, // Gate
        State_UnitID, // Unit
    } tag;

    union {
        GateID_t gateID; // Gate
        UnitID_t unitID; // Unit
    };
};

struct VoxelArray {
public:
    VoxelArray() {
        xWidth = 010000;
        yHeight = 010000;
        zDepth = 000020;
        voxels.resize(Volume());
    }
    
    int XWidth()  { return xWidth; }
    int YHeight() { return yHeight; }
    int ZDepth()  { return zDepth; }
                                   
    int XMax() { return XWidth() / 2;       }
    int XMin() { return XMax() - XWidth();  }
    int YMax() { return YHeight() / 2;      }
    int YMin() { return YMax() - YHeight(); }
    int ZMax() { return ZDepth() / 2;       }
    int ZMin() { return ZMax() - ZDepth();  }

    size_t XYArea() { return XWidth() * YHeight(); }
    size_t Volume() { return XYArea() * ZDepth();  }

    bool InRange(Int3 pos) {
        return (pos.x >= XMin() && pos.x <= XMax() &&
                pos.y >= YMin() && pos.y <= YMax() &&
                pos.z >= ZMin() && pos.z <= ZMax());
    }
    size_t Index(Int3 pos) {
        Int3 pos1(pos.x + XMax(), pos.y + YMax(), pos.z + ZMax());
        return (pos1.z * XYArea()) + (pos1.y * XWidth()) + pos1.x;
    }

    ComponentID& ID_At(Int3 pos) {
        return voxels[Index(pos)];
    }

    template<ComponentID::Tag what>
    Gate* Get(Int3 at) {
        if (InRange(at)) {
            const ComponentID& id = ID_At(at);
            if (id.tag == what)
                return gates.find(id.gateID)->second;
        }
        return nullptr;
    }

    Unit* UnitAt(Int3 pos) {
        if (InRange(pos)) {
            const ComponentID& id = ID_At(pos);
            if (id.tag == ComponentID::State_UnitID)
                return units.find(id.unitID)->second;
        }
        return nullptr;
    }

    GateID_t push(IDMap_Gate::Value_t _gate, Int3 at) {
        auto id = gates.push(_gate);
        ComponentID(id)
    }
    UnitID_t push(IDMap_Unit::Value_t _unit, Int3 at) {
        return units.push(_unit);
    }
    void erase(GateID_t _gateID) {
        gates.erase(_gateID);
    }
    void erase(UnitID_t _unitID) {
        units.erase(_unitID);
    }

private:
    int xWidth;
    int yHeight;
    int zDepth;
    IDMap_Gate gates;
    IDMap_Unit units;
    std::vector<ComponentID> voxels;
};