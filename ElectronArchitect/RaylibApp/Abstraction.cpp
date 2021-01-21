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
        char externalInput = 0b00;
        if ((!elem->input[0].connector) && (!elem->input[1].connector)) externalInput = 0b101; // If both are nullptrs, then this is output-only
        else
        {
            if ((elem->input[0].connector) && (std::find(selection.begin(), selection.end(), elem->input[0].connector) == selection.end())) externalInput |= 0b01;
            if ((elem->input[1].connector) && (std::find(selection.begin(), selection.end(), elem->input[1].connector) == selection.end())) externalInput |= 0b10;
        }

        char externalOutput = 0b00;
        if ((!elem->output[0].connector) && (!elem->output[1].connector)) externalOutput = 0b101; // If both are nullptrs, then this is input-only
        else
        {
            if ((elem->output[0].connector) && (std::find(selection.begin(), selection.end(), elem->output[0].connector) == selection.end())) externalOutput |= 0b01;
            if ((elem->output[1].connector) && (std::find(selection.begin(), selection.end(), elem->output[1].connector) == selection.end())) externalOutput |= 0b10;
        }
        if (externalInput & externalOutput & 0b100) printf("Some real bullshit happened"); // How the fuck does this happen

        if (externalInput && externalOutput) // Connects externally both on input and output
        {
            // elem will always be the input
            Vector2 elemInPos;
            elemInPos.x = -1.0f * g_gridSize;
            elemInPos.y = (g_gridSize * (float)(m_inputCount++));
            if (elemInPos.y > maxCorner.y) maxCorner.y = elemInPos.y;
            elem->pos = position + elemInPos;

            // The output transistor will be created here
            Vector2 elemOutPos;
            elemOutPos.x = g_gridSize;
            elemOutPos.y = (g_gridSize * (float)(m_outputCount++));
            if (elemOutPos.y > maxCorner.y) maxCorner.y = elemOutPos.y;
            Transistor* output = new Transistor(position + elemOutPos);
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
            if (externalOutput & 0b01)
            {
                output->output[0].connector = elem->output[0].connector;
                elem->output[0].connector = output;
                int port = output->output[0].connector->GetInputIndex(elem);
                output->output[0].connector->input[port] = output;
            }
            if (externalOutput & 0b10)
            {
                output->output[1].connector = elem->output[1].connector;
                if (elem->output[0].connector != output) elem->output[1].connector = output; // There may already be an internal connection in output[0]
                int port = output->output[1].connector->GetInputIndex(elem);
                output->output[1].connector->input[port] = output;
            }
        }
        if (externalInput && !externalOutput) // Has only input(s) outside the selection
        {
            Vector2 elemPos;
            elemPos.x = -1.0f * g_gridSize;
            elemPos.y = (g_gridSize * (float)(m_inputCount++));
            if (elemPos.y > maxCorner.y) maxCorner.y = elemPos.y;
            elem->pos = position + elemPos;
        }
        else if (!externalInput && externalOutput) // Has only output(s) outside the selection
        {
            Vector2 elemPos;
            elemPos.x = g_gridSize;
            elemPos.y = (g_gridSize * (float)(m_outputCount++));
            if (elemPos.y > maxCorner.y) maxCorner.y = elemPos.y;
            elem->pos = position + elemPos;
        }
        else // Entirely contained within the selection
        {
            elem->pos = position;
            m_internalCount++;
        }
        elem->containingComponent = this;
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

    m_componentTransistors = new Transistor*[m_componentTransistorCount];

    size_t currentIndex[3] = { 0, m_inputCount, m_inputCount + m_internalCount }; // { inputs, internals, outputs }
    for (size_t index = 0; index < selection.size(); ++index)
    {
        Transistor* elem = selection[index];
        char externalIO = 0b00;
        if (std::find(selection.begin(), selection.end(), elem->input[0].connector) == selection.end()) externalIO |= 0b01;
        else if (std::find(selection.begin(), selection.end(), elem->input[1].connector) == selection.end()) externalIO |= 0b01;
        if (std::find(selection.begin(), selection.end(), elem->output[0].connector) == selection.end()) externalIO |= 0b10;
        else if (std::find(selection.begin(), selection.end(), elem->output[1].connector) == selection.end()) externalIO |= 0b10;

        if (externalIO != 3) m_componentTransistors[(currentIndex[externalIO]++) + index] = elem;
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
    Vector2 maxCorner = { m_caseShape.x + g_gridSize, m_caseShape.y + g_gridSize };
    Vector2 minCorner = { maxCorner.x + m_caseShape.width - (g_gridSize * 2.0f), maxCorner.y + m_caseShape.height - (g_gridSize * 2.0f)};
    
    auto updateCorners = [&minCorner, &maxCorner](Transistor* elem) {
        if (elem->pos.x > maxCorner.x) maxCorner.x = elem->pos.x;
        else if (elem->pos.x < minCorner.x) minCorner.x = elem->pos.x;
        if (elem->pos.y > maxCorner.y) maxCorner.y = elem->pos.y;
        else if (elem->pos.y < minCorner.y) minCorner.y = elem->pos.y;
    };

    for (Transistor** input = InputsBegin(); input != InputsEnd(); input++)
    {
        updateCorners(*input);
    }
    for (Transistor** output = OutputsBegin(); output != OutputsEnd(); output++)
    {
        updateCorners(*output);
    }

    m_caseShape.x = m_caseShape.x + minCorner.x - g_gridSize;
    m_caseShape.y = m_caseShape.y + minCorner.y - g_gridSize;
    Vector2 space = maxCorner + (minCorner * -1.0f); // Max - min
    m_caseShape.width = (space.x + (g_gridSize * 2.0f));
    m_caseShape.height = (space.y + (g_gridSize * 2.0f));
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
    // TODO
}

std::vector<Component*> Component::s_allComponents;
