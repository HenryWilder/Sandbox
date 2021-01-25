#include <fstream>
#include <iostream>
#include "Abstraction.h"
#include "Globals.h"
#include "Transistor.h"

Component::Component(std::vector<Transistor*>& selection, Vector2 position)
{
    m_componentTransistorCount = selection.size();
    s_allComponents.push_back(this);
    Vector2 minCorner = { -1.0f * g_gridSize, -1.0f * g_gridSize };
    Vector2 maxCorner = { g_gridSize, -1.0f * g_gridSize };

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

        Vector2 elemPos = { 0,0 };
        switch ((((externalIO & 0b0011) > 0) * 0b01) | (((externalIO & 0b1100) > 0) * 0b10)) // Convert the 4 bits of data into just 2
        {
        default:
        case 0b00: // Entirely contained within the selection
            m_internalCount++;
            break;

        case 0b01: // Has only input(s) outside the selection
            elemPos.x = -1.0f * g_gridSize;
            elemPos.y = (g_gridSize * (float)(m_inputCount));
            m_inputCount++;
            break;

        case 0b10: // Has only output(s) outside the selection
            elemPos.x = g_gridSize;
            elemPos.y = (g_gridSize * (float)(m_outputCount));
            m_outputCount++;
            break;

        case 0b11: // Connects externally both on input and output
            
            // The output transistor will be created here
            elemPos.x = g_gridSize;
            elemPos.y = (g_gridSize * (float)(m_outputCount));
            m_outputCount++;
            maxCorner.y = __max(elemPos.y, maxCorner.y);

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
            elemPos.x = -1.0f * g_gridSize;
            elemPos.y = (g_gridSize * (float)(m_inputCount++));
            break;
        }
        maxCorner.y = __max(elemPos.y, maxCorner.y);
        elem->SetPos(position + elemPos);
        elem->SetComponent(this);
    }
    selection.reserve(selection.size() + toBePushed.size());
    for (Transistor* push : toBePushed)
    {
        selection.push_back(push);
    }

    m_caseShape.x = position.x + minCorner.x - g_gridSize;
    m_caseShape.y = position.y + minCorner.y;
    m_caseShape.width = (g_gridSize * 4.0f);
    m_caseShape.height = (abs(maxCorner.y - minCorner.y) + (g_gridSize));
    printf("Position: { %#1.1f, %#1.1f }", position.x, position.y);
    printf("Rectangle: {\n    x = %#1.1f,\n    y = %#1.1f,\n    width = %#1.1f,\n    height = %#1.1f\n}\n", m_caseShape.x, m_caseShape.y, m_caseShape.width, m_caseShape.height);

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

        if (externalIO == 1) elem->Hide();

        m_componentTransistors[(currentIndex[externalIO])] = *elem;
        m_componentTransistors[(currentIndex[externalIO])].Hide();

        printf("Transistor [%i] of the component is now: %p\n", int(currentIndex[externalIO]), m_componentTransistors[(currentIndex[externalIO])]);
        elem->ClearReferences();
        delete elem;
        elem = nullptr;
        currentIndex[externalIO]++;
    }
    for (size_t i = 0; i < m_componentTransistorCount; ++i)
    {
        printf("Transistor [%i] confirmed to be %p.\n", i, m_componentTransistors[i]);
    }
}

