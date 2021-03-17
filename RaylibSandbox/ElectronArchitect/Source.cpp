#include <vector>
#include <deque>
#include <queue>
#include <set>
#include <unordered_set>
#include <list>
#include <stack>
#include <raylib.h>
#include <raymath.h>

#pragma region Objects

class PortList;

// Base class connection point
// (Node*)(Port*) does NOT do the same thing as dynamic_cast<Node*>(Port*) !!!!!!
class Port {
protected:
	// Worldspace location of the Port
	Vector2 position;

public:
	// There is no default constructor. A world position is required.
	Port(Vector2 _pos) : position(_pos) {}

	// Enumeration of all Port child variants
	enum class PortType { Node, Gate };
	// Returns what child class varient this is.
	virtual PortType GetType() = 0;

	// Returns the state of the given bit. (bit is only needed in Cable context)
	virtual bool GetValue(int bit = 0) = 0;

	// Returns the current world position of the Port
	Vector2 GetLocation() { return position; }
	// Sets the current world position of the Port to the provided value
	void SetLocation(Vector2 location) { position = location; }

	// Returns the next Port*.
	virtual Port* Next(size_t path) = 0;
	// Returns the previous Port*.
	virtual Port* Prev(size_t path) = 0;

	// Returns the Port* which got overwritten to make the push possible. Returns nullptr if push succeeded without overwrite
	virtual Port* Push_I(Port* input) = 0;
	// Returns the Port* which got overwritten to make the push possible. Returns nullptr if push succeeded without overwrite
	virtual Port* Push_O(Port* output) = 0;

	// Erase what from the input(s), if it is there. Returns true on success.
	virtual bool Erase_I(Port* what) = 0; 
	// Erase what from the output(s), if it is there. Returns true on success.
	virtual bool Erase_O(Port* what) = 0;

	// Returns the number of inputs currently accessible
	virtual size_t I_Size() = 0;
	// Returns the number of outputs currently accessible
	virtual size_t O_Size() = 0;

	// Returns the number of inputs that could be used
	virtual size_t I_Capacity() = 0;
	// Returns the number of outputs that could be used
	virtual size_t O_Capacity() = 0;
};

// Nodes can split, but not combine.
class Node : public Port {
protected:
	Port* i;
	std::vector<Port*> o;

public:
	friend PortList;

	Node(Vector2 _pos) : Port(_pos), i(nullptr), o({}) {}

	PortType GetType() override { return PortType::Node; }

	bool GetValue(int bit = 0) override {
		if (i) return i->GetValue();
		else return false; // Node is start of circuit
	}

	Port* Next(size_t path) override { return o[path]; }
	Port* Prev(size_t path = 0) override { return i; }

	Port* Push_I(Port* input) override {
		Port* _i = i;
		i = input;
		return _i;
	}
	Port* Push_O(Port* output) override {
		o.push_back(output);
		return nullptr;
	}
	// Erase "what", if it is the input.
	// Returns true if the erasure was successful or input is already free.
	// Returns false if "what" was not the input.
	bool Erase_I(Port* what) override {
		return !(i = (i == what ? nullptr : i));
	}
	// Erase "what", if it is an output.
	// Returns true if the erasure was successful.
	// Returns false if "what" was not an output.
	bool Erase_O(Port* what) override {
		int i = 0;
		for (Port* port : o) {
			if (port == what) {
				o.erase(o.begin() + i);
				return true;
			}
			++i;
		}
		return false;
	}

	size_t I_Size() override { return (i ? 1 : 0); }
	size_t O_Size() override { return o.size(); }

	size_t I_Capacity() override { return 1; }
	size_t O_Capacity() override { return SIZE_MAX; }
};

// Gates can combine (exactly 2), but not split.
class Gate : public Port {
private:
	struct { Port* a; Port* b; } i;
	Port* o;
	bool b_evaluated, b_state;

public: enum class Method : char { OR = '|', NOT = '!', AND = '&', XOR = '^' }; private: Method evm; // Evaluation Method

public:
	friend PortList;

