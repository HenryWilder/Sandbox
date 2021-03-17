#include <bitset>
#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <unordered_map>
#include <array>
#include <thread>
#include <atomic>
#include <stack>

#include <raylib.h>
#include <raymath.h>


//  __  __ _
// |  \/  (_)
// | \  / |_ ___  ___
// | |\/| | / __|/ __|
// | |  | | \__ \ (__ 
// |_|  |_|_|___/\___|


#pragma region Misc
#pragma region Math

int Log2(int val) {
	int steps = 0;
	while (val) {
		val /= 2;
		++steps;
	}
	return steps;
}

int Roundf(float f) {
	if (f >= 0.0f)
		return ((int)(f + 0.5f));
	else
		return ((int)(f - 0.5f));
}

Vector2 Vector2Round(Vector2 vec) { return Vector2{ (float)Roundf(vec.x), (float)Roundf(vec.y) }; }
Vector2 Vector2Snap(Vector2 vec, float to) { return Vector2Scale(Vector2Round(Vector2Scale(vec, 1.0f / to)), to); }

#pragma endregion
#pragma region Integer positions

struct Int3 {
	Int3() : x(), y(), z() {};
	constexpr Int3(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {};

	Int3(Vector3 pos)
		: x(Roundf(pos.x)), y(Roundf(pos.y)), z(Roundf(pos.z)) {};
	Int3(Vector2 pos, float layer)
		: x(Roundf(pos.x)), y(Roundf(pos.y)), z(Roundf(layer)) {};
	Int3(Vector2 pos, int layer)
		: x(Roundf(pos.x)), y(Roundf(pos.y)), z(layer) {};

	bool operator==(const Int3& b) { return (x == b.x && y == b.y && z == b.z); }
	bool operator!=(const Int3& b) { return (x != b.x || y != b.y || z != b.z); }

	Vector2 xy() { return Vector2{ (float)x, (float)y }; }
	Vector3 xyz() { return Vector3{ (float)x, (float)y, (float)z }; }

	int x, y, z;
};

struct IntRect {
	IntRect() : x1(), y1(), w(), h() {};
	constexpr IntRect(int _x, int _y, int _w, int _h) : x1(_x), y1(_y), w(_w), h(_h) {};

	IntRect(Rectangle rec) : x1(Roundf(rec.x)), y1(Roundf(rec.y)), w(Roundf(rec.width)), h(Roundf(rec.height)) {};

	operator Rectangle() { return Rectangle{ (float)x1, (float)y1, (float)w, (float)h }; }

	// x + w
	int x2() { return x1 + w; }
	// y + h
	int y2() { return y1 + h; }

	int x1, y1, w, h;
};
bool CheckInt3InsideIntRect(Int3 pt, IntRect rec) {
	return (pt.x >= rec.x1 && pt.y <= rec.x2() &&
			pt.y >= rec.y1 && pt.y <= rec.y2());
}

#pragma endregion
#pragma region Drawing

enum class WireShape : char {
	XFirst = 'x',
	YFirst = 'y',

