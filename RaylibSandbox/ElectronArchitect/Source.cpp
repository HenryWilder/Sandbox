#include <raylib.h>
#include <raymath.h>
#include <set>
#include <unordered_set>
#include <list>
#include "Transistors.h"
#include "Interactive.h"

struct BoardPinID
{
    enum { N, I, O } tag;
    size_t index;
};

struct Block
{
    Rectangle collision;
    Color activeColor;
    Color inactiveColor;
};
struct Ball
{
    Vector2 center;
    float radius;
    Color activeColor;
    Color inactiveColor;
};

template<> struct DragDataR<Block>
{
    static Rectangle& GetRectangle(Block* source)
    {
        return source->collision;
    }
};
template<> struct DragDataC<Ball>
{
    static Vector2& GetCenter(Ball* source)
    {
        return source->center;
    }
    static float GetRadius(Ball* source)
    {
        return source->radius;
    }
};

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Test");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes
    ******************************************/

    Block box{ { 0,0,32,64}, SKYBLUE, BLUE };
    Ball ball{ { 48,128 }, 16.0f, GREEN, DARKGREEN };

    Draggable interactive[] =
    {
        LoadDraggableR(&box),
        LoadDraggableC(&ball)
    };

    Dragger settings { DRAG_RELATIVE, };

    UpdateDragSettings(settings);

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables
        ******************************************/

        for (Draggable element : interactive)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && IsDraggable(element))
            {
                BeginDragging(element);
            }
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                StopDragging(element);
            }
        }

        TickDragging();

        /******************************************
        *   Draw the frame
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            if (IsBeingDragged(interactive[0]) || IsDraggable(interactive[0])) DrawRectangleRec(box.collision, box.activeColor);
            else DrawRectangleRec(box.collision,box.inactiveColor);

            if (IsBeingDragged(interactive[1]) || IsDraggable(interactive[1])) DrawCircleV(ball.center, ball.radius, ball.activeColor);
            else DrawCircleV(ball.center, ball.radius,ball.inactiveColor);


        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory
    ******************************************/

    for (Draggable element : interactive)
    {
        UnloadDraggable(element);
    }

    CloseWindow();
    return 0;
}
#if 0
int mainn()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Electron Architect");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes
    ******************************************/

    // TODO: Load persistent assets & variables

    float pinRadius = 5.0f;
    int boardMargin = 30;

    std::list<IPin*> boardInputs;
    std::list<OPin*> boardOutputs;

    BoardPinID hovered;
    BoardPinID dragging;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables
        ******************************************/

        const float windowMidHeight = (float)windowHeight * 0.5f;
        const float windowLeftMargin = (float)boardMargin;
        const float windowRightMargin = (float)windowWidth - windowLeftMargin;

        Vector2 mousePos = GetMousePosition();

        hovered.tag = hovered.N;
        for (size_t i = 0; i < boardInputs.size(); ++i)
        {
            Vector2 pt{ windowLeftMargin, (float)i };
            pt.y -= ((float)boardInputs.size() * 0.5f);
            pt.y *= (pinRadius * 3.0f);
            pt.y += windowMidHeight;

            if (CheckCollisionPointCircle(mousePos, pt, pinRadius * 1.25f))
            {
                hovered.tag = BoardPinID::I;
                hovered.index = i;
            }
        }
        for (size_t i = 0; i < boardOutputs.size(); ++i)
        {
            Vector2 pt{ windowRightMargin, (float)i };
            pt.y -= ((float)boardOutputs.size() * 0.5f);
            pt.y *= (pinRadius * 3.0f);
            pt.y += windowMidHeight;

            if (CheckCollisionPointCircle(mousePos, pt, pinRadius * 1.25f))
            {
                hovered.tag = BoardPinID::O;
                hovered.index = i;
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            dragging.tag = hovered.tag;
            dragging.index = hovered.index;
        }
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            if ((dragging.tag + dragging.tag) == (BoardPinID::I + BoardPinID::O))
            {
                OPin* from;
                IPin* to;
                if (dragging.tag == BoardPinID::I)
                {
                    from = boardOutputs[hovered.index];
                    to   = boardInputs[dragging.index];
                }
                else
                {
                    from = boardOutputs[dragging.index];
                    to   = boardInputs[hovered.index];
                }
                if (CanConnectPins(from, to)) ConnectPins(from, to);
            }
            dragging.tag = BoardPinID::N;
        }

        TickDraggableUI();

        /******************************************
        *   Draw the frame
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            for (size_t i = 0; i < boardInputs.size(); ++i)
            {
                Vector2 pt{ windowLeftMargin, (float)i };
                pt.y -= ((float)boardInputs.size() * 0.5f);
                pt.y *= (pinRadius * 3.0f);
                pt.y += windowMidHeight;

                if (boardInputs[i]->prev) boardInputs->prev

                if (hovered.tag == hovered.I && i == hovered.index)
                    DrawCircleV(pt,pinRadius * 1.25f,WHITE);
                else
                    DrawCircleV(pt,pinRadius,BLUE);
            }
            for (size_t i = 0; i < boardOutputs.size(); ++i)
            {
                Vector2 pt{ windowRightMargin, (float)i };
                pt.y -= ((float)boardOutputs.size() * 0.5f);
                pt.y *= (pinRadius * 3.0f);
                pt.y += windowMidHeight;

                if (hovered.tag == hovered.O && i == hovered.index)
                    DrawCircleV(pt, pinRadius * 1.25f, WHITE);
                else
                    DrawCircleV(pt, pinRadius, BLUE);
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory
    ******************************************/

    for (IPin* pin : boardInputs)
    {
        delete pin;
    }
    for (OPin* pin : boardOutputs)
    {
        delete pin;
    }

    CloseWindow();

	return 0;
}
#endif