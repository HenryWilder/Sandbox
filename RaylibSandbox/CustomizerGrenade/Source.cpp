
#include <raylib.h>
#include <raymath.h>
#include "vecmath.h"
#include "random.h"
#include "particles.h"

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    Random::Init();

    Emitter particleSystem0;

    ParticleProperties prop;
    prop.position = { (float)(windowWidth / 2), (float)(windowHeight / 2) };
    prop.velocity = { 10, 32 };
    prop.velocityVariation = { 5, 5 };
    prop.colorBegin = WHITE;
    prop.colorEnd = RED;
    prop.sizeBegin = 10;
    prop.sizeEnd = 2;
    prop.sizeVariation = 10;
    prop.lifeTime = 5.0f;

    particleSystem0.Emit(prop);
    particleSystem0.Emit(prop);
    particleSystem0.Emit(prop);
    particleSystem0.Emit(prop);
    particleSystem0.Emit(prop);

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        float dt = GetFrameTime();

        particleSystem0.OnUpdate(dt);

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            particleSystem0.OnRender();

            particleSystem0.Debug(2,0,32,8,MAGENTA);

            DrawFPS(0,0);

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}