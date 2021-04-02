#include <vector>
#include <deque>
#include <queue>
#include <set>
#include <unordered_set>
#include <list>
#include <stack>
#include <bitset>
#include <unordered_map>
#include <fstream>
#include <raylib.h>
#include <raymath.h>

#if 0

enum class EvalMethod : short
{
	L_AND	= '&&',
	L_NOR	= '!',
	L_OR	= '||',
	L_XOR	= '^^',

	B_MASK	= '&',
	B_FLIP	= '~',
	B_COMB	= '|',
	B_SHFL	= '<<',
	B_SHFR	= '>>',
	B_BXOR	= '^',

	A_ADD	= '+',
	A_SUB	= '-',
	A_MUL	= '*',
	A_DIV	= '/',
	A_MOD	= '%',

	C_MORE	= '>',
	C_LESS	= '<',
	C_EQUA	= '==',
	C_NEQU	= '!=',
	C_GTOE	= '>=',
	C_LTOE	= '<=',
};
std::istream& operator>>(std::istream& stream, EvalMethod& val)
{
	char _val[2];
	stream >> _val;
	val = (EvalMethod)*reinterpret_cast<short*>(_val);
	return stream;
}
std::ostream& operator<<(std::ostream& stream, EvalMethod val)
{
	char* _vall = reinterpret_cast<char*>(&val);
	char _val[2]{ _vall[0], _vall[2] };
	stream << _val;
	return stream;
}
enum class Derivative : char
{
	Base = '0',
	Node = 'n',
	Gate = 'g',
	Comp = 'c',
	Unit = 'u',
};
std::istream& operator>>(std::istream& stream, EvalMethod& val)
{
	char _val;
	stream >> _val;
	val = (EvalMethod)_val;
	return stream;
}
std::ostream& operator<<(std::ostream& stream, EvalMethod val)
{
	return stream << (char)val;
}

#include "Inheritance.h"
//#include "Composition.h"
//#include "Handler.h"
#include "MethodInterface.h" // Creates names/macros/functions for interacting with an unknown methodology

// 0 = none, 1 = show tree, 2 = show names by each vertex
#define DEBUG_LEVEL 0

#pragma region Containers

#if DEBUG_LEVEL >= 1
std::unordered_map<Poly_t*, char> g_namedPtrs;
#endif

class PortList {
private:
	std::vector<Poly_t*> bases;

public:
	auto begin() { return bases.begin(); }
	auto end() { return bases.end(); }

	Poly_t* Get(size_t i) { return bases[i]; }

#if DEBUG_LEVEL >= 1
	std::string ConstructTreeStr() {
		g_namedPtrs[nullptr] = '-';
		auto PortName = [](Poly_t* ptr) {
			auto it = g_namedPtrs.find(ptr);
			if (it == g_namedPtrs.end()) {
				char name = (char)g_namedPtrs.size() - 1;
				switch (ptr->GetType())
				{
				case Port::PortType::Gate: name += 'A'; break;	// Gates are capital
				case Port::PortType::Node: name += 'a'; break;	// Nodes are lowercase
				default: name += '0'; break;					// Unknown are numbers
				}
				g_namedPtrs.insert({ ptr, name });
				it = g_namedPtrs.find(ptr);
			}
			return it->second;
		};
		std::string str;
		for (Poly_t* base : bases)
		{
			std::string str2;
			switch (base->GetType())
			{
			case Port::PortType::Gate: {
				Gate* gate = (Gate*)base;
				str2 = TextFormat(R"(
		(Gate)
		InputA: %c,
		InputB: %c,
		Output: %c,)", PortName(gate->i.a), PortName(gate->i.b), PortName(gate->o));

			}	break;
			case Port::PortType::Node: {
				Node* node = (Node*)base;

				std::string str3;
				for (Poly_t* elem : node->o) {
					str3 += TextFormat(" %c,", PortName(elem));
				}
				if (str3.empty()) str3 = " - ";
				else str3 += " ";

				str2 = TextFormat(R"(
		(Node)
		Input: %c,
		Outputs: {%s})", PortName(node->i), str3.c_str());
			}	break;
			}
			
			str += TextFormat("\n\t%c{%s\n\t}", PortName(base), (str2.empty() ? "\0" : str2.c_str()));
		}
		return (str.empty() ? " - " : str + "\n");
	};
#endif

