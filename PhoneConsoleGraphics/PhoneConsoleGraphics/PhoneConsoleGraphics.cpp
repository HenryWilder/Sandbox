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

struct Vector2 { float x; float y; };

struct Rectangle { float x; float y; float w; float h; };

struct Triangle{ Vector2 p1; Vector2 p2; Vector2 p3; };

typedef int ID_t;

struct Texture{
    ID_t id;
    int width, height;
};
typedef Texture RenderTexture;
typedef Texture Texture2D;

struct TexSrc {
    enum {
        CT, // Const Tex
        RT  // Render Tex
    } tag;
    union {
        const char* ctex; // Const Tex
        char* rtex; // Render Tex
    };
};

struct Camera2D {
    Vector2 pos;
    float zoom;
};

enum UniformType {
    UNIFORM_VAL,
    UNIFORM_VEC1,
    UNIFORM_VEC2,
    UNIFORM_TEX,
};
struct ShaderParam {
    int type;
    union {
        char val;
        float vec1;
        float vec2[2];
        Texture tex;
    };
};
struct GenericShader {
    void(*func)();
    unordered_map<string,ShaderParam> inputs;
    unordered_map<string,ShaderParam> uniforms;
    unordered_map<string,ShaderParam> outputs;
};
struct ShaderData {
    GenericShader vert;
    GenericShader frag;
};
struct Shader {
    ID_t id;
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
    unordered_map<ID_t, TexSrc> textures;
    unordered_map<ID_t, ShaderData> shaders;
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
}
// Pops the current top camera
void EndCamera2DMode() {
    CORE.camera.pop();
}

// Pushes a new top shader
void BeginShaderMode(Shader shader) {
    CORE.shader.push(shader);
}
// Pops the current top shader
void EndShaderMode() {
    CORE.shader.pop();
}

int g_TopTexID = 0;

Texture2D LoadTexture(
    int width,
    int height,
    const char* texture) {
    CORE.textures[g_TopTexID].tag =
        TexSrc::CT;
    CORE.textures[g_TopTexID].ctex = texture;
    Texture2D out{ g_TopTexID, width, height };
    ++g_TopTexID;
    return out;
}
void UnloadTexture(Texture2D tex) {
    CORE.textures.erase(tex.id);
}

RenderTexture LoadRenderTexture(
    int width,
    int height) {
    CORE.textures[g_TopTexID].tag = 
        TexSrc::RT;
    CORE.textures[g_TopTexID].rtex =
        new char[(width + 1) * height];
    RenderTexture out{ g_TopTexID, width, height };
    ++g_TopTexID;
    return out;
}
void UnloadRenderTexture(RenderTexture tex) {
    auto it = CORE.textures.find(tex.id);
    if (it != CORE.textures.end())
    {
        delete[] it->second.rtex;
        CORE.textures.erase(it);
    }
}

void BeginTextureMode(RenderTexture texture) {
    CORE.frame.push(texture);
    CORE.bounds.x = 0;
    CORE.bounds.y = 0;
    CORE.bounds.w = texture.width;
    CORE.bounds.h = texture.height;
}
void EndTextureMode() {
    CORE.frame.pop();
}

void BeginDrawing() {
    RenderTexture frame = CORE.frame.top();
    CORE.bounds.x = 0;
    CORE.bounds.y = 0;
    CORE.bounds.w = frame.width;
    CORE.bounds.h = frame.height;
}
void EndDrawing() {
    auto it = CORE.textures.find(CORE.frame.top().id);
    const char* buffer = it->second.rtex;
    printf("\x1b[1;1f\x1b[48;m%s", buffer);
    CORE.frameTime = clock();
    FrameEndSleep();
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
    if (it->second.tag == TexSrc::RT)
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
    // Shader shd0 = DefaultShader;
    // CORE.shader.push(shd0);
}
bool WindowShouldClose() {
    return false; // todo
}
void CloseWindow() {
    for (auto it : CORE.textures) {
        // Only delete render textures
        if (it.second.tag ==
            TexSrc::RT)
            delete[] it.second.rtex;
    }
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

void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    ShaderData* shader =
        &CORE.shaders.find(CORE.shader.top().id)->second;

    shader->vert.inputs.find("pos") = x1,y1; // todo
    shader->vert.func();

    int minx, maxx;
    int miny, maxy;
    minx = max(min(min(x1,x2),x3),0);
    maxx = min(max(max(x1,x2),x3),CORE.frame.top().width);
    miny = max(min(min(y1,y2),y3),0);
    maxy = min(max(max(y1,y2),y3),CORE.frame.top().height);

    for (int y = miny; y < maxy; ++y) {
        for (int x = minx; x < maxx; ++x) {
            if (((x - x1) * (x - x2) - (y - y1) * (y - y2) >= 0) &&
                ((x - x2) * (x - x3) - (y - y2) * (y - y3) >= 0) &&
                ((x - x1) * (x - x3) - (y - y1) * (y - y3) >= 0))
            {

            }
        }
    }
}
void DrawTriangleTri(Triangle tri)
{
    DrawTriangle(
        (int)lroundf(tri.p1.x), (int)lroundf(tri.p1.y),
        (int)lroundf(tri.p2.x), (int)lroundf(tri.p2.y),
        (int)lroundf(tri.p3.x), (int)lroundf(tri.p3.y));
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
