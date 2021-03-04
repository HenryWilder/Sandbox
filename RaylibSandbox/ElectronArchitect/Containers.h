#pragma once
#include <raylib.h>
#include <list>
#include <unordered_map>
#include <array>
#include <thread>
#include <atomic>
#include "Main.h"

struct Int3 {
    constexpr Int3() : x(), y(), z() {};
    constexpr Int3(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {};

    Int3(Vector3 pos);
    Int3(Vector2 pos, float layer);
    Int3(Vector2 pos, int layer);

    bool operator==(const Int3& b);
    bool operator!=(const Int3& b);
    Vector2 xy();
    Vector3 xyz();

	int x, y, z;
};

// The list of actual gates. This is what the map values are pointing at.
struct GateMemory {
public:
    using Type      = Gate;
    using ListType  = std::list<Type>;
    using Iter      = typename ListType::iterator;

    Iter push(const Type& _gate);
    Iter end();
    void erase(GateMemory::Iter at);

private:
    ListType items;
};

// The list of actual units. This is what the map values are pointing at.
struct UnitMemory {
public:
    using Type      = Unit;
    using ListType  = std::list<Type>;
    using Iter      = typename ListType::iterator;

    Iter push(const Type& _gate);
    Iter end();
    void erase(Iter at);

private:
    ListType items;
};

using GateID_t = uint32_t;
struct Gate;
class IDMap_Gate
{
public:
    using Key_t         = GateID_t;
    using Value_t       = GateMemory::Iter;

    using Map_t         = std::unordered_map<Key_t, Value_t>;
    using Map_Iter_t    = typename Map_t::iterator;
    using Map_CIter_t   = typename Map_t::const_iterator;
    using KeyList_t     = std::list<Key_t>;

    static constexpr Key_t nPos = (Key_t)(0);

    IDMap_Gate();

    bool valid(Map_CIter_t elem);
    bool valid(Key_t key);

    Map_Iter_t find(Key_t id);

    Key_t push(Value_t val);

    void erase(Key_t id);

    auto begin();
    auto end();
    auto size();

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
    using Value_t       = UnitMemory::Iter;

    using Map_t         = std::unordered_map<Key_t, Value_t>;
    using Map_Iter_t    = typename Map_t::iterator;
    using Map_CIter_t   = typename Map_t::const_iterator;
    using KeyList_t     = std::list<Key_t>;

    static constexpr Key_t nPos = (Key_t)(0);

    IDMap_Unit();

    bool valid(Map_CIter_t elem);
    bool valid(Key_t key);

    Map_Iter_t find(Key_t id);

    Key_t push(Value_t val);

    void erase(Key_t id);

    auto begin();
    auto end();
    auto size();

private:
    Map_t items;
    KeyList_t invalids;
};

struct ComponentID {
    ComponentID();
    ComponentID(GateID_t _gateID);
    ComponentID(UnitID_t _unitID);

    ComponentID& operator=(nullptr_t _null);
    ComponentID& operator=(GateID_t _gateID);
    ComponentID& operator=(UnitID_t _unitID);

    enum {
        Comp_NullID, // Null
        Comp_GateID, // Gate
        Comp_UnitID, // Unit
    } tag;

    union {
        GateID_t gateID; // Gate
        UnitID_t unitID; // Unit
    };
};

struct VoxelArray {
public:
    VoxelArray();
    VoxelArray(int _xWidth, int _yHeight, int _zDepth);

    VoxelArray& operator=(const VoxelArray&);
    
    int XWidth(); int YHeight(); int ZDepth();
                                   
    int XMax(); int XMin();
    int YMax(); int YMin();
    int ZMax(); int ZMin();

    size_t XYArea();
    size_t Volume();

    bool InRange(Int3 pos);
    size_t Index(Int3 pos);

    ComponentID& ID_At(Int3 pos);
    bool IsPointFree(Int3 pos);

    IDMap_Gate::Value_t GateAt(Int3 at);
    IDMap_Unit::Value_t UnitAt(Int3 at);

    GateID_t PushGate(const Gate& gate, Int3 at);
    UnitID_t PushUnit(const Unit& unit, Int3 at);

    void EraseAt(Int3 at);

    size_t ResizedMemory();

private:
    std::atomic_int64_t validVoxelCount;

    int xWidth, yHeight, zDepth;

    GateMemory gate_mem;
    UnitMemory unit_mem;

    IDMap_Gate gates;
    IDMap_Unit units;
public:
    std::vector<ComponentID> voxels;
};