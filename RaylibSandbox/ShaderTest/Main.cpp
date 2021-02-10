#include <raylib.h>
#include <string>

const char* BuildVertShader(const char* uniformInserts = "", const char* posCode = "", const char* fragTexCoordCode = "")
{
	const char* out = TextFormat(
		R"(#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

// Custom uniforms
%s

void main()
{
// Send vertex attributes to fragment shader
// fragTexCoord
fragTexCoord = vertexTexCoord;
%s
fragColor = vertexColor;

// Pos code
vec3 pos = vertexPosition;
%s

// Calculate final vertex position
gl_Position = mvp*vec4(pos, 1.0);
})",
uniformInserts,
fragTexCoordCode,
posCode
);

	//printf("\n-------------------------\nVertex shader code built!\n-------------------------\n\n%s\n~~~~~~~~~~~~\n\n", out);

	return out;
}
const char* BuildFragShader(
	const char* uniformInserts = "",
	const char* ampCode = "",
	bool b_usesRand = false,
	bool b_usesTimeUniform = false)
{
	const char* out = TextFormat(
		R"(#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Custom uniforms
uniform vec2 resolution = vec2(1280, 720);
%s%s%s
void main()
{
float x = 1 / resolution.x;
float y = 1 / resolution.y;

vec4 color = vec4(vec3(0.0), 1.0);

// Pixel color calculation
// -----------------------------------------@
%s
// -----------------------------------------@
// Return

finalColor = color;
})",
uniformInserts,
(b_usesTimeUniform ?
 R"(
// Time
uniform float time = float(0.0);
)"
	: ""),
(b_usesRand ?
 R"(
// Implementation for rand() function
highp float rand(vec2 co)
{
highp float a = 12.9898;
highp float b = 78.233;
highp float c = 43758.5453;
highp float dt = dot(co.xy, vec2(a, b));
highp float sn = mod(dt, 3.14);
return fract(sin(sn) * c);
}
)"
	: ""),
		ampCode
	);

	//printf("\n---------------------------\nFragment shader code built!\n---------------------------\n\n%s\n~~~~~~~~~~~~\n\n", out);

	return out;
}

