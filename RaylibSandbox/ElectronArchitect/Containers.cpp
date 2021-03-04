#include "Logic.h"
#include "Containers.h"

Int3::Int3(Vector3 pos) : x(Round(pos.x)), y(Round(pos.y)), z(Round(pos.z)) {};
Int3::Int3(Vector2 pos, float layer) : x(Round(pos.x)), y(Round(pos.y)), z(Round(layer)) {};
Int3::Int3(Vector2 pos, int layer) : x(Round(pos.x)), y(Round(pos.y)), z(layer) {};

bool Int3::operator==(const Int3& b) {
    return (x == b.x && y == b.y && z == b.z);
}

bool Int3::operator!=(const Int3& b) {
    return (x != b.x || y != b.y || z != b.z);
}

Vector2 Int3::xy() {
    return Vector2{ (float)x, (float)y };
}

Vector3 Int3::xyz() {
    return Vector3{ (float)x, (float)y, (float)z };
}


VoxelArray::VoxelArray() {
    validVoxelCount = 0;
    xWidth = 0;
    yHeight = 0;
    zDepth = 0;
}

VoxelArray::VoxelArray(int _xWidth, int _yHeight, int _zDepth) {
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

VoxelArray& VoxelArray::operator=(const VoxelArray& src) {
    xWidth = src.xWidth;
    yHeight = src.yHeight;
    zDepth = src.zDepth;

    voxels = src.voxels;
    return *this;
}

int VoxelArray::XWidth() { return xWidth; }
int VoxelArray::YHeight() { return yHeight; }
int VoxelArray::ZDepth() { return zDepth; }

int VoxelArray::XMax() { return XWidth() / 2; }
int VoxelArray::XMin() { return XMax() - XWidth(); }
int VoxelArray::YMax() { return YHeight() / 2; }
int VoxelArray::YMin() { return YMax() - YHeight(); }
int VoxelArray::ZMax() { return ZDepth() / 2; }
int VoxelArray::ZMin() { return ZMax() - ZDepth(); }

size_t VoxelArray::XYArea() { return (size_t)XWidth() * (size_t)YHeight(); }
size_t VoxelArray::Volume() { return XYArea() * (size_t)ZDepth(); }

bool VoxelArray::InRange(Int3 pos) {
    return (pos.x >= XMin() && pos.x <= XMax() &&
            pos.y >= YMin() && pos.y <= YMax() &&
            pos.z >= ZMin() && pos.z <= ZMax());
}

size_t VoxelArray::Index(Int3 pos) {
    Int3 pos1(pos.x + XMax(), pos.y + YMax(), pos.z + ZMax());
    return (pos1.z * XYArea()) + (pos1.y * (size_t)XWidth()) + pos1.x;
}

ComponentID& VoxelArray::ID_At(Int3 pos) {
    return voxels[Index(pos)];
}

bool VoxelArray::IsPointFree(Int3 pos) {
    ComponentID& id = ID_At(pos);
    return (id.tag != ComponentID::Comp_NullID);
}

IDMap_Gate::Value_t VoxelArray::GateAt(Int3 at) {
    if (InRange(at)) {
        const ComponentID& id = ID_At(at);
        if (id.tag == ComponentID::Comp_GateID)
            return gates.find(id.gateID)->second;
    }
    return gate_mem.end();
}
IDMap_Unit::Value_t VoxelArray::UnitAt(Int3 at) {
    if (InRange(at)) {
        const ComponentID& id = ID_At(at);
        if (id.tag == ComponentID::Comp_UnitID)
            return units.find(id.unitID)->second;
    }
    return unit_mem.end();
}

GateID_t VoxelArray::PushGate(const Gate& gate, Int3 at) {
    GateID_t id = gates.push(gate_mem.push(gate));
    ID_At(at) = ComponentID(id);
    return id;
}
UnitID_t VoxelArray::PushUnit(const Unit& unit, Int3 at) {
    UnitID_t id = units.push(unit_mem.push(unit));
    ID_At(at) = ComponentID(id);
    return id;
}

void VoxelArray::EraseAt(Int3 at) {
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

size_t VoxelArray::ResizedMemory() {
    return validVoxelCount;
}

IDMap_Gate::IDMap_Gate() {
    items[nPos];
}

bool IDMap_Gate::valid(Map_CIter_t elem) { return !(elem == items.end()); }
bool IDMap_Gate::valid(Key_t key) { return !((key == nPos) || (items.find(key) == items.end())); }

IDMap_Gate::Map_Iter_t IDMap_Gate::find(Key_t id) {
    auto it = items.find(id);
    if (it != items.end()) return it;
    else return Map_Iter_t{};
}

IDMap_Gate::Key_t IDMap_Gate::push(Value_t val) {
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

void IDMap_Gate::erase(Key_t id) {
    if (valid(id)) {
        items.erase(id);
        invalids.push_back(id);
    }
}

auto IDMap_Gate::begin() { return items.begin(); }
auto IDMap_Gate::end()   { return items.end();   }
auto IDMap_Gate::size()  { return items.size();  }

IDMap_Unit::IDMap_Unit() {
    items[nPos];
}

bool IDMap_Unit::valid(Map_CIter_t elem) { return !(elem == items.end()); }
bool IDMap_Unit::valid(Key_t key) { return !((key == nPos) || (items.find(key) == items.end())); }

IDMap_Unit::Map_Iter_t IDMap_Unit::find(Key_t id) {
    auto it = items.find(id);
    if (it != items.end()) return it;
    else return Map_Iter_t{};
}

IDMap_Unit::Key_t IDMap_Unit::push(Value_t val) {
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

void IDMap_Unit::erase(Key_t id) {
    if (valid(id)) {
        items.erase(id);
        invalids.push_back(id);
    }
}

auto IDMap_Unit::begin() { return items.begin(); }

auto IDMap_Unit::end() { return items.end(); }

auto IDMap_Unit::size() { return items.size(); }

ComponentID::ComponentID() : tag(Comp_NullID), gateID(IDMap_Gate::nPos) {};
ComponentID::ComponentID(GateID_t _gateID) : tag(Comp_GateID), gateID(_gateID) {};
ComponentID::ComponentID(UnitID_t _unitID) : tag(Comp_UnitID), unitID(_unitID) {};

ComponentID& ComponentID::operator=(nullptr_t _null) {
    tag = Comp_NullID;
    gateID = IDMap_Gate::nPos;
    return *this;
}

ComponentID& ComponentID::operator=(GateID_t _gateID) {
    tag = Comp_GateID;
    gateID = _gateID;
    return *this;
}

ComponentID& ComponentID::operator=(UnitID_t _unitID) {
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
