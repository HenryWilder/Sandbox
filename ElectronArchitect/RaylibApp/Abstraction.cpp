#include "Abstraction.h"
#include "Wire.h"
#include <fstream>

AbstractComponent::AbstractComponent(std::vector<Transistor*> const* base)
{
    // TODO
    for (Transistor* templateTransistor : *base)
    {
        templateTransistor->pos;
    }
}

void AbstractComponent::Spawn(Vector2 position)
{
    Component newComponent;
    newComponent.internalCount = this->internals.size();

    newComponent.internals = new ComponentTransistor[newComponent.internalCount];
    for (int i = 0; i < newComponent.internalCount; ++i)
    {
        newComponent.internals[i] = this->internals[i];
    }
}

void Component::Evaluate()
{
    // TODO
}

void ComponentPort::Draw()
{
    DrawCircleV(pos, 4, GREEN);
}

void Component::Draw()
{
    DrawRectangleRec(casing, DARKGRAY);
}

void MakeAbstract(std::vector<Transistor*>& selection)
{
    Vector2 center = { 0,0 };
    for (Transistor* elem : selection)
    {
        center = center + elem->pos;
    }
    if (selection.size() > 0) center = { center.x / (float)selection.size(), center.y / (float)selection.size() };
    for (Transistor* elem : selection)
    {
        elem->pos = center;
    }
}
