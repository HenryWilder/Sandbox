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

class Port {
protected:
	Vector2 position;

public:
	friend PortList;

	Port(Vector2 _pos) : position(_pos) {}

	enum class PortType { Node, Gate };
	virtual PortType GetType() = 0;

	virtual bool GetValue(int bit = 0) = 0;

	Vector2 GetLocation() { return position; }
	void SetLocation(Vector2 location) { position = location; }

	virtual Port* operator+(int path) = 0; // Returns the next Port*. Use 0 as a default.
	virtual Port* operator-(int path) = 0; // Returns the previous Port*. Use 0 as a default.

	virtual Port* Push_I(Port* input) = 0;  // Returns the Port* which got overwritten to make the push possible. Returns nullptr if push succeeded without overwrite
	virtual Port* Push_O(Port* output) = 0; // Returns the Port* which got overwritten to make the push possible. Returns nullptr if push succeeded without overwrite

	virtual bool Erase_I(Port* what) = 0;
	virtual bool Erase_O(Port* what) = 0;

	virtual int I_Size() = 0; // Returns the number of inputs
	virtual int O_Size() = 0; // Returns the number of outputs
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

	Port* operator+(int path) override { return o[path]; }
	Port* operator-(int path) override { return i; }

	Port* Push_I(Port* input) override {
		i = input;
	}
	Port* Push_O(Port* output) override {
		o.push_back(output);
	}
	bool Erase_I(Port* what) override {
		bool is = (i == what);
		if (is) i = nullptr;
		return is;
	}
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

	int I_Size() override { return 1; }
	int O_Size() override { return o.size(); }
};

// Gates can combine (exactly 2), but not split.
class Gate : public Port {
private:
	struct { Port* a; Port* b; } i;
	Port* o;
	bool b_evaluated, b_state;

	enum class Method : char { OR = '|', NOT = '!', AND = '&', XOR = '^' } evm; // Evaluation Method

public:
	friend PortList;

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

	Port* operator+(int path) override { return o; }
	Port* operator-(int path) override { return (path ? i.b : i.a); }

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
	bool Erase_I(Port* what) override {
		if (i.a == what) { return !(i.a = nullptr); }
		if (i.b == what) { return !(i.b = nullptr); }
		return false;
	}
	bool Erase_O(Port* what) override {
		return !(o = (o == what ? nullptr : o));
	}

	int I_Size() override { return 2; }
	int O_Size() override { return 1; }
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

	void Push(Port* port) { ports.push_back(port); }

	size_t Size() { return ports.size(); }

	Port* FindAtPos(Vector2 at) {
		for (Port* port : ports) {
			if (port && Vector2Distance(at, port->GetLocation()) < 1.0f)
				return port;
		}
		return nullptr;
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
	if (value == 0.0f) return 0.0f;
	if (value > 0.0f) return 1.0f;
	if (value < 0.0f) return -1.0f;
}
float Round(float value) { return (float)(int)(value + (0.5f * Sign(value))); }
Vector2 Round(Vector2 vec) { return { Round(vec.x), Round(vec.y) }; }
void DrawCord(Vector2 start, Vector2 end, Color color) {
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
	// Closer to diagonal
	if (abs(*shorter) > (abs(*longer) * 0.5f)) {
		if (Positive(*longer) != Positive(*shorter)) *longer = -(*shorter);
		else *longer = *shorter;
	}
	// Closer to cardinal
	else {
		*shorter = 0.0f;
	}
	end = Vector2Add(start, dist);
	DrawLineV(start, end, color);
}
void DrawWire(Vector2 start, Vector2 end, bool state) { DrawCord(start, end, (state ? RED : WHITE)); }
void DrawCable(Vector2 start, Vector2 end) { DrawCord(start, end, BLUE); }

#pragma region Main

int main() {
	int windowWidth = 1280;
	int windowHeight = 720;

	InitWindow(windowWidth, windowHeight, "Electron Architect");
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

	Vector2 mousePos, worldMousePos, mousePos_last, worldMousePos_last;
	mousePos = { 0,0 };
	worldMousePos = { 0,0 };

	Vector2 wireStart, wireEnd;

	Rectangle worldPixel;
	auto UpdateWorldPixel = [&] {
		Vector2 pos = GetScreenToWorld2D({ 0, 0 }, camera);
		Vector2 width = Vector2Subtract(GetScreenToWorld2D({ 0, 1 }, camera), GetScreenToWorld2D({ 0, 0 }, camera));

		worldPixel.x = pos.x;
		worldPixel.y = pos.y;
		worldPixel.width = width.y;
		worldPixel.height = width.y;
	};

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
		if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {}

		if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
			if (GetMouseWheelMove() > 0.0f)
				camera.zoom *= 2.0f;
			else if (GetMouseWheelMove() < 0.0f)
				camera.zoom *= 0.5f;
		}
		//else depth += GetMouseWheelMove();

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			wireStart = Round(worldMousePos);
		}
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			wireEnd = Round(worldMousePos);
		}
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			wireEnd = Round(worldMousePos);
			if (Vector2Distance(wireEnd, wireStart) > 0.9) {
				Port* start = g_allPorts.FindAtPos(wireStart);
				if (!start) g_allPorts.Push(start = new Node(wireStart));
				Port* end = g_allPorts.FindAtPos(wireEnd);
				if (!end) g_allPorts.Push(end = new Node(wireEnd));
				start->Push_O(end);
				end->Push_I(start);
			}
		}

#pragma endregion

#pragma region Draw

		BeginDrawing(); {

			camera.offset = cameraPos;
			UpdateWorldPixel();

			ClearBackground(BLACK);

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

				int i = 0;
				for (Port* port : g_allPorts) {
					Vector2 pt = port->GetLocation();
					DrawCircleV(pt, 1, BLUE);
					switch (port->GetType())
					{
					case Port::PortType::Node:
						DrawWire(port->GetLocation(), (port + 0)->GetLocation(), port->GetValue());
						break;
					case Port::PortType::Gate:
						for () {
							DrawWire(port->GetLocation(), (port + 0)->GetLocation(), port->GetValue());
						}
						break;
					}
					DrawWire(pt);
				}

				if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) { DrawCord(wireStart, wireEnd, GRAY); }

			} EndMode2D();

			//DrawLineV(mousePos_last, mousePos, WHITE);

			DrawText(TextFormat("Mouse at: { %#1.1f, %#1.1f }\nTotal components: %i", worldMousePos.x, worldMousePos.y, g_allPorts.Size()), 0, 0, 8, WHITE);
			//DrawText(TextFormat("{ %f, %f }", worldMousePos.x, worldMousePos.y), mousePos.x, mousePos.y + 10, 8, WHITE);
			//DrawText(TextFormat("%f", camera.zoom), mousePos.x, mousePos.y + 20, 8, WHITE);

		} EndDrawing();

#pragma endregion
	}

	CloseWindow();
}

#pragma endregion