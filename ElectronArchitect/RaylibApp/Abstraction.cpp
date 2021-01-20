#include "Abstraction.h"
#include <fstream>
#include "Globals.h"
#include "Wire.h"

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
    for (size_t i = 0; i < newComponent.internalCount; ++i)
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

void MakeAbstract(std::vector<Transistor*>& selection, Vector2 position, float g_gridSize)
{
    size_t input = 0;
    size_t output = 0;
    for (Transistor* elem : selection)
    {
        if (elem->OutputOnly()) elem->pos = { position.x - g_gridSize, position.y + (g_gridSize * (float)(input++))};
        else if (elem->InputOnly()) elem->pos = { position.x + g_gridSize, position.y + (g_gridSize * (float)(output++))};
        else elem->pos = position;
    }
}