	// ONLY call this if it's certain "what" has no external references!!
	// If "what" has the possibility of external references, please call Wipe() instead!
	void Erase(size_t index)
	{
		bases.erase(bases.begin() + index);
	}
	// ONLY call this if it's certain "what" has no external references!!
	// If "what" has the possibility of external references, please call Wipe() instead!
	void Erase(Poly_t* what)
	{
		for (size_t i = 0; i < bases.size(); ++i)
		{
			if (bases[i] == what)
			{
				Erase(i);
				break;
			}
		}
	}

	void Push(Poly_t* base)
	{
		bases.push_back(base);
	}
	void Reserve(size_t reservation)
	{
		if ((bases.size() + reservation) > bases.capacity())
		{
			bases.reserve(bases.size() + reservation);
		}
	}

	size_t Size() { return bases.size(); }

	Poly_t* FindAtPos(Vector2 at) {
		for (Poly_t* base : bases) {
			if (base && Vector2Distance(at, Location_Of(base)) < 1.0f)
				return base;
		}
		return nullptr;
	}
	Poly_t* FindAtPos(Vector2 at, Poly_t* avoid) {
		for (Poly_t* base : bases) {
			if ((base) && (Vector2Distance(at, Location_Of(base)) < 1.0f) && (base != avoid))
				return base;
		}
		return nullptr;
	}
	Poly_t* FindAtPos(Vector2 at, std::unordered_set<Poly_t*>& avoid) {
		for (Poly_t* base : bases) {
			if ((base) && (Vector2Distance(at, Location_Of(base)) < 1.0f) && (avoid.find(base) == avoid.end()))
				return base;
		}
		return nullptr;
	}

#if 0 // Wipe is no longer useful.
#define NULLIFY_IF_FOUND(var) if (hitlist.find(var) != hitlist.end()) var = nullptr // If the passed variable has a value on the no-no list, euthanize it.
	void Wipe(std::unordered_set<Poly_t*>& hitlist) { // unordered_set has constant-time (very fast) find(), making it perfect for "is this a wanted guy?"
		std::stack<size_t> srcs; // Stack of indices to the actual Poly_t* objects in the vector (FIFO so that indices don't get invalidated while erasing)
		size_t i = 0ull; // Stores index so we can do a range-based for loop :>
		for (Poly_t* base : bases) {
			if (hitlist.find(base) != hitlist.end()) srcs.push(i); // That one right there, officer.
			else { // This guy's gonna be gone anyway. Don't bother wasting the operations.
				switch (base->GetType()) // The different varients of Poly_t* have to be handled differently
				{
				case Port::PortType::Gate: { // Poly_t* is of the Gate class
					Gate* gate = (Gate*)base; // We are safe not to dynamic_cast here as we have already confirmed the type
					NULLIFY_IF_FOUND(gate->i.a);
					NULLIFY_IF_FOUND(gate->i.b);
					NULLIFY_IF_FOUND(gate->o);
				} break;

				case Port::PortType::Node: { // Poly_t* is of the Node class
					Node* node = (Node*)base; // We are safe not to dynamic_cast here as we have already confirmed the type
					NULLIFY_IF_FOUND(node->i);
					std::stack<size_t> badOuts; // Stack of indices of outputs on the no-no list (FIFO so that indices don't get invalidated while erasing)
					for (size_t o = 0; o < node->O_Size(); ++o) {
						if (hitlist.find(node->o[o]) != hitlist.end()) badOuts.push(o); // If the element at the index is bad, put it on the badOuts stack
					}
					while (!badOuts.empty()) { // For as long as there are outputs needing to be gone'd,
						node->o.erase(node->o.begin() + badOuts.top()); // Erase the index of the top bad output
						badOuts.pop(); // Whatever is on the top of the badOuts stack is no longer useful. Stop storing it.
					}
				} break;
				}
			}
			++i; // Increment the index so it's up to date
		}
		while (!srcs.empty()) { // For as long as there are elements needing to be gone'd,
			bases.erase(bases.begin() + srcs.top()); // Erase the index of the top source
			srcs.pop(); // Whatever is on the top of the srcs stack is no longer useful. Stop storing it.
		}
	}
#undef NULLIFY_IF_FOUND // Undefine so this name doesn't leak

#define REPLACE(var) if (var == target) var = with
	void Replace(Poly_t* target, Poly_t* with) {
		size_t targetIndex = bases.size();
		size_t i = 0;
		bool b_withAlreadyExists = false;
		for (Poly_t* base : bases) {
			if (base == with) b_withAlreadyExists = true;
			else if (base == target) targetIndex = i;
			else {
				switch (base->GetType())
				{
				case Port::PortType::Gate: {
					Gate* gate = (Gate*)base;
					REPLACE(gate->i.a);
					REPLACE(gate->i.b);
					REPLACE(gate->o);
				} break;
				case Port::PortType::Node: {
					Node* node = (Node*)base;
					REPLACE(node->i);
					for (Poly_t*& o : node->o) {
						REPLACE(o);
					}
				} break;
				}
			}
			++i;
		}
		if (targetIndex < bases.size()) {
			if (b_withAlreadyExists) bases[targetIndex] = with;
			else bases.erase(bases.begin() + targetIndex);
		}
	}
#undef REPLACE // Undefine so this name doesn't leak
#endif
};
PortList g_allPorts;