Component::~Component()
{
    delete[] m_componentTransistors;
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

Transistor** Component::InputsBegin()
{
    return m_componentTransistors;
}

Transistor** Component::InputsEnd()
{
    return InputsBegin() + m_inputCount;
}

Transistor** Component::InternalsBegin()
{
    return InputsEnd();
}

Transistor** Component::InternalsEnd()
{
    return InternalsBegin() + m_internalCount;
}

Transistor** Component::OutputsBegin()
{
    return InternalsEnd();
}

Transistor** Component::OutputsEnd()
{
    return (m_componentTransistors + m_componentTransistorCount);
}

Transistor** Component::Begin()
{
    return m_componentTransistors;
}

Transistor** Component::End()
{
    return m_componentTransistors + m_componentTransistorCount;
}

std::vector<Transistor*> Component::GetInputs()
{
    std::vector<Transistor*> arr;
    arr.reserve(m_inputCount);

    for (size_t i = 0; i < m_inputCount; ++i)
    {
        arr.push_back(m_componentTransistors[i]);
    }

    return arr;
}

std::vector<Transistor*> Component::GetInternals()
{
    std::vector<Transistor*> arr;
    arr.reserve(m_internalCount);

    for (size_t i = m_inputCount; i < (m_inputCount + m_internalCount); ++i)
    {
        arr.push_back(m_componentTransistors[i]);
    }

    return arr;
}

std::vector<Transistor*> Component::GetOutputs()
{
    std::vector<Transistor*> arr;
    arr.reserve(m_outputCount);

    for (size_t i = (m_componentTransistorCount - m_outputCount); i < m_componentTransistorCount; ++i)
    {
        arr.push_back(m_componentTransistors[i]);
    }

    return arr;
}

std::vector<Transistor*> Component::GetTransistors()
{
    std::vector<Transistor*> arr;
    arr.reserve(m_outputCount);

    for (size_t i = 0; i < m_componentTransistorCount; ++i)
    {
        arr.push_back(m_componentTransistors[i]);
    }

    return arr;
}

void Component::SelectTransistors(std::vector<Transistor*>& selection)
{
    selection.reserve(selection.size() + m_componentTransistorCount);

    for (size_t i = 0; i < m_componentTransistorCount; ++i)
    {
        selection.push_back(m_componentTransistors[i]);
    }
}

Vector2 Component::GetPos() const
{
    return Vector2{ m_caseShape.x, m_caseShape.y };
}

Rectangle Component::GetCaseRect() const
{
    return m_caseShape;
}

void Component::Move(Vector2 deltaPos)
{
    m_caseShape.x = m_caseShape.x + deltaPos.x;
    m_caseShape.y = m_caseShape.y + deltaPos.y;
}

void Component::MoveAbsolute(Vector2 newPos)
{
    m_caseShape.x = newPos.x;
    m_caseShape.y = newPos.y;
}

void Component::UpdateCasing()
{
    int maxX = INT_MIN;
    int maxY = INT_MIN;

    int minX = INT_MAX;
    int minY = INT_MAX;
    
    auto updateCorners = [&](Transistor* elem) {
        Vector2 checkPos = elem->GetPos();
        int checkPosX = (int)checkPos.x;
        int checkPosY = (int)checkPos.y;

        maxX = __max(maxX, checkPosX);
        minX = __min(minX, checkPosX);
        maxY = __max(maxY, checkPosY);
        minY = __min(minY, checkPosY);
    };

    for (size_t i = 0; i < m_inputCount; ++i)
    {
        updateCorners(m_componentTransistors[i]);
    }
    for (size_t i = (m_componentTransistorCount - m_outputCount); i < m_componentTransistorCount; ++i)
    {
        updateCorners(m_componentTransistors[i]);
    }

    m_caseShape.x = ((float)minX - g_gridSize);
    m_caseShape.y = ((float)minY - g_gridSize);
    Vector2 extents = Vector2{ (float)maxX + g_gridSize, (float)maxY + g_gridSize } - Vector2{ m_caseShape.x, m_caseShape.y };
    m_caseShape.width = (extents.x);
    m_caseShape.height = (extents.y);
}

void Component::Icon(Color color)
{
    Vector3 edgeColor = ColorToHSV(color);
    DrawRectangleRec(m_caseShape, color);
    DrawRectangleLinesEx(m_caseShape, 2, ColorFromHSV(edgeColor.x, edgeColor.y, edgeColor.z + 12.0f));
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
