#include <raylib.h>
#include <raymath.h>
#include <physac.h>
#include <vector>
#include <array>

#ifdef _DEBUG
#define _EDITOR 1
#else
#define _EDITOR 0
#endif

Texture g_spritesheet;

enum class TileType
{
    Air,
    Grass,

    Number
};

void DrawTiled(int offsetx, int offsety, int x, int y, int width, int height)
{
    DrawTextureTiled(g_spritesheet,
                 Rectangle{ (float)offsetx, (float)offsety, 16, 16 },
                 Rectangle{ (float)x, (float)y, (float)width, (float)height },
                 { 0,0 }, 0.0f, 1.0f, WHITE);
}

enum class Slice
{
    TopLeft, TopMid, TopRight,
    MidLeft, MidMid, MidRight,
    LowLeft, LowMid, LowRight,
};

void DrawNineSlice(TileType material, Slice slice, int x, int y, int width, int height)
{
    switch (slice)
    {
    case Slice::TopLeft:  DrawTiled( 0,  0, x, y, width, height); break;
    case Slice::TopMid:   DrawTiled(16,  0, x, y, width, height); break;
    case Slice::TopRight: DrawTiled(32,  0, x, y, width, height); break;
    case Slice::MidLeft:  DrawTiled( 0, 16, x, y, width, height); break;
    case Slice::MidMid:   DrawTiled(16, 16, x, y, width, height); break;
    case Slice::MidRight: DrawTiled(32, 16, x, y, width, height); break;
    case Slice::LowLeft:  DrawTiled( 0, 32, x, y, width, height); break;
    case Slice::LowMid:   DrawTiled(16, 32, x, y, width, height); break;
    case Slice::LowRight: DrawTiled(32, 32, x, y, width, height); break;
    }
}

void DrawTileRegion(TileType material, int x, int y, int width, int height)
{
    x *= 16;
    y *= 16;
    width *= 16;
    height *= 16;

    int xLow = x;
    int xMid = x + 16;
    int xFar = x + width - 16;

    int yLow = y;
    int yMid = y + 16;
    int yFar = y + height - 16;

    int widthNorm = 16;
    int widthLong = width - 32;

    int heightNorm = 16;
    int heightLong = height - 32;

    DrawNineSlice(material, Slice::TopLeft,  xLow, yLow, widthNorm, heightNorm);
    DrawNineSlice(material, Slice::TopMid,   xMid, yLow, widthLong, heightNorm);
    DrawNineSlice(material, Slice::TopRight, xFar, yLow, widthNorm, heightNorm);

    DrawNineSlice(material, Slice::MidLeft,  xLow, yMid, widthNorm, heightLong);
    DrawNineSlice(material, Slice::MidMid,   xMid, yMid, widthLong, heightLong);
    DrawNineSlice(material, Slice::MidRight, xFar, yMid, widthNorm, heightLong);

    DrawNineSlice(material, Slice::LowLeft,  xLow, yFar, widthNorm, heightNorm);
    DrawNineSlice(material, Slice::LowMid,   xMid, yFar, widthLong, heightNorm);
    DrawNineSlice(material, Slice::LowRight, xFar, yFar, widthNorm, heightNorm);
}
void DrawTileRegion(TileType material, Rectangle shape)
{
    int xLow = shape.x;
    int xMid = shape.x + 16;
    int xFar = shape.x + shape.width - 16;

    int yLow = shape.y;
    int yMid = shape.y + 16;
    int yFar = shape.y + shape.height - 16;

    int widthNorm = 16;
    int widthLong = shape.width - 32;

    int heightNorm = 16;
    int heightLong = shape.height - 32;

    DrawNineSlice(material, Slice::TopLeft, xLow, yLow, widthNorm, heightNorm);
    DrawNineSlice(material, Slice::TopMid, xMid, yLow, widthLong, heightNorm);
    DrawNineSlice(material, Slice::TopRight, xFar, yLow, widthNorm, heightNorm);

    DrawNineSlice(material, Slice::MidLeft, xLow, yMid, widthNorm, heightLong);
    DrawNineSlice(material, Slice::MidMid, xMid, yMid, widthLong, heightLong);
    DrawNineSlice(material, Slice::MidRight, xFar, yMid, widthNorm, heightLong);

    DrawNineSlice(material, Slice::LowLeft, xLow, yFar, widthNorm, heightNorm);
    DrawNineSlice(material, Slice::LowMid, xMid, yFar, widthLong, heightNorm);
    DrawNineSlice(material, Slice::LowRight, xFar, yFar, widthNorm, heightNorm);
}

struct Block
{
    Block()                                 : material(TileType::Air),  side(Slice::MidMid) {}
    Block(TileType _material)               : material(_material),      side(Slice::MidMid) {}
    Block(TileType _material, Slice _side)  : material(_material),      side(_side)         {}

    TileType material;
    Slice side;

#ifdef _EDITOR



#endif
};
#define B(...) Block(__VA_ARGS__)
std::array<Block, 32 * 32> g_world{
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
    B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),B(),
};
#undef B

bool CheckCollisionWorld(Rectangle rec)
{
    rec.x         *= 0.0625f;
    rec.y         *= 0.0625f;
    rec.width     *= 0.0625f;
    rec.height    *= 0.0625f;

    return (int)g_world.at((int)truncf(rec.y * 32.0f + rec.x)).material;
}