struct ColorScheme {
	ColorScheme() : background(), grid(), selection(), icons() {}
	ColorScheme(Color _background, Color _grid, Color _selection, Color _icons) : background(_background), grid(_grid), selection(_selection), icons(_icons) {}

	Color background;
	Color grid;
	Color selection;
	Color icons;
};

#pragma endregion

#pragma region Other

bool Positive(float value) { return value >= 0.0f; }
float Sign(float value) {
	if (value > 0.0f) return 1.0f;
	if (value < 0.0f) return -1.0f;
	return 0.0f;
}
Vector2 roundv2(Vector2 vec) { return { roundf(vec.x), roundf(vec.y) }; }

float Shift(float value, int shift) {
	if (shift > 0) return (value * (1 << shift));
	if (shift < 0) return (value / (1 << -shift));
	return value;
}
float Shift(float value, float shift) { return Shift(value, lroundf(shift)); }

// Returns end but moved to the best place route I decided
Vector2 QueenRules(Vector2 start, Vector2 end) {
	start = roundv2(start);
	end = roundv2(end);
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

Shader g_shader_wire;
int g_uniform_wireActive;
Shader g_shader_cable;
Texture g_defaultTex;

void DrawCord(Vector2 start, Vector2 end, Color color) {
	DrawLineV(start, end, color);
}
void DrawCordEx(Vector2 start, Vector2 end, Color color, float thickness) {
	Rectangle src; {
		src.x = start.x;
		src.y = start.y;
		src.width = Vector2Distance(start, end);
		src.height = thickness;
	}
	float rot = Vector2Angle(start, end);

	DrawCircleV(start, thickness * 0.5f, color);
	DrawCircleV(end, thickness * 0.5f, color);

	DrawTexturePro(g_defaultTex, {0,0,1,1}, src, { 0.0f, 0.5f * src.height }, rot, WHITE);
}

#pragma endregion

//  __  __       _
// |  \/  |     (_)
// | \  / | __ _ _ _ __
// | |\/| |/ _` | | '_ \
// | |  | | (_| | | | | |
// |_|  |_|\__,_|_|_| |_|

#pragma region Main

int _main() {
	int windowWidth = 1280;
	int windowHeight = 720;

	InitWindow(windowWidth, windowHeight, "Electron Architect");
	SetTargetFPS(30);

	g_shader_wire = LoadShader(0, "wire.frag");
	g_uniform_wireActive = GetShaderLocation(g_shader_wire, "b_active");
	int uniform_wireTime = GetShaderLocation(g_shader_wire, "time");
	int uniform_wireRes = GetShaderLocation(g_shader_wire, "resolution");
	{
		float resolution[2] = { (float)windowWidth, (float)windowHeight };
		SetShaderValue(g_shader_wire, uniform_wireRes, resolution, UNIFORM_VEC2);
	}
	g_shader_cable = LoadShader(0, "cable.frag");

	Image img = GenImageColor(1,1,WHITE);
	g_defaultTex = LoadTextureFromImage(img); // A 1x1 pixel texture of just white; used for drawing rectangles with shaders applied (DrawRectangle command doesn't give fragTexCoord)
	UnloadImage(img);

	// Array of textures for drawing gates
	Texture gateIcons[] = {
		LoadTexture("Gate_L-AND.png"),
		LoadTexture("Gate_L-NOT.png"),
		LoadTexture("Gate_L-OR.png"),
		LoadTexture("Gate_L-XOR.png"),
	};
	// Draws the icon for a gate of the chosen evaluation method (AND, OR, NOT, XOR) centered at the position, scaled by world units
	auto DrawGateIcon = [&gateIcons](EvalMethod m, Vector2 pt, float scale, Color tint) {
		int iconIndex;
		switch (m)
		{
		case EvalMethod::L_AND: iconIndex = 0; break;
		case EvalMethod::L_NOR: iconIndex = 1; break;
		case EvalMethod::L_OR:  iconIndex = 2; break;
		case EvalMethod::L_XOR: iconIndex = 3; break;
		}
		Texture& tex = gateIcons[iconIndex];
		float halfScale = scale * 0.5f;
		DrawTexturePro(tex, { 0, 0, (float)tex.width, (float)tex.height }, { pt.x - halfScale, pt.y - halfScale, scale, scale }, { 0.0f, 0.0f }, 0.0f, tint);
	};

	Camera2D camera; {
		camera.offset = { };
		camera.target = { };
		camera.rotation = 0.0f;
		camera.zoom = 8.0f;
	}
	// Used for updating the position of the camera when zooming
	Vector2 cameraPos = {
		(float)windowWidth * 0.5f,
		(float)windowHeight * 0.5f
	};

	Vector2 mousePos, worldMousePos, mousePos_last, worldMousePos_last;
	mousePos = { 0,0 };
	worldMousePos = { 0,0 };

	Poly_t* startPort = nullptr;	// The base which was hovered when drawing began (resets when m1 is released)
	Vector2 startPos{};			// Where the mouse was when drawing was started
	Vector2 endPos{};			// Where the mouse was when drawing was finished
	Rectangle collision;		// For selection

	// The worldspace position & size of the top-left screen pixel
	Rectangle worldPixel;
	// Worldspace rectangle from screen 0 to screen width/height
	Rectangle worldScreen;
	// Update worldPixel & worldScreen to reflect possible camera changes
	auto UpdateWorldPixel = [&] {
		Vector2 pos = GetScreenToWorld2D({ 0, 0 }, camera);
		Vector2 width = Vector2Subtract(GetScreenToWorld2D({ 1, 1 }, camera), pos);

		worldPixel.x = worldScreen.x = pos.x;
		worldPixel.y = worldScreen.y = pos.y;
		worldPixel.width  = width.x;
		worldPixel.height = width.y;

		width = Vector2Subtract(GetScreenToWorld2D({ (float)windowWidth, (float)windowHeight }, camera), pos);
		worldScreen.width  = width.x;
		worldScreen.height = width.y;
	};

	std::unordered_set<Poly_t*> selection;
	Poly_t* hovered = nullptr;

	enum class InputMode { Draw, Select, Edit } mode = InputMode::Draw;
	Derivative make = Derivative::Node;

	float time = 0.0f;

	while (!WindowShouldClose()) {
#pragma region Simulate

		time = (float)GetTime();
		SetShaderValue(g_shader_wire, uniform_wireTime, &time, UNIFORM_FLOAT);

		if (GetMouseWheelMove() != 0.0f) {
			float zoom = Shift(camera.zoom, GetMouseWheelMove());
			if (zoom >= 1.0f) {
				if (zoom < 127.0f) camera.zoom = zoom;
				else camera.zoom = 127.0f;
			}
			else camera.zoom = 1.0f;
			cameraPos = Vector2Scale(roundv2(Vector2Scale(cameraPos, 1.0f / camera.zoom)), camera.zoom); // Update camera position now that zoom has been changed
		}

		mousePos_last = mousePos;
		mousePos = GetMousePosition();

		worldMousePos_last = worldMousePos;
		worldMousePos = GetScreenToWorld2D(mousePos, camera);

		hovered = g_allPorts.FindAtPos(worldMousePos);

		switch (mode)
		{
		default:
		case InputMode::Draw: {
			// Middle mouse drag
			// Pan
			if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
				Vector2 mouse_delta = Vector2Subtract(mousePos, mousePos_last);
				Vector2 worldMouse_delta = Vector2Subtract(worldMousePos, worldMousePos_last);

				cameraPos = Vector2Scale(roundv2(Vector2Scale(Vector2Add(cameraPos, mouse_delta), 1.0f / camera.zoom)), camera.zoom);
			}
			// M1 Press
			// Start wire
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				startPort = hovered;
				if (startPort) startPos = Location_Of(startPort);
				else startPos = roundv2(worldMousePos);
			}
			// M1 Dragging
			// Update endpoint position
			if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				endPos = roundv2(QueenRules(startPos, worldMousePos));
			}
			// M1 Release
			// Connect wire
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				// If the start and end points aren't within a unit of each other
				if (Vector2Distance(endPos, startPos) > 0.9)
				{
			#define BIT_START 0
			#define BIT_END 1

					std::bitset<2> neededAllocs;

					Poly_t* start = (neededAllocs.set(BIT_START, !startPort), (neededAllocs.test(BIT_START) ? new Node(startPos) : startPort));
					Poly_t* end = (neededAllocs.set(BIT_END, !hovered), (neededAllocs.test(BIT_END) ? new Node(endPos) : hovered));

					g_allPorts.Reserve(neededAllocs.count());
					if (neededAllocs.test(BIT_START)) g_allPorts.Push(start);
					if (neededAllocs.test(BIT_END))   g_allPorts.Push(end);

					Push_Output(start, end); // Push end as an output of start
					Push_Input(end, start); // Push start as an input of end

			#undef BIT_START
			#undef BIT_END
				}
			}
			// M2 press
			// Switch modes
			if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				mode = InputMode::Select;
			}
		} break;
		case InputMode::Select: {
			// M1 press
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				startPos = worldMousePos;
			}
			// M1 drag
			// Select
			if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
				if (worldMousePos.x < startPos.x) {
					collision.x = worldMousePos.x;
					collision.width = startPos.x - worldMousePos.x;
				}
				else {
					collision.x = startPos.x;
					collision.width = worldMousePos.x - startPos.x;
				}
				if (worldMousePos.y < startPos.y) {
					collision.y = worldMousePos.y;
					collision.height = startPos.y - worldMousePos.y;
				}
				else {
					collision.y = startPos.y;
					collision.height = worldMousePos.y - startPos.y;
				}

				for (Poly_t* base : g_allPorts) {
					if (CheckCollisionPointRec(Location_Of(base), collision)) {
						selection.insert(base);
					}
					else {
						if (selection.find(base) != selection.end()) selection.erase(base);
					}
				}
			}
			// M1 Release
			// Start editing selection
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				if (!selection.empty()) mode = InputMode::Edit;
			}
			// M2 Press
			// Exit select mode
			if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				selection.clear();
				mode = InputMode::Draw;
			}
		}	break;
		case InputMode::Edit: {
			// Middle release
			// Stop dragging
			if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
				for (Poly_t* base : selection) {
					if (Poly_t* existing = g_allPorts.FindAtPos(worldMousePos, selection)) {
						g_allPorts.Erase(existing); // First we remove the base from the world, so its source doesn't get overwritten
						g_allPorts.Replace(existing, base); // Then we overwrite every reference to it
						base->Push_I(existing->Prev(0)); // Replace base's input with existing's input
						delete existing; // And finally free it
					}
				}
				selection.clear();
			}
			// Middle dragging
			// Drag selection
			if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
				Vector2 mouse_delta = Vector2Subtract(mousePos, mousePos_last);
				Vector2 worldMouse_delta = Vector2Subtract(worldMousePos, worldMousePos_last);

				for (Poly_t* base : selection) {
					base->SetLocation(Roundf(Vector2Add(base->Location_Of(), worldMouse_delta)));
				}
			}
			// Delete/backspace
			// Delete selected Poly_t*s
			if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) {
				g_allPorts.Wipe(selection);
				selection.clear();
				mode = InputMode::Select;
			}

			// Key
			{
				// Number
				// Change type of bases selected
				int pressed = GetCharPressed();
				if (pressed == '1' || pressed == '2') {
					Vector2 pos = Roundf(worldMousePos);
					std::unordered_set<Poly_t*> updatedSelection;
					for (Poly_t* base : selection) {
						switch (pressed)
						{
						case '1': // Set selected to nodes
							updatedSelection.insert(ToNode(base));
							break;
						case '2': // Set selected to gates
							updatedSelection.insert(ToGate(base));
							break;
						default:
							break;
						}
					}
					selection = updatedSelection;
				}
				// Letter
				// Change type of gate
				if (((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && (pressed == '|' || pressed == '&' || pressed == '!')) || pressed == '^') {
					for (Poly_t* base : selection) {
						if (Gate* gate = dynamic_cast<Gate*>(base)) {
							switch (pressed)
							{
							case '|': // Set selected to nodes
								gate->SetMethod(Gate::Method::OR);
								break;
							case '&': // Set selected to gates
								gate->SetMethod(Gate::Method::AND);
								break;
							case '!': // Set selected to gates
								gate->SetMethod(Gate::Method::NOT);
								break;
							case '^': // Set selected to gates
								gate->SetMethod(Gate::Method::XOR);
								break;
							}
						}
					}
				}
			}

			// M2 Press
			if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				selection.clear();
				mode = InputMode::Draw;
			}
		} break;
		}

		// Evaluate
		std::unordered_set<Poly_t*> nullbodies; // Ports with no inputs nor outputs
		// Draw wires
		for (Poly_t* base : g_allPorts) {
			if (base->I_Size() || base->O_Size()) base->GetValue(); // Evaluate
			else nullbodies.insert(base); // Eliminate
		}
		g_allPorts.Wipe(nullbodies);

		hovered = g_allPorts.FindAtPos(worldMousePos); // Might no longer be available

