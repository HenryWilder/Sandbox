#include <raylib.h>
#include <raymath.h>
#include <vector>

Vector2 Vector2Snap(const Vector2& vector, float gridSize)
{
    return Vector2{
        ((float)((int)((vector.x / gridSize) + 0.5f)) * gridSize),
        ((float)((int)((vector.y / gridSize) + 0.5f)) * gridSize)
    };
}

enum class WireDirection : char
{
    XFirst = 0,
    YFirst,
    Direct,
};

void DrawSnappedLine(Vector2 start, Vector2 end, Color color, WireDirection direction)
{
    switch (direction)
    {
    case WireDirection::XFirst:
        DrawLineV(start, { start.x, end.y }, color);
        DrawLineV({ start.x, end.y }, end, color);
        break;
    case WireDirection::YFirst:
        DrawLineV(start, { end.x, start.y }, color);
        DrawLineV({ end.x, start.y }, end, color);
        break;
    case WireDirection::Direct:
        DrawLineV(start, end, color);
        break;
    }
}

bool Vector2Equal(const Vector2& a, const Vector2& b)
{
    return
        abs(a.x - b.x) < 2.0f &&
        abs(a.y - b.y) < 2.0f;
}

// Just transmits signals from start to end
struct Wire
{
    Wire() : startPos{}, endPos{}, fromWire{ nullptr }, toWire{ nullptr }, direction{}, active{} {};

    Wire(Vector2 _startPos, Vector2 _endPos, WireDirection _direction, std::vector<Wire*>* wireArr = nullptr)
    {
        startPos = _startPos;
        endPos = _endPos;
        direction = _direction;

        fromWire = nullptr;
        toWire = nullptr;

        if (wireArr) SearchConnectableWires(wireArr);

        active = false;
        if (fromWire) active = fromWire->active;
    };

    Vector2 startPos, endPos;
    Wire* fromWire;
    Wire* toWire;
    WireDirection direction;
    bool active;

    void Draw()
    {
        Color color;
        if (this->active) color = RED;
        else color = WHITE;
        DrawSnappedLine(startPos, endPos, color, direction);
    }

    void SearchConnectableWires(std::vector<Wire*>* wireArr)
    {
        for (Wire* search : *wireArr)
        {
            if (Vector2Equal(search->endPos, this->startPos))
            {
                this->fromWire = search;
                search->toWire = this;
            }
            if (fromWire) break;
        }
        for (Wire* search : *wireArr)
        {
            if (Vector2Equal(this->endPos, search->startPos))
            {
                this->toWire = search;
                search->fromWire = this;
            }
            if (toWire) break;
        }
        if (fromWire) fromWire->active = true;
        if (toWire) toWire->active = true;
        if (fromWire || toWire) this->active = true;
    }
};

struct InputMode
{
    enum class Mode : char
    {
        None = 0,
        Wire,
        Gate,
    };
    Mode mode;

    union Data
    {
        WireDirection wireDirection;
    };
    Data data;
};

int main(void)
{
    const int WindowWidth = 1280;
    const int WindowHeight = 720;

    std::vector<Wire*> wires;

    auto Vector2InvertY = [&WindowHeight](Vector2 vector) {
        return Vector2{ vector.x, WindowHeight - vector.y };
    };

    InitWindow(WindowWidth, WindowHeight, "Electron Architect");

    Vector2 cursorPos = { -100.0f, -100.0f };
    Vector2 wireStart = { -100.0f, -100.0f };

    InputMode mode = {};

    while (!WindowShouldClose())
    {
        // Update vars
        //---------------------------------------------
        cursorPos = GetMousePosition();
        cursorPos = Vector2Snap(cursorPos, 8.0f);

        switch (mode.mode)
        {
        case InputMode::Mode::None:
        {
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                mode.mode = InputMode::Mode::Gate;
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                wireStart = cursorPos;
                mode.mode = InputMode::Mode::Wire;
            }
        }
        break;

        case InputMode::Mode::Wire:
        {
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                Wire* newWire = new Wire;
                newWire->startPos = wireStart;
                newWire->endPos = cursorPos;
                newWire->direction = mode.data.wireDirection;
                newWire->active = false;
                wires.push_back(newWire);

                mode.mode = InputMode::Mode::None;
            }
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                mode.data.wireDirection = (WireDirection)(((char)mode.data.wireDirection + 1) % 4);
            }
        }
        break;

        case InputMode::Mode::Gate:
        {
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                mode.mode = InputMode::Mode::None;
                break;
            }
        }
        break;

        }

        // Draw
        //---------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        // Draw cursor
        {
            Color cursorColor = WHITE;
            if (mode.mode == InputMode::Mode::Wire) cursorColor = RED;
            else if (mode.mode == InputMode::Mode::Gate) cursorColor = YELLOW;

            DrawRectangleV({ cursorPos.x - 2, cursorPos.y - 2 }, { 4,4 }, cursorColor);
        }

        if (mode.mode == InputMode::Mode::Wire)
        {
            DrawSnappedLine(wireStart, cursorPos, GRAY, mode.data.wireDirection); // Temporary wire
        }

        if (mode.mode == InputMode::Mode::Gate)
        {
            
        }

        // Draw the current architecture
        for (Wire* wire : wires) { wire->Draw(); }

        EndDrawing();
    }

    // Return memory to heap
    //--------------------------

    for (Wire* wire : wires)
    {
        delete wire;
    }
    wires.clear();

    return 0;
}


