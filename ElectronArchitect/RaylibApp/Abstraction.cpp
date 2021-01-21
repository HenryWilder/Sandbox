#include "Abstraction.h"
#include <fstream>
#include "Globals.h"
#include "Transistor.h"

Component::~Component()
{
    for (size_t i = 0; i < internalCount; ++i)
    {
        delete internals[i];
    }
    delete[] internals;

    for (size_t i = 0; i < inputCount; ++i)
    {
        Erase(Transistor::allTransistors, inputs[i]);
        delete inputs[i];
    }
    delete[] inputs;

    for (size_t i = 0; i < outputCount; ++i)
    {
        Erase(Transistor::allTransistors, outputs[i]);
        delete outputs[i];
    }
    delete[] outputs;
}

void Component::Move(Vector2 deltaPos)
{
    pos = pos + deltaPos;
}

void Component::MoveAbsolute(Vector2 newPos)
{
    Vector2 tempPos = pos;
    pos = newPos;
    Move(newPos - tempPos);
}

void Component::UpdateCasing()
{
    Vector2 maxCorner = { casing.x + g_gridSize, casing.y + g_gridSize };
    Vector2 minCorner = { maxCorner.x + casing.width - (g_gridSize * 2.0f), maxCorner.y + casing.height - (g_gridSize * 2.0f)};
    
    auto updateCorners = [&minCorner, &maxCorner](Transistor* elem) {
        if (elem->pos.x > maxCorner.x) maxCorner.x = elem->pos.x;
        else if (elem->pos.x < minCorner.x) minCorner.x = elem->pos.x;
        if (elem->pos.y > maxCorner.y) maxCorner.y = elem->pos.y;
        else if (elem->pos.y < minCorner.y) minCorner.y = elem->pos.y;
    };

    for (size_t i = 0; i < inputCount; ++i)
    {
        updateCorners(inputs[i]);
    }
    for (size_t i = 0; i < outputCount; ++i)
    {
        updateCorners(outputs[i]);
    }

    casing.x = pos.x + minCorner.x - g_gridSize;
    casing.y = pos.y + minCorner.y - g_gridSize;
    Vector2 space = maxCorner + (minCorner * -1.0f); // Max - min
    casing.width = (space.x + (g_gridSize * 2.0f));
    casing.height = (space.y + (g_gridSize * 2.0f));
}

void Component::Icon(Color color)
{
    Vector3 edgeColor = ColorToHSV(color);
    DrawRectangleRec(casing, color);
    DrawRectangleLinesEx(casing, 2, ColorFromHSV(edgeColor.x, edgeColor.y, edgeColor.z + 12.0f));
}

void Component::Draw()
{
    Icon({ 32,32,32, 255 });

    for (size_t i = 0; i < internalCount; ++i)
    {
        internals[i]->b_drawnThisFrame = true; // Skip drawing later
    }
}

void Component::Highlight(Color color)
{
    Icon(color);
}

void Component::ClearReferences()
{
    // TODO
}

std::vector<Component*> Component::allComponents;

Component* MakeAbstract(std::vector<Transistor*>& selection, Vector2 position, float g_gridSize)
{
    Component* component = new Component();
    Component::allComponents.push_back(component);
    Vector2 minCorner = { -1.0f * g_gridSize, -1.0f * g_gridSize };
    Vector2 maxCorner = { g_gridSize, -1.0f * g_gridSize };

    for (Transistor* elem : selection)
    {
        if (elem->OutputOnly())
        {
            Vector2 elemPos;
            elemPos.x = -1.0f * g_gridSize;
            elemPos.y = (g_gridSize * (float)(component->inputCount++));
            if (elemPos.y > maxCorner.y) maxCorner.y = elemPos.y;
            elem->pos = position + elemPos;
        }
        else if (elem->InputOnly())
        {
            Vector2 elemPos;
            elemPos.x = g_gridSize;
            elemPos.y = (g_gridSize * (float)(component->outputCount++));
            if (elemPos.y > maxCorner.y) maxCorner.y = elemPos.y;
            elem->pos = position + elemPos;
        }
        else
        {
            elem->pos = position;
            component->internalCount++;
        }
        elem->containingComponent = component;
    }

    component->casing.x = position.x + minCorner.x - g_gridSize;
    component->casing.y = position.y + minCorner.y;
    component->casing.width = (g_gridSize * 4.0f);
    component->casing.height = (abs(maxCorner.y - minCorner.y) + (g_gridSize));
    printf("Position: { %#1.1f, %#1.1f }", position.x, position.y);
    printf("Rectangle: {\n    x = %#1.1f,\n    y = %#1.1f,\n    width = %#1.1f,\n    height = %#1.1f\n}\n", component->casing.x, component->casing.y, component->casing.width, component->casing.height);

    component->inputs = new Transistor*[component->inputCount];
    size_t index = 0;
    for (Transistor* elem : selection)
    {
        if (elem->OutputOnly()) component->inputs[index++] = elem;
        if (index >= component->inputCount) break;
    }

    component->outputs = new Transistor*[component->outputCount];
    index = 0;
    for (Transistor* elem : selection)
    {
        if (elem->InputOnly()) component->outputs[index++] = elem;
        if (index >= component->outputCount) break;
    }

    component->internals = new Transistor*[component->internalCount];
    index = 0;
    for (Transistor* elem : selection)
    {
        if (!elem->InputOnly() && !elem->OutputOnly())
        {
            component->internals[index] = elem;
            elem->b_hidden = true;
            Erase(Transistor::allTransistors, component->internals[index]);
        }
        index++;
        if (index >= component->internalCount) break;
    }

    return component;
}
