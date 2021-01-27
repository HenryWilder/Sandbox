#include <fstream>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include "CustomTypes.h"
#include "Globals.h"
#include "Transistor.h"
#include "Abstraction.h"

Component::Component(std::vector<Transistor*>& selection, Int2 position)
{
    m_componentTransistorCount = selection.size();
    s_allComponents.push_back(this);

    IntRect rectangle(Int2(-g_gridSize), Int2(g_gridSize, -g_gridSize));

    m_inputCount = 0;
    m_internalCount = 0;
    m_outputCount = 0;

    std::vector<Transistor*> toBePushed;

    for (Transistor* elem : selection)
    {
        char externalIO = 0b00000;
        if ((!elem->input[0].connector) && (!elem->input[1].connector)) externalIO = 0b10001; // If both are nullptrs, then this is output-only
        else
        {
            if ((elem->input[0].connector) && (std::find(selection.begin(), selection.end(), elem->input[0].connector) == selection.end())) externalIO |= 0b0001;
            if ((elem->input[1].connector) && (std::find(selection.begin(), selection.end(), elem->input[1].connector) == selection.end())) externalIO |= 0b0010;
        }

        if ((!elem->output[0].connector) && (!elem->output[1].connector)) externalIO = 0b10100; // If both are nullptrs, then this is input-only
        else
        {
            if ((elem->output[0].connector) && (std::find(selection.begin(), selection.end(), elem->output[0].connector) == selection.end())) externalIO |= 0b0100;
            if ((elem->output[1].connector) && (std::find(selection.begin(), selection.end(), elem->output[1].connector) == selection.end())) externalIO |= 0b1000;
        }
        if (externalIO & 0b10000) printf("Some real bullshit happened"); // How the fuck does this happen

        Int2 elemPos(0);
        switch ((((externalIO & 0b0011) > 0) * 0b01) | (((externalIO & 0b1100) > 0) * 0b10)) // Convert the 4 bits of data into just 2
        {
        default:
        case 0b00: // Entirely contained within the selection
            m_internalCount++;
            break;

        case 0b01: // Has only input(s) outside the selection
            elemPos.x = -g_gridSize;
            elemPos.y = (int)((size_t)g_gridSize * m_inputCount);
            m_inputCount++;
            break;

        case 0b10: // Has only output(s) outside the selection
            elemPos.x = g_gridSize;
            elemPos.y = (int)((size_t)g_gridSize * m_outputCount);
            m_outputCount++;
            break;

        case 0b11: // Connects externally both on input and output
            
            // The output transistor will be created here
            elemPos.x = g_gridSize;
            elemPos.y = (int)((size_t)g_gridSize * m_outputCount);
            m_outputCount++;
            rectangle.m_cornerMax.y = Max(elemPos.y, rectangle.m_cornerMax.y);

            {
                Transistor* output = new Transistor(position + elemPos);
                toBePushed.push_back(output); // We'll push them all at once later so we don't invalidate the iterator.
                ++m_componentTransistorCount;

                /*********************
                *
                *   i-->a-->o
                *   becomes
                *   i-->a-->b-->o
                *
                *   i-->a-->o
                *   i---'
                *   becomes
                *   i-->a-->b-->o
                *   i---'
                *
                *   i-->a-->o
                *       '-->o
                *   becomes
                *   i-->a-->b-->o
                *           '-->o
                *
                *   i-->a-->o
                *   i---"-->o
                *   becomes
                *   i-->a-->b-->o
                *   i---'   '-->o
                *
                *********************/
                // Find *which* output to connect (non-exclusive)
                if (externalIO & 0b0100)
                {
                    output->output[0].connector = elem->output[0].connector;
                    elem->output[0].connector = output;
                    int port = output->output[0].connector->GetInputIndex(elem);
                    output->output[0].connector->input[port] = output;
                }
                if (externalIO & 0b1000)
                {
                    output->output[1].connector = elem->output[1].connector;
                    if (elem->output[0].connector != output) elem->output[1].connector = output; // There may already be an internal connection in output[0]
                    int port = output->output[1].connector->GetInputIndex(elem);
                    output->output[1].connector->input[port] = output;
                }
            }

            // elem will always be the input
            elemPos.x = -g_gridSize;
            elemPos.y = (int)((size_t)g_gridSize * m_inputCount++);
            break;
        }
        rectangle.m_cornerMax.y = Max(elemPos.y, rectangle.m_cornerMax.y);
        elem->SetPos(position + elemPos);
        elem->SetComponent(this);
    }
    selection.reserve(selection.size() + toBePushed.size());
    for (Transistor* push : toBePushed)
    {
        selection.push_back(push);
    }

    m_caseShape = rectangle;
    m_caseShape.Move(position);
    m_caseShape.GrowBorder(1);

    printf("Position: { %i, %i }", position.x, position.y);
    printf("Rectangle: {\n    x = %i,\n    y = %i,\n    width = %i,\n    height = %i\n}\n", m_caseShape.m_cornerMin.x, m_caseShape.m_cornerMin.y, m_caseShape.Range().x, m_caseShape.Range().y);

    m_componentTransistors = new Transistor[m_componentTransistorCount]();

    size_t currentIndex[3] = { 0, m_inputCount, m_componentTransistorCount - m_outputCount }; // { inputs, internals, outputs }
    for (Transistor* elem : selection)
    {
        char externalIO = 1;

        if ((!elem->input[0].connector && !elem->input[1].connector) ||
            (elem->input[0].connector) && (std::find(selection.begin(), selection.end(), elem->input[0].connector) == selection.end()) ||
            (elem->input[1].connector) && (std::find(selection.begin(), selection.end(), elem->input[1].connector) == selection.end()))
        {
            externalIO = 0;
        }
        else if ((!elem->output[0].connector && !elem->output[1].connector) ||
            (elem->output[0].connector) && (std::find(selection.begin(), selection.end(), elem->output[0].connector) == selection.end()) ||
            (elem->output[1].connector) && (std::find(selection.begin(), selection.end(), elem->output[1].connector) == selection.end()))
        {
            externalIO = 2;
        }

        printf("%p belongs in sector %i\n", elem, externalIO);

        if ((currentIndex[externalIO]) < m_componentTransistorCount)
        {
            if (externalIO == 1) elem->Hide();

            memcpy(m_componentTransistors + (currentIndex[externalIO]), elem, sizeof(Transistor));
            m_componentTransistors[(currentIndex[externalIO])].Hide();

            printf("Transistor [%i] of the component is now: %p\n", int(currentIndex[externalIO]), m_componentTransistors + (currentIndex[externalIO]));
            elem->ClearReferences();
            delete elem;
            elem = nullptr;
            currentIndex[externalIO]++;
        }
    }
    for (size_t i = 0; i < m_componentTransistorCount; ++i)
    {
        printf("Transistor [%zu] confirmed to be %p.\n", i, m_componentTransistors + i);
    }
}