	Gate(Vector2 _pos) : Port(_pos), i({ nullptr, nullptr }), o(nullptr), b_evaluated(false), b_state(false), evm(Method::OR) {}

	void SetMethod(Method evalMethod) { evm = evalMethod; }

	PortType GetType() override final { return PortType::Gate; }

	bool Evaluate() {
		bool b_not = (evm == Method::NOT);
		if (i.a) {
			bool a = i.a->GetValue();
			if (b_not) return !a; // If evm is !, it will exit before the following occurs
			else if (i.b) {
				bool b = i.b->GetValue();
				switch (evm)
				{
				default:
				case Method::OR:  return (a || b);
				case Method::AND: return (a && b);
				case Method::XOR: return (a ? !b : b);
				}
			}
		}
		return b_not;
	}

	bool GetValue(int bit) override final {
		if (!b_evaluated) {
			b_state = Evaluate();
			b_evaluated = true;
		}
		return b_state;
	}
	void FrameEndReset() { b_evaluated = false; }

	Port* Next(size_t path) override { return o; }
	Port* Prev(size_t path) override { return (path ? i.b : i.a); }

	Port* Push_I(Port* input) override {
		if (i.a) {
			Port* _b = i.b;
			i.b = input;
			return _b;
		}
		else i.a = input;
		return nullptr;
	}
	Port* Push_O(Port* output) override {
		Port* _o = o;
		o = output;
		return _o;
	}
	// Erase "what", if it is an input.
	// Returns true if the erasure was successful.
	// Returns false if "what" was not an input.
	bool Erase_I(Port* what) override {
		if (i.a == what) { i.a = nullptr; return true; }
		if (i.b == what) { i.b = nullptr; return true; }
		return false;
	}
	// Erase "what", if it is the output.
	// Returns true if the erasure was successful or if output is already free.
	// Returns false if "what" was not the output.
	bool Erase_O(Port* what) override { return !(o = (o == what ? nullptr : o)); }

	size_t I_Size() override { return ((i.a ? 1ull : 0ull) + (i.b ? 1ull : 0ull)); }
	size_t O_Size() override { return (o ? 1 : 0); }

	size_t I_Capacity() override { return 2; }
	size_t O_Capacity() override { return 1; }
};

// TODO: Add Units

#pragma endregion

#pragma region Containers

class PortList {
private:
	std::vector<Port*> ports;

public:
	auto begin() { return ports.begin(); }
	auto end() { return ports.end(); }

	Port* Get(size_t i) { return ports[i]; }

	// ONLY call this if it's certain "what" has no external references!!
	// If "what" has the possibility of external references, please call Wipe() instead!
	void Erase(size_t index) { ports.erase(ports.begin() + index); }
	// ONLY call this if it's certain "what" has no external references!!
	// If "what" has the possibility of external references, please call Wipe() instead!
	void Erase(Port* what) {
		size_t i = 0;
		for (; i < ports.size(); ++i) { if (ports[i] == what) break; }
		Erase(i);
	}

	void Push(Port* port) { ports.push_back(port); }
	void Reserve(int reservation) { if ((ports.size() + reservation) > ports.capacity()) ports.reserve(ports.size() + reservation); }

	size_t Size() { return ports.size(); }

