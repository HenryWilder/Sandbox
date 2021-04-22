#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unordered_map>
#include<stack>
#include<chrono>
#include<thread>
using namespace std;

#if 0
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#ifndef _HUGE_ENUF
#define _HUGE_ENUF  1e+300  // _HUGE_ENUF*_HUGE_ENUF must overflow
#endif // !_HUGE_ENUF
#ifndef INFINITY
#define INFINITY ((float)(_HUGE_ENUF * _HUGE_ENUF))
#endif // !INFINITY

/*******************************
* Phone screen is 32 char wide *
* and 28 char tall, in console *
*******************************/

// Multiply by to get clock time
// in seconds
constexpr double g_tick =
    1.0 / (double)CLOCKS_PER_SEC; 

struct Vector2 {
    float x;
    float y;
};

struct Rectangle {
    float x;
    float y;
    float w;
    float h;
};

struct Triangle{
    Vector2 p1;
    Vector2 p2;
    Vector2 p3;
};

struct Texture{
    int id;
    int width;
    int height;
};
typedef Texture RenderTexture;
typedef Texture Texture2D;

struct Camera2D {
    Vector2 pos;
    float zoom;
};

struct Camera2DStackNode {
    Camera2DStackNode* prev;
    Camera2D cam;
};

typedef char (*Shader)(
    Vector2 fragTexCoord,
    Texture texture0);

struct ShaderStackNode {
    ShaderStackNode* prev;
    Shader shd;
};


////////////////////////////////
// Global state
struct CoreData {
    clock_t frameTime; // Updates every time the frame is drawn
    chrono::high_resolution_clock
        ::duration
        targetFrameDuration;
    chrono::high_resolution_clock
        ::time_point
        nextFrameTime;
    // Textures
    struct TexSrc {
        enum { CT, RT } tag;
        union {
            const char* ctex;
            char* rtex;
        };
    };
    unordered_map<int, TexSrc> textures;
    stack<RenderTexture> frame;
    Rectangle bounds;
    stack<Camera2D> camera;
    stack<Shader> shader;
} CORE;
////////////////////////////////


void SetTargetFramerate(unsigned long fps) {
    unsigned long frameDuration = 1000ul / fps;
    CORE.targetFrameDuration =
        chrono::duration_cast<decltype(CORE.targetFrameDuration)>(
            chrono::milliseconds(frameDuration));
    CORE.nextFrameTime =
        chrono::high_resolution_clock::now() +
        CORE.targetFrameDuration;
    LOG("Frame duration: %u\n", frameDuration);
}
void FrameEndSleep() {
    this_thread::sleep_until(CORE.nextFrameTime);
    CORE.nextFrameTime += CORE.targetFrameDuration;
}

Vector2 Vector2Zero() {
    static const Vector2 nullvector{ 0.0f, 0.0f };
    return nullvector;
}

long GetTime() {
    return ((double)clock() * g_tick);
}
double GetFrameTime() {
    return ((double)(clock() - CORE.frameTime) * g_tick);
}

// Pushes a new top camera
void BeginCamera2DMode(Camera2D camera) {
    CORE.camera.push(camera);
    LOG("Pushed camera\n");
}
// Pops the current top camera
void EndCamera2DMode() {
    CORE.camera.pop();
    LOG("Popped camera\n");
}

// Pushes a new top shader
void BeginShaderMode(Shader shader) {
    CORE.shader.push(shader);
    LOG("Pushed shader\n");
}
// Pops the current top shader
void EndShaderMode() {
    CORE.shader.pop();
    LOG("Popped shader\n");
}

int g_TopTexID = 0;

Texture2D LoadTexture(
    int width,
    int height,
    const char* texture) {
    CORE.textures[g_TopTexID].tag =
        CoreData::TexSrc::CT;
    CORE.textures[g_TopTexID].ctex = texture;
    Texture2D out{ g_TopTexID, width, height };
    ++g_TopTexID;
    LOG("Loaded texture %i: \"%s\"\n", out.id, texture);
    return out;
}
void UnloadTexture(Texture2D tex) {
    CORE.textures.erase(tex.id);
    LOG("Unloaded texture %i\n", tex.id);
}

RenderTexture LoadRenderTexture(
    int width,
    int height) {
    CORE.textures[g_TopTexID].tag = 
        CoreData::TexSrc::RT;
    CORE.textures[g_TopTexID].rtex =
        new char[(width + 1) * height];
    RenderTexture out{ g_TopTexID, width, height };
    ++g_TopTexID;
    LOG("Loaded render texture %i { %i, %i }\n", out.id, out.width, out.height);
    return out;
}
void UnloadRenderTexture(RenderTexture tex) {
    auto it = CORE.textures.find(tex.id);
    if (it != CORE.textures.end())
    {
        delete[] it->second.rtex;
        CORE.textures.erase(it);
        LOG("Unloaded render texture %i\n", tex.id);
    }
}

