#include <thread>
#include <vector>
#include "raylib.h"
#include "raymath.h"

constexpr int worldHeight = 3;
constexpr int chunkWidth = 16;

constexpr short textureCount = 1;

//constexpr 

struct TexSet
{
	TexSet() {
		for (short i = 0; i < sizeof(s_textures); ++i)
		{
			s_textures[i] = LoadTexture("missing.png");
		}
	}
	~TexSet() {
		for (short i = 0; i < sizeof(s_textures); ++i)
		{
			UnloadTexture(s_textures[i]);
		}
	}

	static short s_currentTex;
	static Texture2D s_textures[textureCount];

	void Load(const char* texFileName)
	{
		if (s_currentTex < sizeof(s_textures)) s_textures[s_currentTex++] = LoadTexture(texFileName);
	}
	void Unload(short which)
	{
		if (s_currentTex < sizeof(which)) UnloadTexture(s_textures[which]);
	}
	void UnloadLast()
	{
		Unload(s_currentTex--);
	}
};
short TexSet::s_currentTex;
Texture2D TexSet::s_textures[];

struct Block
{
	Block() : BlockID(0) {};
	Block(short id) : BlockID(id) {};

	short BlockID;

	void Draw(Vector3 pos)
	{
		if (BlockID) DrawCubeTexture(TexSet::s_textures[BlockID], pos, 1.0f, 1.0f, 1.0f, WHITE);
	}
};

struct Chunk
{
	Chunk() : blocks(new Block[chunkWidth * chunkWidth * worldHeight]()) {};
	~Chunk() { delete[] blocks; }

	Block* blocks;

	void Create()
	{
		for (int z = 0; z < chunkWidth; ++z) {
			for (int y = 0; y < worldHeight; ++y) {
				for (int x = 0; x < chunkWidth; ++x) {
					blocks[(y * (chunkWidth * chunkWidth)) + (z * chunkWidth) + x] = Block(1);
				}
			}
		}
	}

	void Draw(int chX, int chZ)
	{
		for (int z = 0; z < chunkWidth; ++z) {
			for (int y = 0; y < worldHeight; ++y) {
				for (int x = 0; x < chunkWidth; ++x) {
					Vector3 blockPos{ (float)(x + chX - chunkWidth / 2) + 0.5f, (float)y + 0.5f, (float)(z + chZ - chunkWidth / 2) + 0.5f };
					blocks[(y * (chunkWidth * chunkWidth)) + (z * chunkWidth) + x].Draw(blockPos);
				}
			}
		}
	}
};

int main(int argc, char* argv[])
{
	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 1280;
	const int screenHeight = 720;

	//SetConfigFlags(FLAG_MSAA_4X_HINT);

	InitWindow(screenWidth, screenHeight, "Minecraft2");

	TexSet texturePack = TexSet();
	texturePack.Load("block_texture0.png");

	Chunk chunks[9];
	chunks[4].Create();

	Camera camera = { };
	camera.position = Vector3{ 8.0f, 8.0f, 8.0f };
	camera.target = Vector3{ 0.0f, 1.5f, 0.0f };
	camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.type = CAMERA_PERSPECTIVE;

	SetCameraMode(camera, CAMERA_ORBITAL);  // Set an orbital camera mode

	SetTargetFPS(60);
	//--------------------------------------------------------------------------------------
	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		// Update variables
		//----------------------------------------------------------------------------------

		UpdateCamera(&camera);              // Update camera
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(SKYBLUE);

		BeginMode3D(camera);

		for (int ch = 0; ch < 9; ++ch)
		{
			int chX = ((ch % 3) * chunkWidth) - (chunkWidth / 2);
			int chZ = ((ch / 3) * chunkWidth) - (chunkWidth / 2);

			chunks[ch].Draw(chX, chZ);
		}

		EndMode3D();

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	delete[] chunks;

	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}