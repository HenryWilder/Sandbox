#include "Logic.h"
#include "Containers.h"



GameData::GameData() {
    xWidth = 0;
    yHeight = 0;
    zDepth = 0;
}

GameData::GameData(int _xWidth, int _yHeight, int _zDepth) {
    xWidth = _xWidth;
    yHeight = _yHeight;
    zDepth = _zDepth;
    unsigned int size = sizeof(ComponentID) * xWidth * yHeight * zDepth;
    printf("Reserving %u bytes (%u GB)...\n", size, size / 1024 / 1024 / 1024);
    voxels.reserve(Volume());
    for (validVoxelCount = 0; validVoxelCount < Volume(); ++validVoxelCount) {
        voxels.push_back(ComponentID());
    }
    printf("Memory reserved!\n");
}

GameData& GameData::operator=(const GameData& src) {
    xWidth = src.xWidth;
    yHeight = src.yHeight;
    zDepth = src.zDepth;

    voxels = src.voxels;
    return *this;
}



bool GameData::InRange(Int3 pos) {
    return (pos.x >= XMin() && pos.x <= XMax() &&
            pos.y >= YMin() && pos.y <= YMax() &&
            pos.z >= ZMin() && pos.z <= ZMax());
}

size_t GameData::Index(Int3 pos) {
    Int3 pos1(pos.x + XMax(), pos.y + YMax(), pos.z + ZMax());
    return (pos1.z * XYArea()) + (pos1.y * (size_t)XWidth()) + pos1.x;
}



bool GameData::IsPointFree(Int3 pos) {
    ComponentID& id = ID_At(pos);
    return (id.tag != ComponentID::Comp_NullID);
}

GameData::IDMap::Value_t GameData::GateAt(Int3 at) {
    if (InRange(at)) {
        const ComponentID& id = ID_At(at);
        if (id.tag == ComponentID::Comp_GateID)
            return gates.find(id.gateID)->second;
    }
    return gate_mem.end();
}
GameData::IDMap::Value_t GameData::UnitAt(Int3 at) {
    if (InRange(at)) {
        const ComponentID& id = ID_At(at);
        if (id.tag == ComponentID::Comp_UnitID)
            return units.find(id.unitID)->second;
    }
    return unit_mem.end();
}

GameData::ID_t GameData::PushGate(const Gate& gate, Int3 at) {
    ID_t id = gates.push(gate_mem.push(gate));
    ID_At(at) = ComponentID(id);
    return id;
}
GameData::ID_t GameData::PushUnit(const Unit& unit, Int3 at) {
    ID_t id = units.push(unit_mem.push(unit));
    ID_At(at) = ComponentID(id);
    return id;
}

void GameData::EraseAt(Int3 at) {
    ComponentID& id = ID_At(at);
    if (id.tag == ComponentID::Comp_GateID) {
        gate_mem.erase(gates.find(id.gateID)->second);
        gates.erase(id.gateID);
    }
    else if (id.tag == ComponentID::Comp_UnitID) {
        unit_mem.erase(units.find(id.unitID)->second);
        units.erase(id.unitID);
    }
    id = nullptr;
}



bool GameData::IDMap::valid(Map_CIter_t elem) { return !(elem == items.end()); }
bool GameData::IDMap::valid(Key_t key) { return !((key == null_key) || (items.find(key) == items.end())); }

GameData::IDMap::Map_Iter_t GameData::IDMap::find(Key_t id) {
    auto it = items.find(id);
    if (it != items.end()) return it;
    else return Map_Iter_t{};
}

GameData::IDMap::Key_t GameData::IDMap::push(Value_t val) {
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

    if (id != null_key) items[id] = val;
    return id;
}

void GameData::IDMap::erase(Key_t id) {
    if (valid(id)) {
        items.erase(id);
        invalids.push_back(id);
    }
}

auto GameData::IDMap::begin() { return items.begin(); }

auto GameData::IDMap::end() { return items.end(); }

auto GameData::IDMap::size() { return items.size(); }

ComponentID::ComponentID() : tag(Comp_NullID), gateID(GameData::IDMap::null_key) {};
ComponentID::ComponentID(ID_t _gateID) : tag(Comp_GateID), gateID(_gateID) {};
ComponentID::ComponentID(ID_t _unitID) : tag(Comp_UnitID), unitID(_unitID) {};

ComponentID& ComponentID::operator=(nullptr_t _null) {
    tag = Comp_NullID;
    gateID = GameData::IDMap::null_key;
    return *this;
}

ComponentID& ComponentID::operator=(ID_t _gateID) {
    tag = Comp_GateID;
    gateID = _gateID;
    return *this;
}

ComponentID& ComponentID::operator=(ID_t _unitID) {
    tag = Comp_UnitID;
    unitID = _unitID;
    return *this;
}

GateMemory::Iter GateMemory::push(const Type& _gate) {
    items.push_back(_gate);
    Iter back = items.end();
    return --back;
}

GateMemory::Iter GateMemory::end() {
    return items.end();
}

void GateMemory::erase(GateMemory::Iter at) {
    items.erase(at);
}

UnitMemory::Iter UnitMemory::push(const Type& _unit) {
    items.push_back(_unit);
    Iter back = items.end();
    return --back;
}

UnitMemory::Iter UnitMemory::end() {
    return items.end();
}

void UnitMemory::erase(Iter at) {
    items.erase(at);
}

std::vector<GameData::TreeNode::ElementType>& GameData::TreeNode::GetElements() {
    return elements;
}

int TreeNode::Navigate(int x, int y) {
    IntRect& child0rec = children[0]->coverage;
    return
        ((int)(x > child0rec.x2())) |
        ((int)(y > child0rec.y2()) << 1);
}