	DiagFirst = 'd',
	DiagLast = 'l',
};
WireShape operator++(WireShape shape) {
	switch (shape) {
	case WireShape::XFirst:		return WireShape::YFirst;
	case WireShape::YFirst:		return WireShape::DiagFirst;
	case WireShape::DiagFirst:	return WireShape::DiagLast;
	case WireShape::DiagLast:	return WireShape::XFirst;
	}
}
WireShape operator-(WireShape shape) {
	switch (shape) {
	case WireShape::XFirst:		return WireShape::DiagLast;
	case WireShape::YFirst:		return WireShape::XFirst;
	case WireShape::DiagFirst:	return WireShape::YFirst;
	case WireShape::DiagLast:	return WireShape::DiagFirst;
	}
}

Color DepthColor(Color color, float z, float target, float zoom) {
	return ColorAlpha(color, 1.0f - ((abs(target - z) * zoom)) / 32);
}
Vector2 JointPos(Int3 start, Int3 end, WireShape shape) {
	if (shape == WireShape::DiagFirst || shape == WireShape::DiagLast)
	{
		float xLength = abs(end.x - start.x);
		float yLength = abs(end.y - start.y);

		float& shortLength = (xLength < yLength ? xLength : yLength);

		Vector2 vectorToEnd = {
			(start.x < end.x ? 1.0f : -1.0f),
			(start.y < end.y ? 1.0f : -1.0f),
		};

		if (shape == WireShape::DiagFirst)
			return Vector2Add(start.xy(), Vector2Scale(vectorToEnd, shortLength));
		else // (shape == WireShape::DiagLast)
			return Vector2Subtract(end.xy(), Vector2Scale(vectorToEnd, shortLength));
	}
	else if (shape == WireShape::XFirst)
		return Vector2{ (float)end.x, (float)start.y };
	else if (shape == WireShape::YFirst)
		return Vector2{ (float)start.x, (float)end.y };
	else
		return start.xy();
}
void DrawWire(Int3 start, Int3 end, WireShape shape, bool b_state, float targetDepth, float zoom) {
	Color color = DepthColor((b_state ? RED : WHITE), start.z, targetDepth, zoom);

	Vector2 joint = JointPos(start, end, shape);

	DrawLineV(start.xy(), joint, color);
	DrawLineV(joint, end.xy(), color);
}

#pragma endregion
#pragma endregion


//   _____                                             _
//  / ____|                                           | |
// | |     ___  _ __ ___  _ __   ___  _ __   ___ _ __ | |_ ___ 
// | |    / _ \| '_ ` _ \| '_ \ / _ \| '_ \ / _ \ '_ \| __/ __|
// | |___| (_) | | | | | | |_) | (_) | | | |  __/ | | | |_\__ \
//  \_____\___/|_| |_| |_| .__/ \___/|_| |_|\___|_| |_|\__|___/
//                       | |
//                       |_|


#pragma region Components
#pragma region Polymorphism

// Makes it possible to dynamic cast to any component type. Because it's easier than using std::variant. TRUST ME.
struct Component {
	Int3 pos;
	// To apease the polymorphism god
	virtual bool IsPolymorphic() = 0;
};

struct Gate;
struct Unit;

// A connection for routing signals between objects of different types. Specifically gates/units.
struct IOPort {
	explicit operator Gate* ();
	explicit operator Unit* ();

	Component* component;
};
// An IOPort with the ability to read the current state of the port
struct IPort : public IOPort {
	bool GetState();
};
// An IOPort
struct OPort : public IOPort {
	// I can't think of anything to put in here. It's kinda just a typedef for the time being.
};

#pragma endregion
#pragma region Components
// Battery/power supply. All circuits must both start & end at a battery.
struct Batt : public Component {
	OPort head;									// Start of a circuit
	IPort tail;									// End of a circuit

	bool IsPolymorphic() override {
		return false;
	}
};

// Effectively a transistor/junction. Variable inputs, logical evaluation, variable outputs.
struct Gate : public Component {
	// Logical eval method
	enum class Mode : int {
		OR = '||',		// Any
		NOR = '!',		// Invert
		AND = '&&',		// All
		XOR = 'xor',	// Exactly 1
	};

	// Returns the current state of the gate (NOTE: does not check if it needs re-evaluation!!)
	bool GetState() {
		return state;
	}

	// Returns the evaluation of the inputs using the supplied evaluation method.
	static bool Eval(Mode method, std::vector<bool> inputs) {
		bool out = (method == Mode::AND);
		bool b_xor = (method == Mode::XOR);

		for (bool b : inputs) {
			if (method == Mode::AND ? !b : b) {
				if (b_xor && out) return false;
				out = b;
				if (!b_xor) break;
			}
		}
		return (method == Mode::NOR ? !out : out);
	}

	bool IsPolymorphic() override {
		return true;
	}

