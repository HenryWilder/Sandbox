#include <fstream>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include "CustomTypes.h"
#include "Globals.h"
#include "Transistor.h"
#include "Abstraction.h"

Component::Component(std::vector<ID_t>& selection, Int2 position)
{
    size_t componentTransistorCount = selection.size();
    s_allComponents.push_back(this);

    IntRect rectangle(Int2(-g_gridSize), Int2(g_gridSize, -g_gridSize));

    size_t inputCount = 0;
    size_t internalCount = 0;
    size_t outputCount = 0;

    std::vector<ID_t> toBePushed;

    for (ID_t id : selection)
    {
        Transistor* elem = *id;

        // Something something something uhhhh look at the github backup
        // If it isn't there that's completely understandable and I'm sorry ;w;
        // It wasn't anything important I don't think, just something about determining whether a Transistor was an input or output

        rectangle.m_cornerMax.y = Max(elem->GetPos().y, rectangle.m_cornerMax.y);
        elem->SetPos(position + elem->GetPos());
        elem->SetComponent(this);
    }

    selection.reserve(selection.size() + toBePushed.size());
    for (ID_t push : toBePushed)
    {
        selection.push_back(push);
    }

    m_caseShape = rectangle;
    m_caseShape.Move(position);
    m_caseShape.GrowBorder(1);
}

Component::Component(const Component& src)
{
    s_allComponents.push_back(this);

    m_caseShape = src.m_caseShape;
    GetInputs().reserve(src.GetInputCount());
    GetInternals().reserve(src.GetInternalCount());
    GetOutputs().reserve(src.GetOutputCount());

    std::vector<ID_t> srcIDs = src.GetTransistors();
    std::vector<ID_t&> destIDs = GetTransistorRefs();

    for (size_t i = 0; i < GetTotalCount(); ++i)
    {
        destIDs[i] = srcIDs[i];
    }
}

size_t Component::GetInputCount() const { return m_componentTransistors[0].size(); }

size_t Component::GetInternalCount() const { return m_componentTransistors[1].size(); }

size_t Component::GetOutputCount() const { return m_componentTransistors[2].size(); }

size_t Component::GetTotalCount() const { return GetInputCount() + GetInternalCount() + GetTotalCount(); }

std::vector<ID_t>& Component::GetInputs() { return m_componentTransistors[0]; }

std::vector<ID_t>& Component::GetInternals() { return m_componentTransistors[1]; }

std::vector<ID_t>& Component::GetOutputs() { return m_componentTransistors[2]; }

std::vector<ID_t> Component::GetTransistors() const
{
    std::vector<ID_t> arr;
    arr.reserve(GetTotalCount());

    for (const std::vector<ID_t>& _arr : m_componentTransistors) {
        for (ID_t t : _arr) {
            arr.push_back(t);
        }
    }

    return arr;
}

std::vector<ID_t&> Component::GetTransistorRefs()
{
    std::vector<ID_t&> arr;
    arr.reserve(GetTotalCount());

    for (std::vector<ID_t>& _arr : m_componentTransistors) {
        for (ID_t& t : _arr) {
            arr.push_back(t);
        }
    }

    return arr;
}

void Component::SelectTransistors(std::vector<ID_t>& selection)
{
    selection.reserve(selection.size() + GetTotalCount());

    for (ID_t id : GetTransistors()) {
        selection.push_back(id);
    }
}

Int2 Component::GetPos() const
{
    return m_caseShape.m_cornerMin;
}

IntRect Component::GetCaseRect() const
{
    return m_caseShape;
}

void Component::Move(const Int2 deltaPos)
{
    m_caseShape.Move(deltaPos);
}

void Component::MoveAbsolute(const Int2 newPos)
{
    m_caseShape.Move(newPos - m_caseShape.m_cornerMin);
}

void Component::UpdateCasing()
{
    Int2 min = Int2(INT_MAX);
    Int2 max = Int2(INT_MIN);
    
    auto updateCorners = [&](ID_t elem) {
        Int2 checkPos = Transistor::s_transistorMap.find(elem)->second->GetPos();
        min.x = Min(min.x, checkPos.x);
        min.y = Min(min.y, checkPos.y);
        max.x = Max(max.x, checkPos.x);
        max.y = Max(max.y, checkPos.y);
    };

    for (size_t i = 0; i < GetTotalCount(); ++i) {
        if (i == GetInputCount()) {
            i += GetInternalCount() - 1; // Skip internals
            continue;
        }
        int bucket;
        if (i > (GetInputCount() + GetInternalCount())) bucket = 2;
        else if (i > GetInputCount()) bucket = 1;
        else bucket = 0;
        updateCorners(m_componentTransistors[bucket][i]);
    }

    m_caseShape.m_cornerMin = min;
    m_caseShape.m_cornerMax = max;
}

void Component::Icon(Color color)
{
    Vector3 edgeColor = ColorToHSV(color);
    DrawRectangleRec((Rectangle)m_caseShape, color);
    DrawRectangleLinesEx((Rectangle)m_caseShape, 2, ColorFromHSV(edgeColor.x, edgeColor.y, edgeColor.z + 12.0f));
}

void Component::Draw()
{
    Icon({ 32,32,32, 255 });
}

void Component::Highlight(Color color)
{
    Icon(color);
}

void Component::Destroy()
{
    for (ID_t id : GetTransistors()) {
        Transistor::Destroy(id);
    }
    Erase(s_allComponents, this);
}

std::vector<Component*> Component::s_allComponents;