#pragma endregion

#pragma region Draw

		BeginDrawing(); {

			camera.offset = cameraPos;
			UpdateWorldPixel();

			switch (mode)
			{
			case InputMode::Draw:	ClearBackground(Color{ 0, 5, 10, 255 });	break;
			case InputMode::Select: ClearBackground(Color{ 20, 15, 0, 255 });	break;
			case InputMode::Edit:	ClearBackground(Color{ 20, 5, 30, 255 });	break;
			}

			BeginMode2D(camera); {

				{ // Draw the world grid
					float screenRight = worldPixel.x + worldScreen.width;
					float screenBottom = worldPixel.y + worldScreen.height;

					if (camera.zoom > 1.0f)
					{
						for (float y = worldPixel.y; y <= screenBottom; y += (worldPixel.height * camera.zoom)) {
							DrawLineV({ worldPixel.x, y }, { screenRight, y }, { 20,20,20,255 });
						}
						for (float x = worldPixel.x; x <= screenRight; x += (worldPixel.width * camera.zoom)) {
							DrawLineV({ x, worldPixel.y }, { x, screenBottom }, { 20,20,20,255 });
						}
					}
					else
					{
						DrawRectangleRec(worldScreen, { 20, 20, 20, 255 });
					}
				}

				if (mode == InputMode::Select && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
					DrawRectangleRec(collision, Color{ 255, 127, 0, 127 });
					DrawLineV({ startPos.x, startPos.y }, { worldMousePos.x, startPos.y }, Color{ 255, 127, 0, 255 });
					DrawLineV({ worldMousePos.x, startPos.y }, { worldMousePos.x, worldMousePos.y }, Color{ 255, 127, 0, 255 });
					DrawLineV({ worldMousePos.x, worldMousePos.y }, { startPos.x, worldMousePos.y }, Color{ 255, 127, 0, 255 });
					DrawLineV({ startPos.x, worldMousePos.y }, { startPos.x, startPos.y }, Color{ 255, 127, 0, 255 });
				}

				// Draw wires
				{
					if ((camera.zoom * 0.2f) >= (1.0f)) // The width of a wire must be greater than a pixel
					{
						std::queue<std::pair<Vector2, Vector2>> activeWires;
						std::queue<std::pair<Vector2, Vector2>> inactiveWires;

						// Fill out active/inactive wire queues
						for (Poly_t* base : g_allPorts) {
							if (base) {
								Vector2 pt = base->Location_Of();
								float state = (base->GetValue() ? 0.0f : 1.0f);

								switch (base->GetType())
								{
								case Port::PortType::Gate:
									if (Poly_t* next = base->Next(0)) { // next must not be nullptr
										std::pair<Vector2, Vector2> wire = { pt, next->Location_Of() };
										if (state) activeWires.push(wire);
										else inactiveWires.push(wire);
									}
									break;
								case Port::PortType::Node:
									for (Poly_t* next : base->GetOutputs()) {
										std::pair<Vector2, Vector2> wire = { pt, next->Location_Of() };
										if (state) activeWires.push(wire);
										else inactiveWires.push(wire);
									}
									break;
								}
							}
						}

						// @ Draw the active wires
						float state = 1.0f;
						SetShaderValue(g_shader_wire, g_uniform_wireActive, &state, UNIFORM_FLOAT);
						BeginShaderMode(g_shader_wire);
						while (!activeWires.empty()) {
							DrawCordEx(activeWires.front().first, activeWires.front().second, WHITE, 0.2f);
							activeWires.pop();
						}
						EndShaderMode();

						// @ Draw the inactive wires
						state = 0.0f;
						SetShaderValue(g_shader_wire, g_uniform_wireActive, &state, UNIFORM_FLOAT);
						BeginShaderMode(g_shader_wire);
						while (!inactiveWires.empty()) {
							DrawCordEx(inactiveWires.front().first, inactiveWires.front().second, WHITE, 0.2f);
							inactiveWires.pop();
						}
						EndShaderMode();
					}
					else
					{
						for (Poly_t* base : g_allPorts) {
							if (base) {
								Vector2 pt = base->Location_Of();
								bool state = base->GetValue();
								Color stateColor = (state ? Color{ 255, 0, 0, 255 } : WHITE);

								switch (base->GetType())
								{
								case Port::PortType::Gate:
									if (Poly_t* next = base->Next(0)) { // next must not be nullptr
										DrawCord(pt, next->Location_Of(), stateColor);
									}
									break;
								case Port::PortType::Node:
									for (Poly_t* next : base->GetOutputs()) {
										DrawCord(pt, next->Location_Of(), stateColor);
									}
									break;
								}
							}
						}
					}
				}

				if (hovered) {
					Vector2 pt = hovered->Location_Of();
					switch (hovered->GetType())
					{
					case Port::PortType::Gate:
						DrawGateIcon(((Gate*)hovered)->GetMethod(), pt, 3.0f, GOLD);
						break;

					case Port::PortType::Node:
						DrawCircleV(pt, .4f, GOLD);
						break;

					default:
						break;
					}
				}
				// Highlight selection
				for (Poly_t* base : selection) {
					if (base) {
						Vector2 pt = base->Location_Of();
						switch (base->GetType())
						{
						case Port::PortType::Gate:
							DrawGateIcon(((Gate*)base)->GetMethod(), pt, 3.0f, YELLOW);
							break;

						case Port::PortType::Node:
							DrawCircleV(pt, .4f, YELLOW);
							break;

						default:
							break;
						}
					}
				}
				// Draw bases
				for (Poly_t* base : g_allPorts) {
					if (base) {
						Vector2 pt = base->Location_Of();
						switch (base->GetType())
						{
						case Port::PortType::Gate: {
							DrawGateIcon(((Gate*)base)->GetMethod(), pt, 2.0f, BLUE);
						}	break;
						case Port::PortType::Node:
							DrawCircleV(pt, .1f, BLUE);
							break;

						default:
							break;
						}
					}
				}

				// Draw the wire the user is currently creating
				if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && (mode == InputMode::Draw)) {
					DrawCord(startPos, endPos, GRAY);
				}

			} EndMode2D();

