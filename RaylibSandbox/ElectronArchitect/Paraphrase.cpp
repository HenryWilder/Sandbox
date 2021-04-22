#include <raylib.h>
#include <raymath.h>
#include <vector>

class Node
{
public:
	Vector2 position;
	std::vector<Node*> prev;
	std::vector<Node*> next;
};

int mainn()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes
    ******************************************/

    // TODO: Load persistent assets & variables

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables
        ******************************************/

        // TODO: simulate frame

        /******************************************
        *   Draw the frame
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            BeginShaderMode(gridShader); {

                DrawTexture();

            } EndShaderMode();

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory
    ******************************************/

    // TODO: Unload variables

    CloseWindow();
}