void BeginTextureMode(RenderTexture texture) {
    CORE.frame.push(texture);
    CORE.bounds.x = 0;
    CORE.bounds.y = 0;
    CORE.bounds.w = texture.width;
    CORE.bounds.h = texture.height;
    LOG("Pushed texture for rendering\n");
}
void EndTextureMode() {
    CORE.frame.pop();
    LOG("Popped texture for rendering\n");
}

void BeginDrawing() {
    RenderTexture frame = CORE.frame.top();
    CORE.bounds.x = 0;
    CORE.bounds.y = 0;
    CORE.bounds.w = frame.width;
    CORE.bounds.h = frame.height;
    LOG("Began drawing\n");
}
void EndDrawing() {
    auto it = CORE.textures.find(CORE.frame.top().id);
    const char* buffer = it->second.rtex;
    printf("\x1b[1;1f\x1b[48;m%s", buffer);
    LOG("Drew the bufer\n");
    CORE.frameTime = clock();
    LOG("Sleeping\n");
    FrameEndSleep();
    LOG("Finished sleeping\n");
}

char SampleTexture2D(Texture tex, Vector2 uv) {
    auto it = CORE.textures.find(tex.id);
    if (it == CORE.textures.end())
        return '?';
    uv.x = fmodf(uv.x, 1.0f);
    uv.y = fmodf(uv.y, 1.0f);
    uv.x *= (float)tex.width;
    uv.y *= (float)tex.height;
    int index = (lroundf(uv.y) * tex.width)
        + lroundf(uv.x);
    if (it->second.tag == CoreData::TexSrc::RT)
        return it->second.rtex[index];
    else
        return it->second.ctex[index];
}

char DefaultShader(
    Vector2 fragTexCoord,
    Texture texture0) {
    return SampleTexture2D(texture0, fragTexCoord);
}

void ClearBackground(char c) {
    RenderTexture frame = CORE.frame.top();
    char* buffer = CORE.textures.find(frame.id)->second.rtex;
    for (int y = 0; y < frame.height; ++y) {
        for (int x = 0; x < frame.width; ++x) {
            buffer[(y * (frame.width + 1)) + x] = c;
        }
        buffer[(y * (frame.width + 1)) + frame.width] = '\n';
    }
    buffer[(frame.width + 1) * frame.height] = '\0';
    LOG("Cleared background\n");
}

// Initialize the buffer
void InitWindow(int width, int height) {
    RenderTexture frame =
        LoadRenderTexture(width, height);
    CORE.frame.push(frame);
    ClearBackground(' ');
    CORE.frameTime = clock();
    Camera2D cam0;
    cam0.pos = Vector2Zero();
    cam0.zoom = 1.0f;
    CORE.camera.push(cam0);
    Shader shd0 = DefaultShader;
    CORE.shader.push(shd0);
    LOG("Initialized window\n");
}
bool WindowShouldClose() {
    return false; // todo
}
void CloseWindow() {
    for (auto it : CORE.textures) {
        // Only delete render textures
        if (it.second.tag ==
            CoreData::TexSrc::RT)
            delete[] it.second.rtex;
    }
    LOG("Closed window\n");
}

Vector2 WorldToScreen(Vector2 pt)
{
    Camera2D cam = CORE.camera.top();
    pt.x -= cam.pos.x;
    pt.y -= cam.pos.y;
    pt.x *= cam.zoom;
    pt.y *= cam.zoom;
    return pt;
}

bool CheckCollisionPointRect(
    Vector2 pt,
    Rectangle rec) {
    return (
        pt.x >= rec.x &&
        pt.x <= (rec.x + rec.w) &&
        pt.y >= rec.y &&
        pt.y <= (rec.y + rec.h));
}

void DrawPixel(Vector2 pt, char c) {
    char* buffer =
        CORE.textures.find(CORE.frame.top().id)
        ->second.rtex; // Only works with RenderTexture
    pt = WorldToScreen(pt);
    if (CheckCollisionPointRect(pt, CORE.bounds))
    {
        int index = (((CORE.frame.top().width + 1)
                    * lroundf(pt.y)) + lroundf(pt.x));
        buffer[index] = c;
    }
    LOG("Drew pixel at { %3.3f, %3.3f }\n",pt.x,pt.y);
}

// Used for testing if a point is within a triangle
bool EdgeFunc(
    const Vector2& a,
    const Vector2& b,
    const Vector2& c)
{
    return ((c.x - a.x) * (b.y - a.y) -
            (c.y - a.y) * (b.x - a.x)
            >= 0);
}

#define BLACK ' '
#define DARKGRAY '-'
#define GRAY '/'
#define LIGHTGRAY '+'
#define WHITE '#'
#define NEARWHITE '%'

//
// Custom code below here!!
//

int main()
{
    int windowWidth = 32;
    int windowHeight = 28;
    InitWindow(windowWidth,windowHeight);
    SetTargetFramerate(60);

    BeginDrawing();
    {
        ClearBackground(BLACK);

        DrawPixel({4,2},'a');
    }
    EndDrawing();

    CloseWindow();

    return 0;
}