	Port* FindAtPos(Vector2 at) {
		for (Port* port : ports) {
			if (port && Vector2Distance(at, port->GetLocation()) < 1.0f)
				return port;
		}
		return nullptr;
	}
	Port* FindAtPos(Vector2 at, Port* avoid) {
		for (Port* port : ports) {
			if ((port) && (Vector2Distance(at, port->GetLocation()) < 1.0f) && (port != avoid))
				return port;
		}
		return nullptr;
	}
	Port* FindAtPos(Vector2 at, std::unordered_set<Port*>& avoid) {
		for (Port* port : ports) {
			if ((port) && (Vector2Distance(at, port->GetLocation()) < 1.0f) && (avoid.find(port) == avoid.end()))
				return port;
		}
		return nullptr;
	}
	Port* FindAtPos(Vector2 at, std::vector<Port*>& avoid) {
		// If it would be computationally cheeper to do so, convert the vector to an unordered_set so we can have O(c) find() operations.
		if ((avoid.size() > 10)) { // Must be trying to avoid more than 10 elements
			std::unordered_set<Port*> avoidSet;
			for (Port* port : avoid) { avoidSet.insert(port); }
			return FindAtPos(at, avoidSet);
		}
		else {
			auto VectorContains = [&avoid](Port* target) {
				for (Port* port : avoid) {
					if (port == target) return true;
				}
				return false;
			};
			for (Port* port : ports) {
				if ((port) && (Vector2Distance(at, port->GetLocation()) < 0.5f) && (!VectorContains(port)))
					return port;
			}
			return nullptr;
		}
	}

	void Wipe(Port* target) {
		std::vector<Port*>::iterator it = ports.end();
		int i = 0;
		for (Port* port : ports) {
			it = ports.begin() + i;
			switch (port->GetType())
			{
			case Port::PortType::Gate: {
				Gate* gate = (Gate*)port;
				if (gate->i.a == target) gate->i.a = nullptr;
				if (gate->i.b == target) gate->i.b = nullptr;
				if (gate->o == target) gate->o = nullptr;
			} break;
			case Port::PortType::Node: {
				Node* node = (Node*)port;
				if (node->i == target) node->i = nullptr;
				for (Port*& o : node->o) {
					if (o == target) o = nullptr;
				}
			} break;
			}
			++i;
		}
		if (it != ports.end()) ports.erase(it);
	}
	void Wipe(std::unordered_set<Port*>& hitlist) {
		std::stack<int> its;
		int i = 0;
		for (Port* port : ports) {
			if (hitlist.find(port) != hitlist.end()) its.push(i);
			switch (port->GetType())
			{
			case Port::PortType::Gate: {
				Gate* gate = (Gate*)port;
				if (hitlist.find(gate->i.a) != hitlist.end()) gate->i.a = nullptr;
				if (hitlist.find(gate->i.b) != hitlist.end()) gate->i.b = nullptr;
				if (hitlist.find(gate->o) != hitlist.end()) gate->o = nullptr;
			} break;
			case Port::PortType::Node: {
				Node* node = (Node*)port;
				if (hitlist.find(node->i) != hitlist.end()) node->i = nullptr;
				for (Port*& o : node->o) {
					if (hitlist.find(o) != hitlist.end()) o = nullptr;
				}
			} break;
			}
			++i;
		}
		while (!its.empty()) {
			ports.erase(ports.begin() + its.top());
			its.pop();
		}
	}
	
	void Replace(Port* target, Port* with) {
		std::vector<Port*>::iterator it = ports.end();
		int i = 0;
		for (Port* port : ports) {
			if (port == target) it = ports.begin() + i;
			switch (port->GetType())
			{
			case Port::PortType::Gate: {
				Gate* gate = (Gate*)port;
				if (gate->i.a == target) gate->i.a = with;
				if (gate->i.b == target) gate->i.b = with;
				if (gate->o == target) gate->o = with;
			} break;
			case Port::PortType::Node: {
				Node* node = (Node*)port;
				if (node->i == target) node->i = with;
				for (Port*& o : node->o) {
					if (o == target) o = with;
				}
			} break;
			}
			++i;
		}
		if (it != ports.end()) {
			*it = with;
		}
	}
};
PortList g_allPorts;

#pragma endregion

//  __  __       _
// |  \/  |     (_)
// | \  / | __ _ _ _ __
// | |\/| |/ _` | | '_ \
// | |  | | (_| | | | | |
// |_|  |_|\__,_|_|_| |_|

