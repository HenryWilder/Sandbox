#include <raylib.h>

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

	Texture2D image = LoadTextureFromImage(img);

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
	RenderTexture2D target = LoadRenderTexture(virtualRec.width, virtualRec.height);

	// Initialize the canvas to 0s
	BeginTextureMode(target); {

		DrawRectangleRec(windowRec, BLACK);

	} EndTextureMode();

	Shader effect = LoadShaderCode(NULL, R"(#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Custom uniforms
uniform sampler2D texture1;

void main()
{
finalColor = vec4(fragTexCoord, vec2(1.0)); // vec4(texture2D(texture1, fragTexCoord).rgba);
})");

	int textureLoc = GetShaderLocation(effect, "texture1");

	SetShaderValueTexture(effect, textureLoc, image);

	// Gameloop
	// -------------------------
	while (!WindowShouldClose())
	{
		// Update variables
		// -------------------------

		// Draw phase
		// -------------------------

		BeginDrawing(); {

			ClearBackground(BLACK);
			
			BeginShaderMode(effect); {

				ClearBackground(BLACK);

				

			} EndShaderMode();

		} EndDrawing();
	}
	
	UnloadShader(problemChild);
	UnloadShader(effect);

	UnloadTexture(image);
	UnloadImage(img);

	UnloadRenderTexture(target);

	CloseWindow();

	return 0;
}