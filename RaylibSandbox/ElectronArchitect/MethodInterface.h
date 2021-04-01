#pragma once

#ifdef __INHERITANCE__

// Connector to the polymorphic type of Node, Gate, Unit, etc.
using Poly_t = Port;
using Node_t = Node;
using Gate_t = Gate;
using Comp_t = Comp;
using Unit_t = Unit;

template<Derivative> struct DerivType {};
template<> struct DerivType<Derivative::Base> { using Type = Poly_t; };
template<> struct DerivType<Derivative::Node> { using Type = Node_t; };
template<> struct DerivType<Derivative::Gate> { using Type = Gate_t; };
template<> struct DerivType<Derivative::Comp> { using Type = Comp_t; };
template<> struct DerivType<Derivative::Unit> { using Type = Unit_t; };

#define Type_Of_Enum(_enum) DerivType<_enum>::Type

template<class> struct TypeDeriv {};
template<> struct TypeDeriv<Poly_t> { static constexpr Derivative value = Derivative::Base; };
template<> struct TypeDeriv<Node_t> { static constexpr Derivative value = Derivative::Node; };
template<> struct TypeDeriv<Gate_t> { static constexpr Derivative value = Derivative::Gate; };
template<> struct TypeDeriv<Comp_t> { static constexpr Derivative value = Derivative::Comp; };
template<> struct TypeDeriv<Unit_t> { static constexpr Derivative value = Derivative::Unit; };

#define Enum_Of_Type(_type) TypeDeriv<_type>::value

#define Type_Of(what)				what -> GetType()
#define Location_Of(what)			what -> GetLocation()
#define Free_Poly(what)				delete what
#define Create_Poly(type, ...)		new type (__VA_ARGS__)
#define Push_Input(what, input)		what -> Push_I(input)
#define Push_Output(what, output)	what -> Push_O(output)
#define Erase_Input(what, input)	what -> Erase_I(input)
#define Erase_Output(what, output)	what -> Erase_O(output)

template<Derivative D> typename DerivType<D>::Type ConvertType(Poly_t* port);

// Creates a Node with the same information as the passed port (frees port internally!)
template<> Node_t ConvertType<Derivative::Node>(Poly_t* port)
{
	if (port->GetType() == Port::PortType::Node) return (Node*)port; // Already is node

	Node* node = new Node(port->GetLocation());
	std::vector<Port*> inputs = port->GetInputs();
	std::vector<Port*> outputs = port->GetOutputs();

	switch (port->GetType())
	{
	case Port::PortType::Gate: { // Turning a Gate into a Node
		// 2 inputs -> 1 input
		if (inputs[0]) {
			node->Push_I(inputs[0]);
			inputs[0]->Erase_O(port);
			inputs[0]->Push_O(node);
		}
		if (inputs[1]) inputs[1]->Erase_O(port); // Tell all the ex-input that they no longer have this port as an output

		// 1 output -> 1 output(n)
		if (outputs[0]) {
			node->Push_O(outputs[0]);
			outputs[0]->Erase_I(port);
			outputs[0]->Push_I(node);
		}
	}	break;
	}
	g_allPorts.Erase(port);
	delete port;
	g_allPorts.Push(node);
	return node;
}
// Creates a Gate with the same information as the passed port (frees port internally!)
template<> Gate_t ConvertType<Derivative::Gate>(Poly_t* port)
{
	if (port->GetType() == Port::PortType::Gate) return (Gate*)port; // Already is gate

	Gate* gate = new Gate(port->GetLocation());
	std::vector<Port*> inputs = port->GetInputs();
	std::vector<Port*> outputs = port->GetOutputs();

	switch (port->GetType())
	{
	case Port::PortType::Node: { // Turning a Node into a Gate
		// 1 input -> 1 input
		if (inputs[0]) {
			gate->Push_I(inputs[0]);
			inputs[0]->Erase_O(port);
			inputs[0]->Push_O(gate);
		}

		// n outputs -> 1 output
		if (!outputs.empty()) {
			for (Port* out : outputs) {
				out->Erase_I(port); // Tell all the ex-outputs that they no longer have this port as an input
			}
			gate->Push_O(outputs[0]);
			outputs[0]->Push_I(gate);
		}

	}	break;
	}
	g_allPorts.Erase(port);
	delete port;
	g_allPorts.Push(gate);
	return gate;
}

#define Recast_As(what, as) ConvertType<as>(what)

#else
#ifdef __COMPOSITION__

// Connector to the polymorphic type of Node, Gate, Unit, etc.
using Poly_t = Object*;
#define Location_Of(what) what -> GetLocation()

#else
#ifdef __HANDELEROBJ__

// Connector to the polymorphic type of Node, Gate, Unit, etc.
using Poly_t = Handler*;
#define Location_Of(what) what -> GetLocation()

#else

#error Polymorphic method undefined
// No type has been defined!
using Poly_t = void;
using Node = void;
using Gate = void;
using Comp = void;
using Unit = void;

#define Type_Of(what) Derivative::Base
#define Location_Of(what) Vector2{ NAN, NAN }
#define Free_Poly(what) what = (void*)0x00008123;
#define Push_Input(what, input) static_assert(true, "No push function defined!")
#define Push_Output(what, output) static_assert(true, "No push function defined!")
#define Erase_Input(what, input) static_assert(true, "No erase function defined!")
#define Erase_Output(what, output) static_assert(true, "No erase function defined!")

#endif // __HANDELEROBJ__
#endif // __COMPOSITION__
#endif // __INHERITANCE__