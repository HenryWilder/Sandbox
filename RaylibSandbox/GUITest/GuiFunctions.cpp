#include "GuiFunctions.h"

Color ColorTint(Color color, Color tint)
{
    return {
        (unsigned char)((int)color.r * (int)tint.r),
        (unsigned char)((int)color.g * (int)tint.g),
        (unsigned char)((int)color.b * (int)tint.b),
        (unsigned char)((int)color.a * (int)tint.a)
    };
}

Rectangle GetShape(Rectangle rec)
{
    return { 0.0f, 0.0f, rec.width, rec.height };
}

Vector2 GetPos(Rectangle rec)
{
    return { rec.x, rec.y };
}

void UnloadGUIMat(GUIMat mat)
{
    switch (mat.type)
    {
    default: break;
    case GUIMat::MatType::Mat_Texture: UnloadTexture(mat.data.t); break;
    case GUIMat::MatType::Mat_Material:
        UnloadShader(mat.data.m.shader);
        UnloadMaterial(mat.data.m);
        break;
    }
}

Texture g_defaultTexture;

void DrawGUIMat(GUIMat mat, Rectangle dest, Vector2 origin, float rotation, Color tint)
{
    switch (mat.type)
    {
    case GUIMat::MatType::Mat_Color: DrawRectanglePro(dest, origin, rotation, mat.data.c); break;
    case GUIMat::MatType::Mat_Texture: DrawTexturePro(mat.data.t, mat.src, dest, origin, rotation, tint); break;
    case GUIMat::MatType::Mat_Material:
        BeginShaderMode(mat.data.m.shader); {

            DrawTexturePro(g_defaultTexture, mat.src, dest, origin, rotation, tint);

        } EndShaderMode();
        break;
    }
}

ButtonStateFlags GetButtonState(GUIButton button)
{
    ButtonStateFlags flags = ButtonStateFlags::Up;

    if (!button.b_disabled && CheckCollisionPointRec(GetMousePosition(), button.collision))
    {
        flags |= ButtonStateFlags::Hovered;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            flags |= ButtonStateFlags::Down;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            flags |= ButtonStateFlags::Released;
    }

    return flags;
}

GUIMat& GetButtonDrawMat(GUIButton button, ButtonStateFlags state)
{
    if (button.b_disabled) {
        return button.mat_disabled; // Disabled
    }
    else {
        if (IsButtonStateHovered(state)) { // Hovering
            if (IsButtonStateDown(state))
                return button.mat_pressed; // Mouse down
            else
                return button.mat_hovered; // Mouse up
        }
        else {
            return button.mat_default; // Not hovering
        }
    }
}

void DrawGUIButton(GUIButton button, ButtonStateFlags state)
{
    DrawGUIMat(GetButtonDrawMat(button, state), button.collision, { 0,0 }, 0.0f, button.tint);
}

void nullfnc() {}

bool IsButtonStateUp(ButtonStateFlags state)
{
    return !((char)state & (char)ButtonStateFlags::Down);
}

bool IsButtonStateDown(ButtonStateFlags state)
{
    return ((char)state & (char)ButtonStateFlags::Down);;
}

bool IsButtonStateActive(ButtonStateFlags state)
{
    return ((char)state & (char)ButtonStateFlags::Released);
}

bool IsButtonStatePassive(ButtonStateFlags state)
{
    return !((char)state & (char)ButtonStateFlags::Released);
}

bool IsButtonStateHovered(ButtonStateFlags state)
{
    return ((char)state & (char)ButtonStateFlags::Hovered);
}

ButtonStateFlags operator|(ButtonStateFlags lval, ButtonStateFlags rval)
{
    return ButtonStateFlags((unsigned char)lval | (unsigned char)rval);
}
ButtonStateFlags& operator|=(ButtonStateFlags& lval, ButtonStateFlags rval)
{
    return (lval = (lval | rval));
}
ButtonStateFlags operator&(ButtonStateFlags lval, ButtonStateFlags rval)
{
    return ButtonStateFlags((unsigned char)lval & (unsigned char)rval);
}