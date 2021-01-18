#include "Abstraction.h"
#include "Wire.h"


AbstractComponent::AbstractComponent(std::vector<Transistor*> base)
{
    std::vector<std::vector<Transistor*>::iterator> elementsToErase;
    // Preparing the vector prior to storing
    for (Transistor* transistor : base) // Note that "base" is our own copy. Not a pointer.
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
            elementsToErase.push_back(std::find(base.begin(), base.end(), transistor));
        }
    }
    for (std::vector<Transistor*>::iterator element : elementsToErase)
    {
        base.erase(element); // TODO: May cause bugs. Not sure, but look into this if errors are popping up.
    }

    for (Transistor* transistor : base) // Note that "base" is our own copy. Not a pointer.
    {
        ComponentTransistor tempCT;
        internals.push_back(tempCT);

        tempCT.type = transistor->type;

        if (transistor->OutputOnly())
        {
            inputs.push_back(internals.size() - 1); // Push back current index
        }
        else
        {
            for (Wire* const& inputWire : transistor->inputs)
            {
                Transistor* const& search = inputWire->inTransistor;
                for (int i = 0; i < base.size(); ++i)
                {
                    if (base[i] == search) tempCT.inputs.push_back(i); // The order of the transistors in the base vector should be the same as in the constructed vector
                }
            }
        }

        if (transistor->InputOnly())
        {
            outputs.push_back(internals.size() - 1); // Push back current index
        }
        else
        {
            for (Wire* const& outputWire : transistor->outputs)
            {
                Transistor* const& search = outputWire->outTransistor;
                for (int i = 0; i < base.size(); ++i)
                {
                    if (base[i] == search) tempCT.outputs.push_back(i); // The order of the transistors in the base vector should be the same as in the constructed vector
                }
            }
        }
    }
}

void AbstractComponent::Spawn(Vector2 position)
{
    Component newComponent;
    newComponent.internalCount = this->internals.size();
    newComponent.inputCount = this->inputs.size();
    newComponent.outputCount = this->outputs.size();

    newComponent.internals = new ComponentTransistor[newComponent.internalCount];
    for (int i = 0; i < newComponent.internalCount; ++i)
    {
        newComponent.internals[i] = this->internals[i];
    }

    newComponent.inputs = new Transistor[newComponent.inputCount];
    for (int i = 0; i < newComponent.inputCount; ++i)
    {
        newComponent.inputs[i].pos.x = position.x - (8.0f);
        newComponent.inputs[i].pos.y = position.y + (4.0f * (float)i);
    }

    newComponent.outputs = new Transistor[newComponent.outputCount];
    for (int i = 0; i < newComponent.outputCount; ++i)
    {
        newComponent.outputs[i].pos.x = position.x + (8.0f);
        newComponent.outputs[i].pos.y = position.y + (4.0f * (float)i);
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