// Turns a worldspace position into a block index
size_t WorldPosToBlockIndex(Vector2 pos)
{
    int x = (int)pos.x;
    int y = (int)pos.y;
    long long int* temp = reinterpret_cast<long long int*>(&x); // Reinterpret the x and y ints (32 bits each) as a single long long int (64 bits) so the math can be applied to both simultaniously
    *temp >>= 4; // Divide by 16 but cheaper because it's a bitwise shift. Only possible because 16 is a power of two.
    *temp = ~((~*temp) | 0b0000000000000000000000000000000011110000000000000000000000000000); // clean the 4 bits from the shift, since the two values were sharing a register in the calculation.

    // Example: We want to set the middle two bits to zero always.
    // ~1100       -> 0011 (invert the bits)
    // 0011 | 0110 -> 0111 (turn the middle bits into 1s whether they're 1 or 0)
    // ~0111       -> 1000 (re-invert the bits back to normal)

    return ((y) * 32) + (x); // Make the position into an index in the 32x32 array
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 256;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes
    ******************************************/

    // TODO: Load persistent assets & variables

    g_spritesheet = LoadTexture("spritesheet.png");
    Texture player = LoadTexture("player.png");
    Texture backdrop = LoadTexture("backdrop.png");
    int frame = 0;

    Vector2 playerPos = {0,0};
    Vector2 playerVelocity = { 0, 0.1f };
    Rectangle playerCollision = { 0, 0, 16, 16 };

    auto TestWorldCollision = [&playerPos, &playerCollision]()
    {
        int playerBlockY = ((int)playerPos.y / 16);
        int playerBlockX = ((int)playerPos.x / 16);

        for (int y = ((int)playerPos.y > 0 ? -1 : 0); y <= ((int)playerPos.y < 31 ? 1 : 0); ++y)
        {
            for (int x = ((int)playerPos.x > 0 ? -1 : 0); x <= ((int)playerPos.x < 31 ? 1 : 0); ++x)
            {
                Block& block = g_world[WorldPosToBlockIndex({ (int)truncf(x),(int)truncf(y) })];
                if (block.material != TileType::Air)
                {
                    Rectangle rec{ ((int)playerPos.x + x), ((int)playerPos.y + y), 16, 16 };
                    if (CheckCollisionRecs(playerCollision, rec)) return rec;
                }
            }
        }
        return Rectangle{ 0,0,0,0 };
    };

    while (!WindowShouldClose() && playerPos.y < windowHeight)
    {
        /******************************************
        *   Simulate frame and update variables
        ******************************************/
        
        // Horizontal movement
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_A))
        {
            float movement = (float)IsKeyDown(KEY_D) - (float)IsKeyDown(KEY_A) * 0.25f;
            playerVelocity.x += movement;

            // Clamp speed
            if (abs((int)playerVelocity.x) > 2)
            {
                playerVelocity.x = (playerVelocity.x > 0.0f ? 2.0f : -2.0f);
            }
        }
        else // Friction
        {
            if (!playerVelocity.y) playerVelocity.x *= 0.75f;
            else playerVelocity.x *= 0.95;
        }

        // Gravity
        playerVelocity.y += 0.2f;
        playerPos = Vector2Add(playerPos, playerVelocity);

        playerCollision.x = playerPos.x;
        playerCollision.y = playerPos.y;

        // Collisions
        Rectangle rec = TestWorldCollision();
        if (rec.x)
        {
            playerVelocity.y = 0.0f;
            playerCollision.y = rec.y - playerCollision.height;
            playerPos.x = playerCollision.x;
            playerPos.y = playerCollision.y;

            // Jumping (only when colliding)
            if (IsKeyPressed(KEY_W))
            {
                playerVelocity.y = -4.0f;
                playerCollision.y = playerPos.y += playerVelocity.y;
            }
        }

#ifdef _EDITOR

        Vector2 mousePos = GetMousePosition();
        Block& selection = g_world[((int)mousePos.y / 16) * 32 + ((int)mousePos.x / 16)];

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            selection.material = (TileType)(((int)selection.material + 1) % (int)TileType::Number);
        }

#endif

        /******************************************
        *   Draw the frame
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            DrawTexturePro(backdrop, Rectangle{ (float)frame,0,(float)windowWidth,(float)windowHeight }, Rectangle{ 0,0,(float)windowWidth,(float)windowHeight }, { 0,0 }, 0.0f, WHITE);
            for (size_t y = 0; y < 32; y++)
            {
                for (size_t x = 0; x < 32; x++)
                {
                    Block& block = g_world[y * 32 + x];
                    if (block.material != TileType::Air)
                    {
                        DrawNineSlice(block.material, block.side, x * 16, y * 16, 16, 16);
                    }
                }
            }
            DrawTextureEx(player, playerPos, 0.0f, 1.0f, WHITE);

        } EndDrawing();

        ++frame;
    }

    /******************************************
    *   Unload and free memory
    ******************************************/

    // TODO: Unload variables

    UnloadTexture(g_spritesheet);
    UnloadTexture(player);
    UnloadTexture(backdrop);

    CloseWindow();
}