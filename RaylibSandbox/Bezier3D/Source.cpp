#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <array>
#include "Interp.h"
#include "Triangle.h"

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    using Triangle_t = std::array<Vector2, 6>;
    std::vector<std::array<Vector2, 6>> pt{
        {
            Vector2{ 150.0f, 50.0f }, // p0
            Vector2{ 75.0f, 125.0f },
            Vector2{ 50.0f, 200.0f }, // p1
            Vector2{ 150.0f, 200.0f },
            Vector2{ 250.0f, 200.0f }, // p2
            Vector2{ 200.0f, 100.0f }
        },
        {
            Vector2{ 250 + 150.0f, 50.0f }, // p0
            Vector2{ 250 + 75.0f, 125.0f },
            Vector2{ 250 + 50.0f, 200.0f }, // p1
            Vector2{ 250 + 150.0f, 200.0f },
            Vector2{ 250 + 250.0f, 200.0f }, // p2
            Vector2{ 250 + 200.0f, 100.0f }
        }
    };

    std::vector<Triangle_t*> tri;
    Vector2* selected = nullptr;
    bool held = false;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            selected = nullptr;
            held = false;
        }
        else
        {
            if (!held)
            {
                tri.clear();
                selected = nullptr;
                for (size_t i = 0; i < pt.size(); ++i)
                {
                    for (size_t j = 0; j < 6; ++j)
                    {
                        if (Vector2Distance(pt[i][j], GetMousePosition()) < 5.0f)
                        {
                            selected = &(pt[i][j]);
                            break;
                        }
                    }

                    float minx = FLT_MAX;
                    float miny = FLT_MAX;
                    float maxx = -FLT_MAX;
                    float maxy = -FLT_MAX;

                    for (size_t j = 0; j < 6; ++j)
                    {
                        if (pt[i][j].x < minx)
                            minx = pt[i][j].x;
                        else if (pt[i][j].x > maxx)
                                maxx = pt[i][j].x;

                        if (pt[i][j].y < miny)
                            miny = pt[i][j].y;
                        else if (pt[i][j].y > maxy)
                            maxy = pt[i][j].y;
                    }

                    if (GetMouseX() >= minx && GetMouseX() <= maxx &&
                        GetMouseY() >= miny && GetMouseY() <= maxy)
                    {
                        tri.push_back(&(pt[i]));
                    }
                }
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !!selected)
                {
                    held = true;
                }
            }
            if (held)
            {
                Vector2* near = nullptr;
                for (size_t i = 0; i < pt.size(); ++i)
                {
                    for (size_t j = 0; j < 6; ++j)
                    {
                        if (&(pt[i][j]) == selected)
                            continue;

                        if (Vector2Distance(pt[i][j], GetMousePosition()) < 5.0f)
                        {
                            near = &(pt[i][j]);
                            selected->x = near->x;
                            selected->y = near->y;
                            break;
                        }
                    }
                    if (!!near)
                        break;
                }

                if (!near)
                {
                    selected->x = GetMouseX();
                    selected->y = GetMouseY();
                }
            }
        }


        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            for (size_t i = 0; i < pt.size(); ++i)
            {
                DrawWTri2Q(QuadTriangle2{ pt[i][0], pt[i][1], pt[i][2], pt[i][3], pt[i][4], pt[i][5], }, 0.1f, WHITE);
            }

            for (size_t i = 0; i < tri.size(); ++i)
            {
                Vector2 points[7];
                for (size_t j = 0; j < 6; ++j)
                {
                    points[j] = (*tri[i])[j];
                }
                points[6] = (*tri[i])[0];
                DrawLineStrip(points, 7, GRAY);

                for (size_t j = 0; j < 6; j += 2)
                {
                    DrawRectangleLines(lroundf((*tri[i])[j].x) - 2, lroundf((*tri[i])[j].y) - 2, 5, 5, GRAY);
                    DrawCircle(lroundf((*tri[i])[j + 1].x), lroundf((*tri[i])[j + 1].y), 2, GRAY);
                }
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}