bool Positive(float value) { return value >= 0.0f; }
float Sign(float value) {
	if (value > 0.0f) return 1.0f;
	if (value < 0.0f) return -1.0f;
	return 0.0f;
}
float Roundf(float value) { return (float)(int)(value + (0.5f * Sign(value))); }
int Round(float value) { return (int)(value + (0.5f * Sign(value))); }
Vector2 Roundf(Vector2 vec) { return { Roundf(vec.x), Roundf(vec.y) }; }

float Shift(float value, int shift) {
	if (shift == 0) return value;
	if (value > 1.0f) {
		if (shift > 0) return (float)(Round(value) <<  shift);
		if (shift < 0) return (float)(Round(value) >> -shift);
	}
	else {
		if (shift > 0) return (value * (1 << shift));
		if (shift < 0) return (value / (1 << -shift));
	}
	return value;
}
float Shift(float value, float shift) { return Shift(value, Round(shift)); }

// Returns end but moved to the best place route I decided
Vector2 QueenRules(Vector2 start, Vector2 end) {
	start = Roundf(start);
	end = Roundf(end);
	Vector2 dist = Vector2Subtract(end, start);
	float* shorter;
	float* longer;
	// Wider than tall
	if (abs(dist.x) > abs(dist.y)) {
		longer = &dist.x;
		shorter = &dist.y;
	}
	// Taller than wide
	else {
		longer = &dist.y;
		shorter = &dist.x;
	}
	if (abs(*shorter) > (abs(*longer) * 0.5f)) { // Closer to diagonal
		if (Positive(*longer) != Positive(*shorter)) *longer = -(*shorter);
		else *longer = *shorter;
	}
	else { // Closer to cardinal
		*shorter = 0.0f;
	}
	return Vector2Add(start, dist);
}

Shader shader_wire;
Shader shader_wire_active;
Shader shader_cable;
Texture g_defaultTex;

void DrawCordShaded(Vector2 start, Vector2 end, Color color, float thickness, Shader& shader) {
	Rectangle src; {
		src.x = start.x;
		src.y = start.y;
		src.width = Vector2Distance(start, end);
		src.height = thickness;
	}
	float rot = Vector2Angle(start, end);

	DrawCircleV(start, thickness * 0.5f, color);
	DrawCircleV(end, thickness * 0.5f, color);
	// Cord shader
	BeginShaderMode(shader); {

		DrawTexturePro(g_defaultTex, {0,0,1,1}, src, { 0.0f, 0.5f * src.height }, rot, WHITE);

	} EndShaderMode();
}
void DrawCord(Vector2 start, Vector2 end, Color color) {
	DrawLineV(start, end, color);
}
void DrawWire(Vector2 start, Vector2 end, bool state) { DrawCordShaded(start, end, WHITE, 0.2f, (state ? shader_wire_active : shader_wire)); }
void DrawCable(Vector2 start, Vector2 end) { DrawCordShaded(start, end, WHITE, 0.5f, shader_cable); }

#pragma region Main

