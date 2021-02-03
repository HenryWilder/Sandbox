#include <raylib.h>
#include <raymath.h>
#include "Global.h"

namespace sprite  
{
    Texture2D missing;

    Texture2D pawn;
    Texture2D rook;
    Texture2D knight;
    Texture2D bishop;
    Texture2D queen;
    Texture2D king;
}

namespace conv  
{
    // "To" overloads
    int To(int i, int dest)
    {
        return (i * dest);
    }
    float To(float f, int dest)
    {
        return (f * (float)(dest));
    }
    Vector2 To(Vector2 vec, int dest)
    {
        return Vector2{
            vec.x * (float)(dest),
            vec.y * (float)(dest),
        };
    }
    Rectangle To(Rectangle rec, int dest)
    {
        return Rectangle{
            rec.x * (float)(dest),
            rec.y * (float)(dest),
            rec.width * (float)(dest),
            rec.height * (float)(dest),
        };
    }

    // "From" overloads
    int From(int i, int src)
    {
        return (i / src);
    }
    float From(float f, int src)
    {
        return (f / (float)(src));
    }
    Vector2 From(Vector2 vec, int src)
    {
        return Vector2{
            vec.x / (float)(src),
            vec.y / (float)(src),
        };
    }
    Rectangle From(Rectangle rec, int src)
    {
        return Rectangle{
            rec.x / (float)(src),
            rec.y / (float)(src),
            rec.width / (float)(src),
            rec.height / (float)(src),
        };
    }

    // Rectangle functions
    Vector2 QuarterDim(Rectangle rec)
    {
        return Vector2{
            rec.width * 0.5f,
            rec.height * 0.5f,
        };
    }
    Rectangle Quarter(Rectangle rec)
    {
        Vector2 dim = QuarterDim(rec);
        return Rectangle{
            rec.x,
            rec.y,
            dim.x,
            dim.y,
        };
    }
    Rectangle Offset(Rectangle rec, Vector2 amount)
    {
        return Rectangle{
            rec.x + amount.x,
            rec.y + amount.y,
            rec.width,
            rec.height,
        };
    }
    // Subdivision overloads
    Rectangle Subdivide(Rectangle rec, bool xIsLarge, bool yIsLarge)
    {
        Vector2 dim = QuarterDim(rec);
        return Rectangle{
            rec.x + (xIsLarge ? dim.x : 0.0f),
            rec.y + (yIsLarge ? dim.y : 0.0f),
            dim.x,
            dim.y,
        };
    }
    Rectangle Subdivide(Rectangle rec, char quad)
    {
        return Subdivide(
            rec,
            (quad & 0b10),
            (quad & 0b01)
        );
    }
}