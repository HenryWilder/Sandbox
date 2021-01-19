#include "Abstraction.h"
#include "Wire.h"


AbstractComponent::AbstractComponent(std::vector<Transistor*> const* base)
{
    std::vector<Transistor*> abstraction;
    // Preparing the vector prior to storing
    for (Transistor* transistor : *base) // Note that "base" is our own copy. Not a pointer.
    {
        // Peephole optimization
        if (transistor->inputs.size() == 1 && transistor->outputs.size() == 1) // Transistor is buffer (a--->b/a-!->b without branching or converging)
        {
            /********************
            *
            *   a--->b--->c
            *   becomes
            *   a-------->c
            *
            *   inTransistor<-inputs[0]->(outTransistor == transistor) && (transistor == inTransistor)<-outputs[0]->outTransistor
            *   transistor->outputs[0]->inTransistor == transistor
            *   transistor->inputs[0]->outTransistor == transistor
            *
            *   So we want to instead make it
            *   inputs[0]->outTransistor = outputs[0]->outTransistor
            *   inTransistor<-inputs[0] = inTransistor<-outputs[0]
            *
            ********************/
            transistor->inputs[0]->outTransistor = transistor->outputs[0]->outTransistor;
            transistor->outputs[0]->inTransistor = transistor->inputs[0]->inTransistor;

            if (transistor->type == Transistor::Type::Invert)
            {
                switch (transistor->outputs[0]->inTransistor->type)
                {
                default: case Transistor::Type::Simple:
                    transistor->outputs[0]->inTransistor->type = Transistor::Type::Invert;
                    break;
                case Transistor::Type::Invert:
                    transistor->outputs[0]->inTransistor->type = Transistor::Type::Simple;
                    break;
                }
            }
        }
        else
        {
            // TODO: This definitely won't work.
            abstraction.push_back(transistor);
        }
    }

    for (Transistor* transistor : *base) // Note that "base" is our own copy. Not a pointer.
    {
        ComponentTransistor tempCT;
        internals.push_back(tempCT);

        tempCT.type = transistor->type;

        for (Wire* const& inputWire : transistor->inputs)
        {
            Transistor* const& search = inputWire->inTransistor;
            for (int i = 0; i < base->size(); ++i)
            {
                if ((*base)[i] == search) tempCT.inputs.push_back(i); // The order of the transistors in the base vector should be the same as in the constructed vector
            }
        }

        for (Wire* const& outputWire : transistor->outputs)
        {
            Transistor* const& search = outputWire->outTransistor;
            for (int i = 0; i < base->size(); ++i)
            {
                if ((*base)[i] == search) tempCT.outputs.push_back(i); // The order of the transistors in the base vector should be the same as in the constructed vector
            }
        }
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

void Component::Draw()
{
    // TODO
}