#if DEBUG >= 1
			DrawText(TextFormat("Mouse at: { %#1.1f, %#1.1f }\nTotal components: %i", worldMousePos.x, worldMousePos.y, g_allPorts.Size()), 0, 0, 8, WHITE);

			DrawText(TextFormat("Root{%s}", g_allPorts.ConstructTreeStr().c_str()), 0, 48, 8, WHITE);
			auto it = g_namedPtrs.find(hovered);
			if (it != g_namedPtrs.end())
				DrawText(TextFormat("Hovering %c", it->second), 86, 48, 8, WHITE);
#endif
			for (Poly_t* base : g_allPorts) {
#if DEBUG >= 2
				auto it = g_namedPtrs.find(base);
				if (it != g_namedPtrs.end()) {
					Vector2 pt = GetWorldToScreen2D(base->Location_Of(), camera);
					DrawText(TextFormat("%c", it->second), (int)pt.x + 1, (int)pt.y + 1, 8, MAGENTA);
				}
#endif
				if (Gate* gate = dynamic_cast<Gate*>(base)) gate->FrameEndReset();
			}
	} EndDrawing();

#pragma endregion
	}

	for (Poly_t* base : g_allPorts) { if (base) Free_Poly(base); }

	UnloadShader(g_shader_wire);
	UnloadShader(g_shader_cable);

	for (Texture& icon : gateIcons) {
		UnloadTexture(icon);
	}
	UnloadTexture(g_defaultTex);

	CloseWindow();
}

#pragma endregion
#endif