	Mode mode;									// How to handle evaluation
	bool state;									// The current state of the transistor
	std::vector<IPort> in;						// Vector of all inputs to this transistor
	std::vector<OPort> out;						// Vector of all outputs this transistor leads to
};
// Returns the "next" mode. Returns Mode('err') on failure. NOTE: Will not modify passed var!
Gate::Mode operator++(Gate::Mode m) {
	switch (m) {
	case Gate::Mode::OR:  return Gate::Mode::AND;
	case Gate::Mode::NOR: return Gate::Mode::AND;
	case Gate::Mode::AND: return Gate::Mode::XOR;
	case Gate::Mode::XOR: return Gate::Mode::OR;

	default: return Gate::Mode('err');
	}
}
// Returns the "last" mode. Returns Mode('err') on failure. NOTE: Will not modify passed var!
Gate::Mode operator--(Gate::Mode m) {
	switch (m) {
	case Gate::Mode::OR:  return Gate::Mode::XOR;
	case Gate::Mode::NOR: return Gate::Mode::OR;
	case Gate::Mode::AND: return Gate::Mode::NOR;
	case Gate::Mode::XOR: return Gate::Mode::AND;

	default: return Gate::Mode('err');
	}
}

// Acts as an arithmatic unit or self-contained processor circuit. "Typed" inputs, bitwise/arithmatic evaluation, "Typed" outputs.
struct Unit : public Component {
	// Arithmatic/bitwise eval method
	enum class Mode : int {
		OR = '|',								// Bitwise
		AND = '&',								// Bitwise
		XOR = '^',								// Bitwise
		FLP = '~',								// Bitflip (THIS TAKES ONLY 1 PARAMETER, NOT 2)
		LSH = '<<',								// Bitshift left
		RSH = '>>',								// Bitshift right

		LES = '<',								// Less than
		MOR = '>',								// Greater than
		EQU = '==',								// Equal to

		ADD = '+',								// Add
		SUB = '-',								// Subtract
		MUL = '*',								// Multiply
		DIV = '/',								// Divide
		MOD = '%',								// Modulo

		TGL = 'TF',								// T-Flip flop
		RS = 'RS',								// RS Latch (not gated)
		RSG = 'RSG',							// RS Latch (gated)
	};

	// Returns the number of bits used in the unit
	int LanesIn() {
		return lanes_in;
	}
	// Returns the number of bits used in the unit
	int LanesOut() {
		return lanes_out;
	}

	// Returns the evaluation of a & b using the supplied evaluation method.
	static size_t Eval(Mode method, size_t a, size_t b) {
		switch (method) {
		case Unit::Mode::OR:  return (a | b);
		case Unit::Mode::AND: return (a & b);
		case Unit::Mode::XOR: return (a ^ b);
		case Unit::Mode::FLP: return (~a);
		case Unit::Mode::LSH: return (a << b);
		case Unit::Mode::RSH: return (a >> b);

		case Unit::Mode::LES: return (a < b);
		case Unit::Mode::MOR: return (a > b);
		case Unit::Mode::EQU: return (a == b);

		case Unit::Mode::ADD: return (a + b);
		case Unit::Mode::SUB: return (a - b);
		case Unit::Mode::MUL: return (a * b);
		case Unit::Mode::DIV: return (a / b);
		case Unit::Mode::MOD: return (a % b);

		case Unit::Mode::TGL: return (!a);
		case Unit::Mode::RS:  return (a); // TODO These only make sense when operating on the registers
		case Unit::Mode::RSG: return (a); // TODO These only make sense when operating on the registers

		default: return a;
		}
	}

	// Returns the state of a single bit
	bool GetBit(int at) {
		return (state & (1 << at));
	}
	// Returns the state of a single bit
	void SetBit(int at, bool set) {
		if (set) state |= (1 << at);
		else state &= ~(1 << at);
	}