int main() {
	int windowWidth = 1280;
	int windowHeight = 720;

	InitWindow(windowWidth, windowHeight, "Electron Architect");
	SetTargetFPS(30);

	shader_wire = LoadShader(0, "wire.frag");
	shader_wire_active = LoadShader(0, "wire_active.frag");
	shader_cable = LoadShader(0, "cable.frag");

	Image img = GenImageColor(1,1,WHITE);
	g_defaultTex = LoadTextureFromImage(img);
	UnloadImage(img);

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

	Vector2 mousePos, worldMousePos, mousePos_last, worldMousePos_last;
	mousePos = { 0,0 };
	worldMousePos = { 0,0 };

	Vector2 wireStart{};
	Vector2 wireEnd{};

	Rectangle worldPixel;
	auto UpdateWorldPixel = [&] {
		Vector2 pos = GetScreenToWorld2D({ 0, 0 }, camera);
		Vector2 width = Vector2Subtract(GetScreenToWorld2D({ 0, 1 }, camera), GetScreenToWorld2D({ 0, 0 }, camera));

		worldPixel.x = pos.x;
		worldPixel.y = pos.y;
		worldPixel.width = width.y;
		worldPixel.height = width.y;
	};

	std::unordered_set<Port*> selection;

	enum class InputMode { Norm, Alt } mode = InputMode::Norm;

	while (!WindowShouldClose()) {
#pragma region Simulate
		mousePos_last = mousePos;
		mousePos = GetMousePosition();

		worldMousePos_last = worldMousePos;
		worldMousePos = GetScreenToWorld2D(mousePos, camera);

		if (GetMouseWheelMove() != 0.0f) {
			camera.zoom = Shift(camera.zoom, GetMouseWheelMove());

			cameraPos = Vector2Scale(Roundf(Vector2Scale(cameraPos, 1.0f / camera.zoom)), camera.zoom); // Update camera position now that zoom has been changed
		}

		switch (mode)
		{
		default:
		case InputMode::Norm:
			// Middle mouse drag
			if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
				Vector2 mouse_delta = Vector2Subtract(mousePos, mousePos_last);
				Vector2 worldMouse_delta = Vector2Subtract(worldMousePos, worldMousePos_last);

				cameraPos = Vector2Scale(Roundf(Vector2Scale(Vector2Add(cameraPos, mouse_delta), 1.0f / camera.zoom)), camera.zoom);
			}
			// M1 Press
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				wireStart = Roundf(worldMousePos);
			}
			// M1 Dragging
			if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				wireEnd = Roundf(QueenRules(wireStart, worldMousePos));
			}
			// M1 Release
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				// If the start and end points aren't within a unit of each other
				if (Vector2Distance(wireEnd, wireStart) > 0.9) {
					char neededAllocs = 0; // Bitmask for whether start & end need to be pushed or if they already exist
					Port* start; // The port at the start of the wire
					start = g_allPorts.FindAtPos(wireStart); // See if there is already an existing port at that position
					if (!start) { // If none was found,
						start = new Node(wireStart); // Initialize a new Node (derivitive of Port) at the location
						neededAllocs |= 0b01; // Prepare the ports global for an expected push
					}
					Port* end; // The port at the end of the wire
					end = g_allPorts.FindAtPos(wireEnd); // See if there is already an existing port at that position
					if (!end) { // If none was found,
						end = new Node(wireEnd); // Initialize a new Node at the location
						neededAllocs |= 0b10; // Prepare the ports global for an expected push
					}

					switch (neededAllocs)
					{
					case 0b01: g_allPorts.Push(start);	break; // Push only start
					case 0b10: g_allPorts.Push(end);	break; // Push only end
					case 0b11: // Push both
						g_allPorts.Reserve(2); // Reserve space for the 2 new elements
						g_allPorts.Push(start);
						g_allPorts.Push(end);
						break;
					default: break;
					}
					start->Push_O(end); // Push end as an output of start
					end->Push_I(start); // Push start as an input of end
				}
			}
			// M2 press
			if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				mode = InputMode::Alt;
			}
			break;
		case InputMode::Alt:
			// Middle press
			if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
				selection.insert(g_allPorts.FindAtPos(worldMousePos));
			}
			if (!selection.empty()) {
				// Middle release
				if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
					for (Port* port : selection) {
						if (Port* existing = g_allPorts.FindAtPos(worldMousePos, selection)) {
							g_allPorts.Erase(existing); // First we remove the port from the world, so its source doesn't get overwritten
							g_allPorts.Replace(existing, port); // Then we overwrite every reference to it
							port->Push_I(existing->Prev(0)); // Replace port's input with existing's input
							delete existing; // And finally free it
						}
					}
					selection.clear();
				}
				// Middle dragging
				if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
					Vector2 mouse_delta = Vector2Subtract(mousePos, mousePos_last);
					Vector2 worldMouse_delta = Vector2Subtract(worldMousePos, worldMousePos_last);

					for (Port* port : selection) {
						port->SetLocation(Roundf(worldMousePos));
					}
				}
			}
			// M1 Press
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				Vector2 pos = Roundf(worldMousePos);
				if (Port* port = g_allPorts.FindAtPos(pos)) { // Port was found
					Gate* gate = dynamic_cast<Gate*>(port);
					if (!gate) { // If there is no gate, create one.
						gate = new Gate(pos);
						if (port->O_Size()) gate->Push_O(port->Next(0));
						g_allPorts.Replace(port, gate);
					}
					else { // If there is a gate, delete it.
						Port* node = new Node(pos);
						if (port->O_Size()) node->Push_O(port->Next(0));
						g_allPorts.Replace(port, node);
					}
					delete port;
				}
				else g_allPorts.Push(new Gate(pos));
			}
			// M2 Press
			if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				mode = InputMode::Norm;
			}
			break;
		}

		

