#pragma once
#include <raylib.h>
#include <list>
#include <unordered_map>
#include <array>
#include <thread>
#include <atomic>
#include <stack>
#include "Main.h"

struct Int3 {
    Int3() : x(), y(), z() {};
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

struct IntRect {
    IntRect() : x1(), y1(), w(), h() {};
    constexpr IntRect(int _x, int _y, int _w, int _h) : x1(_x), y1(_y), w(_w), h(_h) {};

    IntRect(Rectangle rec);

    operator Rectangle();

    int x2(); // x + w
    int y2(); // y + h

    int x1, y1, w, h;
};
bool CheckInt3InsideIntRect(Int3 pt, IntRect rec);

class GameData {
public:

    using ID_t = uint32_t;

    class IDMap
    {
    public:
        using Key_t     = GameData::ID_t;
        using Value_t   = Component*;

        using Map_t         = std::unordered_map<Key_t, Value_t>;
        using Map_Iter_t    = typename Map_t::iterator;
        using Map_CIter_t   = typename Map_t::const_iterator;
        using KeyList_t     = std::list<Key_t>;

        static constexpr Key_t null_key = (Key_t)(0);

        IDMap();

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

    class EvalOrder {
    public:
        struct DepthLayer {
            std::vector<GameData::ID_t> componentIDs;
        };

        void EvaluateAll(GameData::IDMap& source);
        void Organize();

    private:
        std::vector<DepthLayer> order;
    };

    class TreeNode {
    public:
        using ElementType = ID_t;

        enum class Quad {
            TL, TR,
            BL, BR,
        };

        TreeNode() : coverage(), children{ nullptr, nullptr, nullptr, nullptr }, elements() {};
        TreeNode(IntRect _coverage) : coverage(_coverage), children{ nullptr, nullptr, nullptr, nullptr }, elements() {};

        int Navigate(int x, int y);

        std::vector<ElementType>& GetElements();

    private:
        IntRect coverage;
        std::array<TreeNode*, 4> children;
        std::vector<ElementType> elements;
    };

private:
    int xWidth, yHeight, zDepth;            // Size of the world (zDepth will usually be only a few layers)
    IDMap m_idSystem;                       // ID handler for components (turns ID numbers into component pointers)
    EvalOrder m_order;                      // Order of evaluation for components
    std::vector<TreeNode*> m_worldLayers;   // Quad-Tree nodes for quickly locating components on the screen

public:
    GameData();
    GameData(int _xWidth, int _yHeight, int _zDepth);
    
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
};