int main()
{
	Image img = LoadImage("test.png");
	
	int windowWidth = img.width;
	int windowHeight = img.height;

	// Position (0,0) on the window
	Vector2 zeroZero; {
		zeroZero.x = 0.0f;
		zeroZero.y = 0.0f;
	}

	// Window canvas rectangle
	Rectangle windowRec; {
		windowRec.x =		zeroZero.x;
		windowRec.y =		zeroZero.y;
		windowRec.width =	windowWidth;
		windowRec.height =	windowHeight;
	}

	float scaleDiff = 0.025;

	// Screen canvas rectangle
	Rectangle virtualRec; {
		virtualRec.x = zeroZero.x;
		virtualRec.y = zeroZero.y;
		virtualRec.width = windowWidth * scaleDiff;
		virtualRec.height = windowHeight * scaleDiff;
	}

	InitWindow(windowWidth, windowHeight, "Test");
	SetTargetFPS(60);

	Texture2D image = LoadTexture("test.png");

	// This is the shader I want to set the uniforms of
	Shader problemChild = LoadShader(NULL, "shader.frag");

	// Uniforms
	// -----------------------------

	// Resolution uniform
	// The resolution won't be changing at runtime, but I want the shader to be flexible.
	int resolution_UniformLoc = GetShaderLocation(problemChild, "resolution"); // <--- This is where the issue is

	float resolution[2]; {
		resolution[0] = (float)windowWidth;
		resolution[1] = (float)windowHeight;
	}

	SetShaderValue(problemChild, resolution_UniformLoc, resolution, UNIFORM_VEC2);

	// Cursor position uniform
	//int cursorPos_UniformLoc = GetShaderLocation(problemChild, "cursorPos"); // <--- This is where the issue is

	//float cursorPos[2]; {
	//	cursorPos[0] = 0.0f;
	//	cursorPos[1] = 0.0f;
	//}

	// Just a blank canvas the size of the window so the UVs don't get messed up
	RenderTexture2D target = LoadRenderTexture(windowWidth, windowHeight);

	// Initialize the canvas to 0s
	BeginTextureMode(target); {

		DrawRectangleRec(windowRec, BLACK);

	} EndTextureMode();

	//int textureLoc = GetShaderLocation(effect, "test");
	//SetShaderValueTexture(effect, textureLoc, image);

	Shader shader = LoadShaderCode(BuildVertShader(), BuildFragShader());
	std::string codeInsert = "";
	std::string fragCode = "";
	bool b_changed = true;
	int frameToggle = 0;
	int cursor = 0;
	int framesBackspaceHeld = 30;
	int framesLeftHeld = 30;
	int framesRightHeld = 30;
	int cursorBlink = 0;
	bool b_usesTime = false;
	int shaderTimeLoc = 0;

	Font font = LoadFont("mono_font.png");

	// Gameloop
	// -------------------------
	while (!WindowShouldClose())
	{
		// Update variables
		// -------------------------
		cursorBlink = (cursorBlink + 1) % 60;
		frameToggle = (frameToggle + 1) % 3;
		int typed = GetCharPressed();
		if (typed)
		{
			codeInsert.insert(cursor, 1, (char)typed);
			cursor++;
			b_changed = true;
		}

		if ((IsKeyPressed(KEY_BACKSPACE) || (IsKeyDown(KEY_BACKSPACE) && !framesBackspaceHeld && !frameToggle)) && !codeInsert.empty() && cursor > 0)
		{
			if (IsKeyDown(KEY_LEFT_CONTROL))
			{
				char c;
				do {
					cursor--;
					codeInsert.erase(codeInsert.begin() + cursor);
					if (!codeInsert.empty() && cursor > 0) c = codeInsert[(size_t)cursor - (size_t)1];
					else break;
				} while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
			}
			else
			{
				cursor--;
				codeInsert.erase(codeInsert.begin() + cursor);
			}
			b_changed = true;
		}
		
		if ((IsKeyPressed(KEY_LEFT) || (IsKeyDown(KEY_LEFT) && !framesLeftHeld)) && cursor > 0) cursor--;
		if ((IsKeyPressed(KEY_RIGHT) || (IsKeyDown(KEY_RIGHT) && !framesRightHeld)) && cursor < codeInsert.size()) cursor++;

		if (IsKeyDown(KEY_BACKSPACE) && framesBackspaceHeld > 0) framesBackspaceHeld--;
		if (IsKeyDown(KEY_LEFT) && framesLeftHeld > 0) framesLeftHeld--;
		if (IsKeyDown(KEY_RIGHT) && framesRightHeld > 0) framesRightHeld--;

		if (IsKeyPressed(KEY_BACKSPACE)) framesBackspaceHeld = 30;
		if (IsKeyPressed(KEY_LEFT)) framesLeftHeld = 30;
		if (IsKeyPressed(KEY_RIGHT)) framesRightHeld = 30;

		if (IsKeyPressed(KEY_ENTER))
		{
			codeInsert.insert(cursor, 1, '\n');
			cursor++;
			b_changed = true;
		}
		if (b_changed)
		{
			UnloadShader(shader);
			b_usesTime = (codeInsert.find("time") != std::string::npos);
			fragCode = BuildFragShader(
				"",
				codeInsert.c_str(),
				(codeInsert.find("rand(") != std::string::npos),
				b_usesTime
			);
			shader = LoadShaderCode(BuildVertShader(), fragCode.c_str());
			float res[2] = { windowWidth, windowHeight };
			SetShaderValue(shader, GetShaderLocation(shader, "resolution"), res, UNIFORM_VEC2);
			shaderTimeLoc = GetShaderLocation(shader, "time");
			cursorBlink = 0;
			b_changed = false;
		}

		if (b_usesTime)
		{
			float time = (float)GetTime();
			SetShaderValue(shader, shaderTimeLoc, &time, UNIFORM_FLOAT);
		}

		// Draw phase
		// -------------------------

		BeginDrawing(); {

			ClearBackground(BLACK);
			
			BeginShaderMode(shader); {
			
				DrawTextureRec(target.texture, { 0, 0, (float)windowWidth, (float)windowHeight }, {0,0}, WHITE);
			
			} EndShaderMode();

			std::string disp = fragCode;

			if (cursorBlink < 30) disp.insert(disp.find('@') + (size_t)cursor + 2, 1, '|');
			else disp.insert(disp.find('@') + (size_t)cursor + 2, 1, ' ');

			DrawTextEx(font, disp.c_str(), { 0, 0 }, 16, 2, WHITE);

		} EndDrawing();
	}
	
	UnloadFont(font);

	UnloadShader(problemChild);
	UnloadShader(shader);

	UnloadTexture(image);
	UnloadImage(img);

	UnloadRenderTexture(target);

	CloseWindow();

	return 0;
}