Component::Component(const Component& src)
{
    s_allComponents.push_back(this);

    m_caseShape = src.m_caseShape;
    m_inputCount = src.m_inputCount;
    m_internalCount = src.m_internalCount;
    m_outputCount = src.m_outputCount;
    m_componentTransistorCount = src.m_componentTransistorCount;

    m_componentTransistors = new Transistor[m_componentTransistorCount];
    if (m_componentTransistors) memcpy(m_componentTransistors, src.m_componentTransistors, (sizeof(Transistor) * m_componentTransistorCount));
}

Component::~Component()
{
    for (size_t i = 0; i < m_componentTransistorCount; ++i)
    {
        m_componentTransistors[i].ClearReferences();
    }
    delete[] m_componentTransistors;
    printf("Component containing %zu transistors deleted!\n", m_componentTransistorCount);
}

size_t Component::GetInputCount() const
{
    return m_inputCount;
}

size_t Component::GetOutputCount() const
{
    return m_outputCount;
}

size_t Component::GetInternalCount() const
{
    return m_internalCount;
}

size_t Component::GetTotalCount() const
{
    return m_componentTransistorCount;
}

std::vector<Transistor*> Component::GetInputs()
{
    std::vector<Transistor*> arr;
    arr.reserve(m_inputCount);

    for (size_t i = 0; i < m_inputCount; ++i)
    {
        arr.push_back(m_componentTransistors + i);
    }

    return arr;
}

void Component::ForEachInput(void(*func)(Transistor& elem, size_t index))
{
}

std::vector<Transistor*> Component::GetInternals()
{
    std::vector<Transistor*> arr;
    arr.reserve(m_internalCount);

    for (size_t i = m_inputCount; i < (m_inputCount + m_internalCount); ++i)
    {
        arr.push_back(m_componentTransistors + i);
    }

    return arr;
}

void Component::ForEachInternal(void(*func)(Transistor& elem, size_t index))
{
}

std::vector<Transistor*> Component::GetOutputs()
{
    std::vector<Transistor*> arr;
    arr.reserve(m_outputCount);

    for (size_t i = (m_componentTransistorCount - m_outputCount); i < m_componentTransistorCount; ++i)
    {
        arr.push_back(m_componentTransistors + i);
    }

    return arr;
}

void Component::ForEachOutput(void(*func)(Transistor& elem, size_t index))
{
}

std::vector<Transistor*> Component::GetTransistors()
{
    std::vector<Transistor*> arr;
    arr.reserve(m_outputCount);

    for (size_t i = 0; i < m_componentTransistorCount; ++i)
    {
        arr.push_back(m_componentTransistors + i);
    }

    return arr;
}

void Component::ForEachTransistor(void(*func)(Transistor& elem, size_t index))
{
}

void Component::SelectTransistors(std::vector<Transistor*>& selection)
{
    selection.reserve(selection.size() + m_componentTransistorCount);

    for (size_t i = 0; i < m_componentTransistorCount; ++i)
    {
        selection.push_back(m_componentTransistors + i);
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
    
    auto updateCorners = [&](Transistor* elem) {
        Int2 checkPos = elem->GetPos();
        min.x = Min(min.x, checkPos.x);
        min.y = Min(min.y, checkPos.y);
        max.x = Max(max.x, checkPos.x);
        max.y = Max(max.y, checkPos.y);
    };

    for (size_t i = 0; i < m_componentTransistorCount; ++i) {
        if (i == m_inputCount) {
            i += m_internalCount - 1; // Skip internals
            continue;
        }
        updateCorners(m_componentTransistors + i);
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

void Component::ClearReferences()
{
    for (size_t i = 0; i < m_componentTransistorCount; ++i)
    {
        m_componentTransistors[i].ClearReferences();
    }
    Erase(s_allComponents, this);
}

std::vector<Component*> Component::s_allComponents;