	// Returns a copy of the current mode
	Mode GetMode() {
		return mode;
	}
	// Sets the method this unit should use for evaluation
	void SetMode(Mode _mode) {
		switch (mode = _mode) {
		default:
		case Mode::OR:  case Mode::AND: case Mode::XOR: case Mode::FLP: case Mode::LSH: case Mode::RSH:
		case Mode::ADD: case Mode::SUB: case Mode::MUL: case Mode::DIV: case Mode::MOD:
			// <size> in, <size> out
			lanes_in = 0;
			lanes_out = 0;
			break;

		case Mode::LES: case Mode::MOR: case Mode::EQU:
			// <size> in, 1	out
			lanes_in = 0;
			lanes_out = 1;
			break;

			// Latches
		case Mode::TGL:
			// 1 in, 1 out
			lanes_in = 1;
			lanes_out = 1;
			break;
		case Mode::RS: case Mode::RSG:
			// 2 in, 1 out
			lanes_in = 2;
			lanes_out = 1;
			break;
		}
	}
	// Sets the number of lanes in the unit. Refreshes the mode in the process.
	// This function can also be used to set both width and mode simultaniously!
	void SetWidth(int _lanes, Mode _mode = Mode('err')) {
		if (_mode == Mode('err')) _mode = mode;
		SetMode(_mode);
		if (!LanesIn()) lanes_in = _lanes;
		if (!LanesOut()) lanes_out = _lanes;
	}

	bool IsPolymorphic() override {
		return true;
	}

	Mode mode;									// How to process the states
	size_t state;								// The bits in the component
	int lanes_in, lanes_out;					// How many (of the maximum 64) lanes are used for input/output
	std::vector<IPort> in;						// Transistors to take the state from
	std::vector<OPort> out;						// Transistors to send the state to
};
// Returns the "next" mode. Returns Mode('err') on failure. NOTE: Will not modify passed var!
Unit::Mode operator++(Unit::Mode m) {
	switch (m) {
	case Unit::Mode::OR:  return Unit::Mode::AND;
	case Unit::Mode::AND: return Unit::Mode::XOR;
	case Unit::Mode::XOR: return Unit::Mode::FLP;
	case Unit::Mode::FLP: return Unit::Mode::LSH;
	case Unit::Mode::LSH: return Unit::Mode::RSH;
	case Unit::Mode::RSH: return Unit::Mode::OR;

	case Unit::Mode::LES: return Unit::Mode::MOR;
	case Unit::Mode::MOR: return Unit::Mode::EQU;
	case Unit::Mode::EQU: return Unit::Mode::LES;

	case Unit::Mode::ADD: return Unit::Mode::SUB;
	case Unit::Mode::SUB: return Unit::Mode::MUL;
	case Unit::Mode::MUL: return Unit::Mode::DIV;
	case Unit::Mode::DIV: return Unit::Mode::MOD;
	case Unit::Mode::MOD: return Unit::Mode::ADD;

	case Unit::Mode::TGL: return Unit::Mode::RS;
	case Unit::Mode::RS:  return Unit::Mode::RSG;
	case Unit::Mode::RSG: return Unit::Mode::TGL;

	default: return Unit::Mode('err');
	}
}
// Returns the "last" mode. Returns Mode('err') on failure. NOTE: Will not modify passed var!
Unit::Mode operator--(Unit::Mode m) {
	switch (m) {
	case Unit::Mode::OR:  return Unit::Mode::RSH;
	case Unit::Mode::AND: return Unit::Mode::OR;
	case Unit::Mode::XOR: return Unit::Mode::AND;
	case Unit::Mode::FLP: return Unit::Mode::XOR;
	case Unit::Mode::LSH: return Unit::Mode::FLP;
	case Unit::Mode::RSH: return Unit::Mode::LSH;

	case Unit::Mode::LES: return Unit::Mode::EQU;
	case Unit::Mode::MOR: return Unit::Mode::LES;
	case Unit::Mode::EQU: return Unit::Mode::MOR;

	case Unit::Mode::ADD: return Unit::Mode::MOD;
	case Unit::Mode::SUB: return Unit::Mode::ADD;
	case Unit::Mode::MUL: return Unit::Mode::SUB;
	case Unit::Mode::DIV: return Unit::Mode::MUL;
	case Unit::Mode::MOD: return Unit::Mode::DIV;

	case Unit::Mode::TGL: return Unit::Mode::RSG;
	case Unit::Mode::RS:  return Unit::Mode::TGL;
	case Unit::Mode::RSG: return Unit::Mode::RS;

	default: return Unit::Mode('err');
	}
}

#pragma endregion
#pragma region IO Ports

IOPort::operator Gate*() {
	if (Gate* gate = dynamic_cast<Gate*>(component)) return gate;
	else return nullptr;
}
IOPort::operator Unit*() {
	if (Unit* unit = dynamic_cast<Unit*>(component)) return unit;
	else return nullptr;
}
bool IPort::GetState() {
	if (Gate* gate = (Gate*)component) return gate->GetState();
	if (Unit* unit = (Unit*)component) return unit->GetBit(0); // TODO: This is DEFINITELY wrong.
}
#pragma endregion
#pragma endregion


//   _____            _        _                      
//  / ____|          | |      (_)                     
// | |     ___  _ __ | |_ __ _ _ _ __   ___ _ __ ___  
// | |    / _ \| '_ \| __/ _` | | '_ \ / _ \ '__/ __| 
// | |___| (_) | | | | || (_| | | | | |  __/ |  \__ \ 
//  \_____\___/|_| |_|\__\__,_|_|_| |_|\___|_|  |___/ 


#pragma region Game Data

namespace GameData {
	int g_xWidth  = 010000;
	int g_yHeight = 010000;
	int g_zDepth  = 000020;

