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
	bool b_usesTimeUniform = false,
	bool b_usesVeryRand = false)
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
%s%s%s%s
/**********************************************************
* Comment "Don't clear the frame, please." anywhere to
* prevent the ClearBackground() function from being called
* in the game loop.
**********************************************************/

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
// Time uniform (uses Raylib GetTime() function)
uniform float time = float(0.0);
)"
	:
 R"(
// Use "time" anywhere to implement the time uniform
)"),
(b_usesRand ?
R"(
// Random float from vector2
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
	:
R"(
// Use "rand(" anywhere to implement the rand(vec2) function"
)"),
(b_usesVeryRand ?
R"(
// Random float from vector2
highp float veryRand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt = dot(co.xy, vec2(a, b));
    highp float sn = mod(dt, 3.14);
    return fract(sin(sn) * c * mod(time, 1.0));
}
)"
	:
R"(
// Use "veryRand(" anywhere to implement a time-based rand function"
)"),
		ampCode
	);

	//printf("\n---------------------------\nFragment shader code built!\n---------------------------\n\n%s\n~~~~~~~~~~~~\n\n", out);

	return out;
}

int main()
{	
	int windowWidth = 1280;
	int windowHeight = 720;

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

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(windowWidth, windowHeight, "Raylib Shader Toy");
	SetTargetFPS(60);

	Image checked = GenImageChecked(32, 32, 16, 16, BLACK, ColorAlpha(MAGENTA, 0.1f));
	Texture2D missing = LoadTextureFromImage(checked);
	UnloadImage(checked);

	// Just a blank canvas the size of the window so the UVs don't get messed up
	RenderTexture2D target = LoadRenderTexture(windowWidth, windowHeight);

	// InitializeSpriteset the canvas to 0s
	BeginTextureMode(target); {

		DrawRectangleRec(windowRec, BLACK);

	} EndTextureMode();

	//int textureLoc = GetShaderLocation(effect, "test");
	//SetShaderValueTexture(effect, textureLoc, image);

	Shader shader = LoadShaderCode(BuildVertShader(), BuildFragShader());
	std::string codeInsert = "";
	std::string fragCode = "";
	bool b_dirty = true;
	int frameToggle = 0;
	int cursor = 0;
	int framesBackspaceHeld = 30;
	int framesLeftHeld = 30;
	int framesRightHeld = 30;
	int cursorBlink = 0;
	bool b_usesTime = false;
	int shaderTimeLoc = 0;
	bool dontClear = false;
	//std::string disp;
	unsigned int lineOffset = 0;

	auto DrawCode = [&fragCode, cursorBlink, cursor, &lineOffset]() {
		std::string disp = fragCode;

		if (cursorBlink < 30)
		{
			int dispPos = disp.find('@') + (size_t)cursor + 2 + 4;

			if (disp[dispPos] == '\n')
				disp.insert(dispPos, 1, '_');
			else
				disp.replace(dispPos, 1, 1, '_');
		}

		DrawText(disp.c_str(), 5, 5 - (10 * lineOffset), 8, WHITE);
	};

	// Gameloop
	// -------------------------
	while (!WindowShouldClose())
	{
		// Update variables
		// -------------------------
		if (GetScreenWidth() != windowWidth || GetScreenHeight() != windowHeight)
		{
			windowWidth = GetScreenWidth();
			windowHeight = GetScreenHeight();
			b_dirty = true;
		}

		cursorBlink = (cursorBlink + 1) % 60;
		frameToggle = (frameToggle + 1) % 3;
		int typed = GetCharPressed();
		if (typed >= ' ' && typed <= '~')
		{
			codeInsert.insert(cursor, 1, (char)typed);
			cursor++;
			b_dirty = true;
		}

		float wheel = GetMouseWheelMove();
		if (wheel > 0 && lineOffset < disp.size()) {
			lineOffset++;
		}
		else if (wheel < 0 && lineOffset > 0) {
			lineOffset--;
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
			b_dirty = true;
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
			codeInsert.insert(cursor++, 1, '\n');
			codeInsert.insert(cursor++, 1, ' ');
			codeInsert.insert(cursor++, 1, ' ');
			codeInsert.insert(cursor++, 1, ' ');
			codeInsert.insert(cursor++, 1, ' ');
			b_dirty = true;
		}
		if (b_dirty)
		{
			cursorBlink = 0;
			UnloadShader(shader);

			b_usesTime = false;
			dontClear = false;

			if ( (fragCode.find("for(")    == std::string::npos) && // Loops are forbidden
			     (fragCode.find("for (")   == std::string::npos) &&
				 (fragCode.find("while(")  == std::string::npos) &&
				 (fragCode.find("while (") == std::string::npos) )
			{
				b_usesTime = (codeInsert.find("time") != std::string::npos) || (codeInsert.find("veryRand(") != std::string::npos);
				fragCode = BuildFragShader(
					"",
					codeInsert.c_str(),
					(codeInsert.find("rand(") != std::string::npos),
					b_usesTime,
					(codeInsert.find("veryRand(") != std::string::npos)					
				);
				dontClear = (codeInsert.find("// Don't clear the frame, please.") != std::string::npos);
				shader = LoadShaderCode(BuildVertShader(), fragCode.c_str());
				float res[2] = { windowWidth, windowHeight };
				SetShaderValue(shader, GetShaderLocation(shader, "resolution"), res, UNIFORM_VEC2);
				shaderTimeLoc = GetShaderLocation(shader, "time");
			}
			else
			{
				shader = GetShaderDefault();
			}
			b_dirty = false;
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
			
			if (shader.id == GetShaderDefault().id)
			{
				DrawTextureRec(missing, { 0, 0, (float)windowWidth, (float)windowHeight }, { 0,0 }, WHITE);
			}
			else
			{
				BeginTextureMode(target); {

					if (!dontClear) ClearBackground(BLACK);
					DrawCode();

				} EndTextureMode();

				BeginShaderMode(shader); {

					DrawTextureRec(target.texture, { 0, 0, (float)windowWidth, -(float)windowHeight }, { 0,0 }, WHITE);

				} EndShaderMode();
			}

			DrawCode();

		} EndDrawing();
	}

	UnloadShader(shader);

	UnloadTexture(missing);

	UnloadRenderTexture(target);

	CloseWindow();

	return 0;
}