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

void MakeAbstract(std::vector<Transistor*>& selection, Vector2 position, float gridSize)
{
    size_t input = 0;
    size_t output = 0;
    for (Transistor* elem : selection)
    {
        if (elem->OutputOnly()) elem->pos = { position.x - gridSize, position.y + (gridSize * (float)(input++))};
        else if (elem->InputOnly()) elem->pos = { position.x + gridSize, position.y + (gridSize * (float)(output++))};
        else elem->pos = position;
    }
}