	int XWidth()  { return g_xWidth;  }
	int YHeight() { return g_yHeight; }
	int ZDepth()  { return g_zDepth;  }

	int XMax() { return XWidth()  / 2; } int XMin() { return XMax() - XWidth();  }
	int YMax() { return YHeight() / 2; } int YMin() { return YMax() - YHeight(); }
	int ZMax() { return ZDepth()  / 2; } int ZMin() { return ZMax() - ZDepth();  }

	// Handles IDs and component pointers
	namespace IDMap {
		using Key_t			= uint32_t;									constexpr Key_t   null_key = 0;
		using Value_t		= Component*;								constexpr Value_t null_val = nullptr;
		
		using Map_t			= std::unordered_map<Key_t, Value_t>;		Map_t g_map;
		using Iter_t		= typename Map_t::iterator;					Iter_t null_iter;
		using CIter_t		= typename Map_t::const_iterator;
		using KeyList_t		= std::list<Key_t>;							KeyList_t invalids;

		void Init() {
			g_map[null_key] = null_val;
			null_iter = g_map.find(null_key);
		}

		Iter_t begin() {
			return g_map.begin();
		}
		Iter_t end() {
			return g_map.end();
		}
		size_t size() {
			return g_map.size();
		}
		
		Iter_t find(Key_t id) {
			auto it = g_map.find(id);
			if (it != end())
				return it;
			else
				return null_iter;
		}
		bool valid(CIter_t iter) {
			return !((iter == null_iter) || (iter == end()));
		}
		bool valid(Key_t key) {
			return !((key == null_key) || (find(key) == null_iter));
		}

		Key_t push(Value_t val) {
			Key_t id;

			// First check if there's an invalid key we can use
			if (!invalids.empty()) {
				id = invalids.front();
				invalids.pop_front();
			}
			// Create a new id if the list of invalid keys is empty
			else {
				id = (int)size();
			}

			if (id != null_key) g_map[id] = val;
			return id;
		}
		void erase(Key_t id) {
			if (valid(id)) {
				g_map.erase(id);
				invalids.push_back(id);
			}
		}
	}
	using ID_t		= IDMap::Key_t;
	using Comp_t	= IDMap::Value_t;

