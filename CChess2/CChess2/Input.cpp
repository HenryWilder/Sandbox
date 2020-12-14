#include "Input.h"
#define DEBUG_MODE false
#if DEBUG_MODE
#include "Rendering.h"
#endif

using namespace space;
using namespace screen;
using namespace output;

HANDLE g_console;

Mouse g_mouse;

void InitInput()
{
    // A handle for the input stream
    g_console = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(g_console, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

    /*
    for (int i = 0; i < KeyStateSlots; ++i)
    {
        g_keyStates[i] = KeyState::Up;
    }
    */
}
Coord Mouse::MouseToBoardspace(COORD pos)
{
    PixelPos out;
    out.x = pos.X * 8;
    out.y = pos.Y * 16;
    out = out / gameScale;
    return BoardSpace(out);
}

bool Mouse::CheckMouseState()
{
    bool state = b_mouseClicked;

    b_mouseClicked = false;

    return state;
}

Coord Mouse::GetMouseClickCoord()
{
    return m_clickCoord;
}

Coord Mouse::ReadMouseHover()
{
    return m_tileCoord;
}

void Mouse::MouseEventProc(MOUSE_EVENT_RECORD mEvent)
{
    switch (mEvent.dwEventFlags)
    {
    case 0:
        if (mEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
            b_mouseClicked = true;
            m_clickCoord = m_tileCoord;
        }
        break;

    case MOUSE_MOVED:
        m_tileCoord = MouseToBoardspace(mEvent.dwMousePosition);

        // Clamp to board
        m_tileCoord.x = min(m_tileCoord.x, game::sideTileCount - 1);
        m_tileCoord.y = min(m_tileCoord.y, game::sideTileCount - 1);

#if DEBUG_MODE
        /* --TESTING-- */
        g_frameBuffer.DrawGridSpaceFAST(m_tileCoord, RGB(255,0,0));
        g_frameBuffer.DrawPixel(m_tileCoord, RGB(0, 255, 0));
#endif
        

        break;

    default:
        break;
    }
}

void PingInput()
{
    DWORD eventCount = 0;

    INPUT_RECORD irec;

    while (true)
    {
        if (PeekConsoleInput(g_console, &irec, 1, &eventCount) && eventCount > 0)
        {
            ReadConsoleInput(g_console, &irec, 1, &eventCount);

            if (irec.EventType == MOUSE_EVENT) g_mouse.MouseEventProc(irec.Event.MouseEvent);
        }
        else break;
    }
}
