#include <raylib.h>
#include <raymath.h>
#include <set>
#include <unordered_set>
#include "Transistors.h"

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
#define Sign(value) (decltype(value))(((value) > NULL) - ((value) < NULL))

Vector2 roundv2(Vector2 vec) { return { roundf(vec.x), roundf(vec.y) }; }

float Shift(float value, long shift)
{
	if (shift > 0) return (value * (1 << shift));
	if (shift < 0) return (value / (1 << -shift));
	return value;
}
float Shift(float value, float shift) { return Shift(value, lroundf(shift)); }

// Returns end but moved to the best place route I decided
Vector2 QueenRules(Vector2 start, Vector2 end)
{
	start = roundv2(start);
	end = roundv2(end);
	Vector2 dist = Vector2Subtract(end, start);
	float* shorter = &dist.x;
	float* longer = &dist.y;
	if (abs(*shorter) > abs(*longer)) std::swap(shorter,longer); // If incorrect, swap.
	if (abs(*shorter) > (abs(*longer) * 0.5f)) *longer = copysignf(*shorter, *longer); // Closer to diagonal
	else *shorter = 0.0f; // Closer to cardinal

	return Vector2Add(start, dist);
}

Shader g_shader_wire;
int g_uniform_wireActive;
Shader g_shader_cable;
Texture g_defaultTex;

void DrawCord(Vector2 start, Vector2 end, Color color)
{
	DrawLineV(start, end, color);
}
void DrawCordEx(Vector2 start, Vector2 end, Color color, float thickness)
{
	Rectangle src;
	{
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

int _main()
{
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
	Texture gateIcons[]
	{
		LoadTexture("Gate_L-AND.png"),
		LoadTexture("Gate_L-NOT.png"),
		LoadTexture("Gate_L-OR.png"),
		LoadTexture("Gate_L-XOR.png"),
	};
	// Draws the icon for a gate of the chosen evaluation method (AND, OR, NOT, XOR) centered at the position, scaled by world units
	auto DrawGateIcon = [&gateIcons](Eval m, Vector2 pt, float scale, Color tint)
	{
		int iconIndex;
		switch (m)
		{
		case Eval::L_AND: iconIndex = 0; break;
		case Eval::L_NOT: iconIndex = 1; break;
		case Eval::L_ORR: iconIndex = 2; break;
		case Eval::L_XOR: iconIndex = 3; break;
		}
		Texture& tex = gateIcons[iconIndex];
		float halfScale = scale * 0.5f;
		DrawTexturePro(tex, { 0, 0, (float)tex.width, (float)tex.height }, { pt.x - halfScale, pt.y - halfScale, scale, scale }, { 0.0f, 0.0f }, 0.0f, tint);
	};

	Camera2D camera;
	{
		camera.offset = { };
		camera.target = { };
		camera.rotation = 0.0f;
		camera.zoom = 8.0f;
	}
	// Used for updating the position of the camera when zooming
	Vector2 cameraPos
	{
		(float)windowWidth * 0.5f,
		(float)windowHeight * 0.5f
	};

	Vector2 mousePos, worldMousePos, mousePos_last, worldMousePos_last;
	mousePos = Vector2Zero();
	worldMousePos = Vector2Zero();

	Gate* startPort = nullptr;	// The base which was hovered when drawing began (resets when m1 is released)
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

	std::unordered_set<Gate*> selection;
	Gate* hovered = nullptr;

	enum class InputMode { Draw, Select, Edit } mode = InputMode::Draw;

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

					Gate* start = (neededAllocs.set(BIT_START, !startPort), (neededAllocs.test(BIT_START) ? new Node(startPos) : startPort));
					Gate* end = (neededAllocs.set(BIT_END, !hovered), (neededAllocs.test(BIT_END) ? new Node(endPos) : hovered));

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

				for (Gate* base : g_allPorts) {
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
				for (Gate* base : selection) {
					if (Gate* existing = g_allPorts.FindAtPos(worldMousePos, selection)) {
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

				for (Gate* base : selection) {
					base->SetLocation(Roundf(Vector2Add(base->Location_Of(), worldMouse_delta)));
				}
			}
			// Delete/backspace
			// Delete selected Gate*s
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
					std::unordered_set<Gate*> updatedSelection;
					for (Gate* base : selection) {
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
					for (Gate* base : selection) {
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
		std::unordered_set<Gate*> nullbodies; // Ports with no inputs nor outputs
		// Draw wires
		for (Gate* base : g_allPorts) {
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
						for (Gate* base : g_allPorts) {
							if (base) {
								Vector2 pt = base->Location_Of();
								float state = (base->GetValue() ? 0.0f : 1.0f);

								switch (base->GetType())
								{
								case Port::PortType::Gate:
									if (Gate* next = base->Next(0)) { // next must not be nullptr
										std::pair<Vector2, Vector2> wire = { pt, next->Location_Of() };
										if (state) activeWires.push(wire);
										else inactiveWires.push(wire);
									}
									break;
								case Port::PortType::Node:
									for (Gate* next : base->GetOutputs()) {
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
						for (Gate* base : g_allPorts) {
							if (base) {
								Vector2 pt = base->Location_Of();
								bool state = base->GetValue();
								Color stateColor = (state ? Color{ 255, 0, 0, 255 } : WHITE);

								switch (base->GetType())
								{
								case Port::PortType::Gate:
									if (Gate* next = base->Next(0)) { // next must not be nullptr
										DrawCord(pt, next->Location_Of(), stateColor);
									}
									break;
								case Port::PortType::Node:
									for (Gate* next : base->GetOutputs()) {
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
				for (Gate* base : selection) {
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
				for (Gate* base : g_allPorts) {
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
			for (Gate* base : g_allPorts) {
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

	for (Gate* base : g_allPorts) { if (base) Free_Poly(base); }

	UnloadShader(g_shader_wire);
	UnloadShader(g_shader_cable);

	for (Texture& icon : gateIcons) {
		UnloadTexture(icon);
	}
	UnloadTexture(g_defaultTex);

	CloseWindow();
}

#pragma endregion