	// Stores IDs in buckets for the g_order in which they need to be evaluated
	namespace EvalOrder {
		using DepthLayer = std::vector<ID_t>;
		using Order = std::vector<DepthLayer>;
		Order g_order;

		int EvaluationDepth(Component* comp) {
			if (!comp) return 0; // Skip nullptrs

			std::vector<IPort> inputArr;
			if (Gate* gate = (Gate*)comp) inputArr = gate->in;
			else if (Unit* unit = (Unit*)comp) {
				// Unit's inputs are in a stack array with 64 members.
				// We only need the usable subset of that.
				inputArr.reserve(unit->LanesIn());
				for (int i = 0; i < unit->LanesIn(); ++i) {
					inputArr.push_back(unit->in[i]);
				}
			}
			else return 0; // Skip Batteries

			int maxDepth = 0;
			for (IPort& port : inputArr) {
				int portDepth = EvaluationDepth(port.component);
				if (portDepth > maxDepth) maxDepth = portDepth;
			}
			return maxDepth;
		}
		void Organize() {
			Order updated;
			for (DepthLayer& comp : g_order) {
				for (ID_t id : comp) {
					IDMap::Iter_t it = IDMap::find(id);
					if (IDMap::valid(it)) {
						int depth = EvaluationDepth(it->second);
						while ((updated.size() - 1) < depth) {
							updated.push_back(DepthLayer());
						}
						updated[depth].push_back(id);
					}
				}
			}
			g_order = updated;
		}
		void Push(ID_t id) {
			auto it = IDMap::find(id);
			if (IDMap::valid(it)) {
				int depth = EvaluationDepth(it->second);
				while ((g_order.size() - 1) < depth) {
					g_order.push_back(DepthLayer());
				}
				g_order[depth].push_back(id);
			}
		}

		void EvaluateAll() {
			for (DepthLayer& layer : g_order) {
				for (ID_t id : layer) {
					IDMap::Iter_t it = IDMap::find(id);

					if (!IDMap::valid(it)) continue;

					Component* component = it->second;

					if (Gate* gate = (Gate*)component) {
						std::vector<bool> inputs;
						inputs.reserve(gate->in.size());

						for (IPort& port : gate->in) {
							inputs.push_back(port.GetState());
						}

						gate->state = Gate::Eval(gate->mode, inputs);

						continue;
					}

					if (Unit* unit = (Unit*)component) {
						// TODO: evaluation needs to be fixed

						continue;
					}
				}
			}
		}
	}

	// Stores IDs in a quad-tree for making mouse selection fast
	class TreeNode {
	public:
		using Element_t		= ID_t;
		using ElemSet_t		= std::vector<Element_t>;
		using Child_t		= TreeNode*;
		using ChildSet_t	= std::array<Child_t, 4>;

		enum class Quad {
			TL, TR,
			BL, BR,
		};

		TreeNode()
			: coverage(), children{ nullptr, nullptr, nullptr, nullptr }, elements() {};
		TreeNode(IntRect _coverage)
			: coverage(_coverage), children{ nullptr, nullptr, nullptr, nullptr }, elements() {};

		int Navigate(int x, int y) {
			if (children[0]) {
				return
					((int)(x > children[0]->coverage.x2())) |
					((int)(y > children[0]->coverage.y2()) << 1);
			}
			else return -1;
		}
		Child_t NavigateChild(int x, int y) {
			int nav = Navigate(x, y);
			if (nav < 0) return nullptr;
			else return children[nav];
		}

		ElemSet_t GetElements() {
			return elements;
		}

		Element_t At(int x, int y) {
			if (Child_t child = NavigateChild(x, y)) {
				return child->At(x,y);
			}
			else {
				for (Element_t elem : elements) {
					IDMap::Iter_t it = IDMap::find(elem);
					if (IDMap::valid(it)) {
						Comp_t comp = it->second;
						if (comp->pos.x == x && comp->pos.y == y) return elem;
					}
				}
				return IDMap::null_key;
			}
		}

