
#include <raylib.h>

int main(void)
{
    const int WindowWidth = 1280;
    const int WindowHeight = 720;

    InitWindow(WindowWidth, WindowHeight, "RaylibApp");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        Color backgroundColor = { 255, 0, 255, 255 };

        ClearBackground(backgroundColor);

        EndDrawing();
    }

    return 0;
}