#pragma endregion

#pragma region Draw

		BeginDrawing(); {

			camera.offset = cameraPos;
			UpdateWorldPixel();

			ClearBackground((mode == InputMode::Alt ? Color{ 20, 15, 0, 255 } : Color{ 0, 5, 10, 255 }));

			BeginMode2D(camera); {

				{ // Draw the world grid
					float screenRight = worldPixel.x + (worldPixel.width * windowWidth);
					float screenBottom = worldPixel.y + (worldPixel.height * windowHeight);

					for (float y = worldPixel.y; y <= screenBottom; y += (worldPixel.height * camera.zoom)) {
						DrawLineV({ worldPixel.x, y }, { screenRight, y }, { 20,20,20,255 });
					}
					for (float x = worldPixel.x; x <= screenRight; x += (worldPixel.width * camera.zoom)) {
						DrawLineV({ x, worldPixel.y }, { x, screenBottom }, { 20,20,20,255 });
					}
				}

				size_t i = 0;
				std::unordered_set<Port*> nullbodies;
				// Draw wires
				for (Port* port : g_allPorts) {
					if (port) {
						Vector2 pt = port->GetLocation();
						bool state = port->GetValue();
						switch (port->GetType())
						{
						case Port::PortType::Gate:
							if (Port* next = port->Next(0))
								DrawWire(pt, next->GetLocation(), state);
							break;

						case Port::PortType::Node:
							if (port->O_Size()) {
								for (size_t j = 0; j < port->O_Size(); ++j) {
									DrawWire(pt, port->Next(j)->GetLocation(), state);
								}
							}
							else if (!port->I_Size()) nullbodies.insert(port);
							break;

						default:
							break;
						}
					}
					++i;
				}
				g_allPorts.Wipe(nullbodies);

				// Draw ports
				for (Port* port : g_allPorts) {
					if (port) {
						Vector2 pt = port->GetLocation();
						switch (port->GetType())
						{
						case Port::PortType::Gate:
							DrawCircleV(pt, 1, BLUE);
							break;

						case Port::PortType::Node:
							DrawCircleV(pt, .1f, BLUE);
							break;

						default:
							break;
						}
					}
				}

				if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && (mode == InputMode::Norm)) { DrawCord(wireStart, wireEnd, GRAY); }

			} EndMode2D();

			for (Port* port : g_allPorts) { if (Gate* gate = dynamic_cast<Gate*>(port)) gate->FrameEndReset(); }

			//DrawLineV(mousePos_last, mousePos, WHITE);

			DrawText(TextFormat("Mouse at: { %#1.1f, %#1.1f }\nTotal components: %i", worldMousePos.x, worldMousePos.y, g_allPorts.Size()), 0, 0, 8, WHITE);
			//DrawText(TextFormat("{ %f, %f }", worldMousePos.x, worldMousePos.y), mousePos.x, mousePos.y + 10, 8, WHITE);
			//DrawText(TextFormat("%f", camera.zoom), mousePos.x, mousePos.y + 20, 8, WHITE);

		} EndDrawing();

#pragma endregion
	}

	for (Port* port : g_allPorts) { if (port) delete port; }

	UnloadShader(shader_wire);
	UnloadShader(shader_wire_active);
	UnloadShader(shader_cable);

	UnloadTexture(g_defaultTex);

	CloseWindow();
}

#pragma endregion