		void Push_At(int x, int y, ID_t what) {
			elements.push_back(what);
			if (Child_t child = NavigateChild(x, y)) child->Push_At(x, y, what);
		}

		void Erase(int x, int y, ID_t what) {
			for (int i = 0; i < elements.size(); ++i) {
				if (elements[i] == what) {
					elements.erase(elements.begin() + i);
					children[Navigate(x, y)]->Erase(x, y, what);
					break;
				}
			}
		}

		IntRect coverage;
		ChildSet_t children;
		ElemSet_t elements;
	};
	std::vector<TreeNode*> m_worldLayers; // Quad-Tree nodes for quickly locating components on the screen

	TreeNode* GetLayer(int depth) {
		int get = depth + ZMax();
		if (get >= 0 && get < m_worldLayers.size()) return m_worldLayers[get];
		else return nullptr;
	}

	void Init() {
		m_worldLayers.resize(ZDepth());
		IntRect worldSpace{ XMin(), YMin(), XWidth(), YHeight() };
		for (TreeNode*& node : m_worldLayers) {
			
			node = new TreeNode();
		}
	}

	size_t XYArea() { return (size_t)XWidth() * (size_t)YHeight(); }
	size_t Volume() { return (size_t)XYArea() * (size_t)ZDepth (); }

	bool InRange(Int3 pos) {
		return (pos.x >= XMin() && pos.x <= XMax() &&
				pos.y >= YMin() && pos.y <= YMax() &&
				pos.z >= ZMin() && pos.z <= ZMax());
	}
	ID_t ID_At(Int3 pos) {
		return m_worldLayers[pos.z + (size_t)ZMax()]->At(pos.x, pos.y);
	}

	ID_t Push(IDMap::Value_t comp, Int3 at) {
		TreeNode* layer = GetLayer(at.z);
		if (layer) {
			ID_t id = IDMap::push(comp); // Push to ID-handler
			layer->Push_At(at.x, at.y, id); // Push to oct-tree
			EvalOrder::Push(id); // Push to evaluator list
			return id;
		}
		return IDMap::null_key;
	}

	void EraseAt(Int3 at) {
		ID_t id = ID_At(at);
		if (id) {
			IDMap::erase(id);
			m_worldLayers[at.z]->Erase(at.x, at.y, id);
		}
	}
}

#pragma endregion


//  __  __       _
// |  \/  |     (_)
// | \  / | __ _ _ _ __
// | |\/| |/ _` | | '_ \
// | |  | | (_| | | | | |
// |_|  |_|\__,_|_|_| |_|


#pragma region Main

int main1() {
	int windowWidth = 1280;
	int windowHeight = 720;

	InitWindow(windowWidth, windowHeight,"Electron Architect");
	SetTargetFPS(30);

	Camera2D camera; {
		camera.offset = { };
		camera.target = { };
		camera.rotation = 0.0f;
		camera.zoom = 8.0f;
	}
	Vector2 cameraPos = {
		(float)windowWidth * 0.5f,
		(float)windowHeight * 0.5f
	};

	float depth = 0.0f;

	auto DrawWireLambda = [&](Int3 start, Int3 end, WireShape shape, bool state) {
		DrawWire(start, end, shape, state, depth, camera.zoom);
	};

	Vector2 mousePos, worldMousePos, mousePos_last, worldMousePos_last;
	mousePos = {0,0};
	worldMousePos = {0,0};

	Int3 wireStart, wireEnd;
	WireShape wireShape = WireShape::XFirst;

	Rectangle worldPixel;
	auto UpdateWorldPixel = [&] {
		Vector2 pos = GetScreenToWorld2D({ 0, 0 }, camera);
		Vector2 width = Vector2Subtract(GetScreenToWorld2D({ 0, 1 }, camera), GetScreenToWorld2D({ 0, 0 }, camera));

		worldPixel.x = pos.x;
		worldPixel.y = pos.y;
		worldPixel.width = width.y;
		worldPixel.height = width.y;
	};	

	GameData::Init();

	while (!WindowShouldClose())
	{
		#pragma region Simulate

		mousePos_last = mousePos;
		mousePos = GetMousePosition();

		worldMousePos_last = worldMousePos;
		worldMousePos = GetScreenToWorld2D(mousePos, camera);
		
		if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
			Vector2 mouse_delta = Vector2Subtract(mousePos, mousePos_last);
			Vector2 worldMouse_delta = Vector2Subtract(worldMousePos, worldMousePos_last);

			cameraPos = Vector2Add(cameraPos, mouse_delta);
		}
		if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
			cameraPos = Vector2Snap(cameraPos, camera.zoom);
		}

