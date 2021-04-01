#include <raylib.h>
#include <raymath.h>
#include <array>
#include <iostream>
#include <thread>

bool Chance(int range, int chance)
{
    return ((rand() % range) < chance);
}

class Block
{
public:
    Block() : stage(Clean), waxed(Chance(16 * 16 * 16, 32)) {}

    void NextStage()
    {
        if (stage != Weathered) stage = (decltype(stage))(stage + 1);
    }

    Color GetColor()
    {
        switch (stage)
        {
        default:
        case Block::Clean: return Color{ 255,63, 0, 255 };
        case Block::Oxidizing: return Color{ 255, 127, 63, 255 };
        case Block::Aged: return Color{ 127, 255, 63, 255 };
        case Block::Weathered: return Color{ 63, 255, 63, 255 };
        }
    }

    void ToggleWax()
    {
        waxed = !waxed;
    }
    bool IsWaxed()
    {
        return waxed;
    }

    bool IsClean()
    {
        return (stage == Clean);
    }

private:
    enum : char { Clean, Oxidizing, Aged, Weathered } stage;
    bool waxed;
};

#if _DEBUG
size_t g_ops;
#define RESET_OPS g_ops = 0
#define ADD_OP ++g_ops
#define ADD_OPS(amt) g_ops += (amt)
#define PRINT_OPS std::cout << "Op count: " << g_ops << '\n'
#else
#define RESET_OPS
#define ADD_OP
#define ADD_OPS(amt)
#define PRINT_OPS
#endif

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Voxel world");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes
    ******************************************/

    constexpr int width = 64;
    constexpr int depth = 64;
    constexpr int height = 1;
    constexpr float blockScale = 1.0f;

    Block* world = new Block[width * depth * height]();

    Camera3D camera{ { width * 2, depth * 2, height * 2 }, { width / 2, depth / 2, height / 2 }, { 0.0f, 0.0f, 1.6f }, 45.0f, CAMERA_PERSPECTIVE };

    SetCameraMode(camera, CAMERA_ORBITAL);

    RESET_OPS;
    int frame = 0;
    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables
        ******************************************/
        UpdateCamera(&camera);

        size_t i = 0;
        for (size_t z = 0; z < height; z++)
        {
            for (size_t y = 0; y < depth; y++)
            {
                for (size_t x = 0; x < width; x++)
                {
                    int chance = 1;
                    if (!world[i].IsClean()) chance *= 2;

                    size_t offs;
#define TestWidth(length, invert, effect)\
                    offs = x - length; ADD_OPS(2); if (offs >= 0 &&            invert##world[((z) * depth * width) + ((y) * width) + (offs)].IsClean()) { ADD_OPS(2); (effect); }\
                    offs = x + length; ADD_OPS(2); if (offs <= (width - 1) &&  invert##world[((z) * depth * width) + ((y) * width) + (offs)].IsClean()) { ADD_OPS(2); (effect); }\
                    offs = y - length; ADD_OPS(2); if (offs >= 0 &&            invert##world[((z) * depth * width) + ((offs) * width) + (x)].IsClean()) { ADD_OPS(2); (effect); }\
                    offs = y + length; ADD_OPS(2); if (offs <= (depth - 1) &&  invert##world[((z) * depth * width) + ((offs) * width) + (x)].IsClean()) { ADD_OPS(2); (effect); }\
                    offs = z - length; ADD_OPS(2); if (offs >= 0 &&            invert##world[((offs) * depth * width) + ((y) * width) + (x)].IsClean()) { ADD_OPS(2); (effect); }\
                    offs = z + length; ADD_OPS(2); if (offs <= (height - 1) && invert##world[((offs) * depth * width) + ((y) * width) + (x)].IsClean()) { ADD_OPS(2); (effect); }

                    TestWidth(1, !, chance += 3);
                    TestWidth(2, !, chance += 2);
                    TestWidth(3, !, chance += 1);
                    TestWidth(1,,   chance -= 3);
                    TestWidth(2,,   chance -= 2);
                    TestWidth(3,,   chance -= 1);
#undef TestWidth
                    if (chance <= 0) chance = 1;

                    if (!world[i].IsWaxed() && Chance(4096, chance)) world[i].NextStage();
                    ++i;
                }
            }
        }
        PRINT_OPS;

        /******************************************
        *   Draw the frame
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);
            
            BeginMode3D(camera); {

                int i = 0;
                for (size_t z = 0; z < height; z++)
                {
                    for (size_t y = 0; y < depth; y++)
                    {
                        for (size_t x = 0; x < width; x++)
                        {
                            DrawCube({ (float)x,(float)y,(float)z }, blockScale, blockScale, blockScale, world[i++].GetColor());
                        }
                    }
                }

            } EndMode3D();

        } EndDrawing();
        ++frame;
        if (!(frame = frame % 60)) RESET_OPS;
    }

    /******************************************
    *   Unload and free memory
    ******************************************/

    delete[] world;

    CloseWindow();

    return 0;
}