		if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
			if (GetMouseWheelMove() > 0.0f)
				camera.zoom *= 2.0f;
			else if (GetMouseWheelMove() < 0.0f)
				camera.zoom *= 0.5f;
		}
		else depth += GetMouseWheelMove();

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			wireStart = Int3(worldMousePos, depth);
		}
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			wireEnd = Int3(worldMousePos, depth);
		}
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			wireEnd = Int3(worldMousePos, depth);
			if (wireEnd != wireStart) {
				// Start gate
				if (GameData::ID_t id = GameData::ID_At(wireStart)) { // Is position taken?
					GameData::IDMap::find(id);
				}
				else {
					Component* temp = new Gate();
					temp->pos = wireStart;
					GameData::Push(temp, wireStart);
				}
				// End gate
				if (GameData::ID_t id = GameData::ID_At(wireEnd)) { // Is position taken?
					GameData::IDMap::find(id);
				}
				else {
					Component* temp = new Gate();
					temp->pos = wireEnd;
					GameData::Push(temp, wireEnd);
				}
			}
		}

		#pragma endregion

		#pragma region Draw

		BeginDrawing(); {

			camera.offset = Vector2Snap(cameraPos, camera.zoom);
			UpdateWorldPixel();

			ClearBackground(BLACK);

			BeginMode2D(camera); {

				{ // Draw the world grid
					float screenRight = worldPixel.x + (worldPixel.width * windowWidth);
					float screenBottom = worldPixel.y + (worldPixel.height * windowHeight);

					for (float y = worldPixel.y; y <= screenBottom; y += (worldPixel.height * camera.zoom)) {
						DrawLineV({ worldPixel.x, y }, { screenRight, y }, {20,20,20,255});
					}
					for (float x = worldPixel.x; x <= screenRight; x += (worldPixel.width * camera.zoom)) {
						DrawLineV({ x, worldPixel.y }, { x, screenBottom }, {20,20,20,255});
					}
				}

				int i = 0;
				for (GameData::EvalOrder::DepthLayer layer : GameData::EvalOrder::g_order) {
					for (GameData::ID_t id : layer) {
						Component* comp = GameData::IDMap::find(id)->second;
						DrawCircleV(comp->pos.xy(), 1, BLUE);
						DrawText(TextFormat("{ %i, %i, %i }", comp->pos.x, comp->pos.y, comp->pos.z), 0, i += 8, 8, WHITE);
					}
				}

				if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
					DrawWireLambda(wireStart, wireEnd, wireShape, false);
				}

			} EndMode2D();

			//DrawLineV(mousePos_last, mousePos, WHITE);

			DrawText(TextFormat("Mouse at: { %i, %i, %i }\nTotal components: %i", (int)Roundf(worldMousePos.x), (int)Roundf(worldMousePos.y), (int)Roundf(depth), GameData::IDMap::size()), 0, 0, 8, WHITE);
			//DrawText(TextFormat("{ %f, %f }", worldMousePos.x, worldMousePos.y), mousePos.x, mousePos.y + 10, 8, WHITE);
			//DrawText(TextFormat("%f", camera.zoom), mousePos.x, mousePos.y + 20, 8, WHITE);

		} EndDrawing();

		#pragma endregion
	}

	CloseWindow();
}

#pragma endregion
