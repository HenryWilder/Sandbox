#include <raylib.h>
#include <raymath.h>
#include <fstream>
#include <vector>

#define sign(x) (((x) > (decltype(x))(0)) - ((x) < (decltype(x))(0)))

using Vector1 = float;

#pragma region Vector math

inline Vector2  operator- (Vector2  vec)          { return      Vector2Negate(vec);             }
inline Vector2  operator+ (Vector2  a, Vector2 b) { return      Vector2Add(a, b);               }
inline Vector2& operator+=(Vector2& a, Vector2 b) { return (a = Vector2Add(a, b));              }
inline Vector2  operator+ (Vector2  a, float val) { return      Vector2AddValue(a, val);        }
inline Vector2& operator+=(Vector2& a, float val) { return (a = Vector2AddValue(a, val));       }
inline Vector2  operator- (Vector2  a, Vector2 b) { return      Vector2Subtract(a, b);          }
inline Vector2& operator-=(Vector2& a, Vector2 b) { return (a = Vector2Subtract(a, b));         }
inline Vector2  operator- (Vector2  a, float val) { return      Vector2SubtractValue(a, val);   }
inline Vector2& operator-=(Vector2& a, float val) { return (a = Vector2SubtractValue(a, val));  }
inline Vector2  operator* (Vector2  a, Vector2 b) { return      Vector2Multiply(a, b);          }
inline Vector2& operator*=(Vector2& a, Vector2 b) { return (a = Vector2Multiply(a, b));         }
inline Vector2  operator* (Vector2  a, float scl) { return      Vector2Scale(a, scl);           }
inline Vector2& operator*=(Vector2& a, float scl) { return (a = Vector2Scale(a, scl));          }
inline Vector2  operator/ (Vector2  a, Vector2 b) { return      Vector2Divide(a, b);            }
inline Vector2& operator/=(Vector2& a, Vector2 b) { return (a = Vector2Divide(a, b));           }
inline Vector2  operator/ (Vector2  a, float div) { return      Vector2Scale(a, 1.0f / div);    }
inline Vector2& operator/=(Vector2& a, float div) { return (a = Vector2Scale(a, 1.0f / div));   }

inline Vector3  operator- (Vector3  vec)          { return      Vector3Negate(vec);             }
inline Vector3  operator+ (Vector3  a, Vector3 b) { return      Vector3Add(a, b);               }
inline Vector3& operator+=(Vector3& a, Vector3 b) { return (a = Vector3Add(a, b));              }
inline Vector3  operator+ (Vector3  a, float val) { return      Vector3AddValue(a, val);        }
inline Vector3& operator+=(Vector3& a, float val) { return (a = Vector3AddValue(a, val));       }
inline Vector3  operator- (Vector3  a, Vector3 b) { return      Vector3Subtract(a, b);          }
inline Vector3& operator-=(Vector3& a, Vector3 b) { return (a = Vector3Subtract(a, b));         }
inline Vector3  operator- (Vector3  a, float val) { return      Vector3SubtractValue(a, val);   }
inline Vector3& operator-=(Vector3& a, float val) { return (a = Vector3SubtractValue(a, val));  }
inline Vector3  operator* (Vector3  a, Vector3 b) { return      Vector3Multiply(a, b);          }
inline Vector3& operator*=(Vector3& a, Vector3 b) { return (a = Vector3Multiply(a, b));         }
inline Vector3  operator* (Vector3  a, float scl) { return      Vector3Scale(a, scl);           }
inline Vector3& operator*=(Vector3& a, float scl) { return (a = Vector3Scale(a, scl));          }
inline Vector3  operator/ (Vector3  a, Vector3 b) { return      Vector3Divide(a, b);            }
inline Vector3& operator/=(Vector3& a, Vector3 b) { return (a = Vector3Divide(a, b));           }
inline Vector3  operator/ (Vector3  a, float div) { return      Vector3Scale(a, 1.0f / div);    }
inline Vector3& operator/=(Vector3& a, float div) { return (a = Vector3Scale(a, 1.0f / div));   }

#pragma endregion

Vector2 Vector2ReflectAcrossPoint(Vector2 cp, Vector2 pt)
{
    return pt - (cp - pt);
}

struct Segment
{
    enum class Type
    {
        EMPTY = 0,
        UNFINISHED,
        LINE, // L
        HORIZONTAL, // H
        VERTICAL, // V
        CUBIC, // C
        SMOOTH_CUBIC, // S
        QUAD, // Q
        SMOOTH_QUAD, // T
    } type;

    union
    {
        struct UnfinishedData { Vector2 cp; } unfinished;
        struct LineData { Vector2 end; } l;
        struct HorizontalData { float x; } h;
        struct VerticalData { float y; } v;
        struct CubicData { Vector2 cp1; Vector2 cp2; Vector2 end; } c;
        struct SmoothCubicData { Vector2 cp2; Vector2 end; } s;
        struct QuadData { Vector2 cp; Vector2 end; } q;
        struct SmoothQuadData { Vector2 end; } t;
    };

    Vector2 GetEnd() const
    {
        switch (type)
        {
        case Segment::Type::LINE: return l.end;
        case Segment::Type::CUBIC: return c.end;
        case Segment::Type::SMOOTH_CUBIC: return s.end;
        case Segment::Type::QUAD: return q.end;
        case Segment::Type::SMOOTH_QUAD: return t.end;
        default: return Vector2Zero();
        }
    }
    bool EndRequiresStart() const
    {
        return type == Type::HORIZONTAL || type == Type::VERTICAL;
    }
    Vector2 GetEndWithStart(Vector2 start) const
    {
        if (type == Type::HORIZONTAL)
            return { h.x, start.y };
        if (type == Type::VERTICAL)
            return { start.x, v.y };
        else
            return GetEnd();
    }


    Vector2 GetLastCP() const
    {
        switch (type)
        {
        case Segment::Type::CUBIC: return c.cp2;
        case Segment::Type::SMOOTH_CUBIC: return s.cp2;
        case Segment::Type::QUAD: return q.cp;
        default: return GetEnd();
        }
    }
    bool LastCPRequiresPrevious() const
    {
        return type == Type::SMOOTH_QUAD;
    }
    // Reflection of the CP across the endpoint
    Vector2 GetLastCPWithPrevious(Vector2 prev) const
    {
        if (type == Type::SMOOTH_QUAD)
            return Vector2ReflectAcrossPoint(prev, t.end);
        else
            return GetLastCP();
    }
};
Segment Segment_Cubic()
{

}

void SimplifySegment(Segment& segment)
{
    if (!segment.EndRequiresStart())
    {
        segment.type = Segment::Type::LINE;
        segment.l.end = segment.GetEnd();
    }
}
void SimplifySegmentWithStart(Segment& segment, Vector2 start)
{
    if (!segment.EndRequiresStart())
    {
        segment.type = Segment::Type::LINE;
        segment.l.end = segment.GetEnd();
        bool matchX = segment.l.end.x == start.x;
        bool matchY = segment.l.end.y == start.y;
        if (matchX && matchY)
            segment.type = Segment::Type::EMPTY;
        if (matchX)
        {
            segment.type = Segment::Type::HORIZONTAL;
            segment.h.x = segment.l.end.x;
        }
        if (matchY)
        {
            segment.type = Segment::Type::VERTICAL;
            segment.v.y = segment.l.end.y;
        }
    }
}
void LineToQuad(Segment& segment, Vector2 cp)
{
    segment.type = Segment::Type::QUAD;
    segment.q.end = segment.l.end;
    segment.q.cp = cp;
}
void QuadToCubic(Segment& segment, Vector2 cp2)
{
    segment.type = Segment::Type::CUBIC;
    Vector2 end = segment.q.end;
    Vector2 cp1 = segment.q.cp;
    segment.c.end = end;
    segment.c.cp1 = cp1;
    segment.c.cp2 = cp2;
}

struct Path
{
    Path(Vector2 _start) : start(_start), segments(), b_closed(false) {}

    Vector2 start; // M

    std::vector<Segment> segments;

    size_t Size() const
    {
        return segments.size() + 1;
    }
    Vector2 GetEndpoint(size_t i) const
    {
        if (i == 0)
            return start;
        else if (segments[i - 1].EndRequiresStart())
            return segments[i - 1].GetEndWithStart(GetEndpoint(i - 1));
        else
            return segments[i - 1].GetEnd();
    }
    Vector2 GetLastCP(size_t i) const
    {
        if (i == 0)
            return start;
        else if (segments[i - 1].LastCPRequiresPrevious())
            return segments[i - 1].GetLastCPWithPrevious(GetLastCP(i - 1));
        else
            return segments[i - 1].GetLastCP();
    }

    bool b_closed; // Z
};

std::ofstream& operator<<(std::ofstream& stream, Vector2 point)
{
    stream << point.x << ',' << point.y;
    return stream;
}
std::ofstream& operator<<(std::ofstream& stream, Rectangle rec)
{
    stream << rec.x << ' ' << rec.y << ' ' << rec.width << ' ' << rec.height;
    return stream;
}
std::ofstream& operator<<(std::ofstream& stream, const Path& path)
{
    stream << "<path d=\"M";
    stream << path.start;
    for (size_t i = 0; i < path.segments.size(); ++i)
    {
        switch (path.segments[i].type)
        {
        case Segment::Type::LINE:
            stream << 'L';
            stream << path.segments[i].l.end;
            break;
        case Segment::Type::HORIZONTAL:
            stream << 'H';
            stream << path.segments[i].h.x;
            break;
        case Segment::Type::VERTICAL:
            stream << 'V';
            stream << path.segments[i].v.y;
            break;
        case Segment::Type::CUBIC:
            stream << 'C';
            stream << path.segments[i].c.cp1 << ',';
            stream << path.segments[i].c.cp2 << ',';
            stream << path.segments[i].c.end;
            break;
        case Segment::Type::SMOOTH_CUBIC:
            stream << 'S';
            stream << path.segments[i].s.cp2 << ',';
            stream << path.segments[i].s.end;
            break;
        case Segment::Type::QUAD:
            stream << 'Q';
            stream << path.segments[i].q.cp << ',';
            stream << path.segments[i].q.end;
            break;
        case Segment::Type::SMOOTH_QUAD:
            stream << 'T';
            stream << path.segments[i].t.end;
            break;
        }
    }
    if (path.b_closed)
        stream << 'Z';
    stream << "\"/>";
    return stream;
}

void Save(const char* filename, Rectangle viewbox, const std::vector<Path*>& paths)
{
    std::ofstream file(filename);
    file << R"txt(<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" viewBox=")txt";
    file << viewbox << "\">\n";
    for (size_t i = 0; i < paths.size(); ++i)
    {
        file << "   ";
        file << *(paths[i]) << '\n';
    }
    file << "</svg>";
    file.close();
}

Vector2 Vector2Qerp(Vector2 p0, Vector2 p2, Vector2 p4, float t)
{
    Vector2 p1 = Vector2Lerp(p0, p2, t);
    Vector2 p3 = Vector2Lerp(p2, p4, t);
    return Vector2Lerp(p1, p3, t);
}
Vector2 Vector2Cerp(Vector2 p0, Vector2 p2, Vector2 p4, Vector2 p6, float t)
{
    Vector2 p1 = Vector2Lerp(p0, p2, t);
    Vector2 p3 = Vector2Lerp(p2, p4, t);
    Vector2 p5 = Vector2Lerp(p4, p6, t);
    return Vector2Qerp(p1, p3, p5, t);
}

void DrawBezierQuad(Vector2 p0, Vector2 cp, Vector2 p1, Color color)
{
    Vector2 last = p0;
    Vector2 current;
    for (float t = 0.04f; t <= 0.96f; t += 0.04f)
    {
        current = Vector2Qerp(p0, cp, p1, t);
        DrawLineV(last, current, color);
        last = current;
    }
    DrawLineV(last, p1, color);
}

void DrawBezierCubic(Vector2 p0, Vector2 cp0, Vector2 cp1, Vector2 p1, Color color)
{
    Vector2 last = p0;
    Vector2 current;
    for (float t = 0.04f; t <= 0.96f; t += 0.04f)
    {
        current = Vector2Cerp(p0, cp0, cp1, p1, t);
        DrawLineV(last, current, color);
        last = current;
    }
    DrawLineV(last, p1, color);
}

Vector2 DrawSegment(Vector2 start, const Segment& seg, Color color)
{
    switch (seg.type)
    {
    case Segment::Type::LINE:
        DrawLineV(start, seg.l.end, color);
        return seg.l.end;

    case Segment::Type::HORIZONTAL: {
        Vector2 end = { seg.h.x, start.y };
        DrawLineV(start, end, color);
        return end;
    }

    case Segment::Type::VERTICAL: {
        Vector2 end = { start.x, seg.v.y };
        DrawLineV(start, end, color);
        return end;
    }

    case Segment::Type::CUBIC:
        DrawBezierCubic(start, seg.c.cp1, seg.c.cp2, seg.c.end, color);
        return seg.c.end;

    case Segment::Type::QUAD:
        DrawBezierQuad(start, seg.q.cp, seg.q.end, color);
        return seg.q.end;

    default:
        return Vector2Zero();
    }
}
void DrawPath(const Path* path, Color color)
{
    if (path->segments.empty())
        DrawPixelV(path->start, color);
    else
    {
        Vector2 prev = path->start;
        for (size_t i = 0; i < path->segments.size(); ++i)
        {
            prev = DrawSegment(prev, path->segments[i], color);
        }

        if (path->b_closed)
            DrawLineV(path->segments.back().l.end, path->start, color);
    }
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    std::vector<Path*> data;
    Path* active = nullptr;
    Vector2 clickPoint = { -100,-100 };

    auto FinishPath = [&data, &active]() {
        active = nullptr;
    };

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // On press
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            clickPoint = GetMousePosition();
            // New path
            if (!active)
            {
                active = new Path(clickPoint);
                data.push_back(active);
                Segment start;
                start.type = Segment::Type::CUBIC;
                start.c.cp1 = start.c.cp2 = start.c.end = clickPoint;
                active->segments.push_back(start);
            }
            // Close path
            else if (Vector2Distance(clickPoint, active->start) < 5)
            {
                active->b_closed = true;
                FinishPath();
            }
            // Continue path
            else
            {
                if (!active->segments.empty() && active->segments.back().type == Segment::Type::UNFINISHED)
                {
                    active->segments.back().type = Segment::Type::CUBIC;
                    active->segments.back().c.cp1 = active->segments.back().unfinished.cp;
                    active->segments.back().c.cp2 = active->segments.back().c.end = GetMousePosition();
                }
                else
                {
                    Segment next;
                    next.type = Segment::Type::LINE;
                    next.l.end = GetMousePosition();
                    active->segments.push_back(next);
                }
            }
        }
        else if (!!active)
        {
            // Dragging
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                active->segments.back().c.cp2 = Vector2ReflectAcrossPoint(GetMousePosition(), active->GetEndpoint(active->Size() - 1));
            }
            else
            {

            }

            // Dragged
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) &&
                (Vector2Distance(clickPoint, GetMousePosition()) > 5))
            {
                // Add second CP to most recent endpoint
                Vector2 end = active->segments.back().GetEnd();
                active->segments.back().type = Segment::Type::CUBIC;
                active->segments.back().c.cp1 = Vector2ReflectAcrossPoint(active->GetLastCP(active->Size() - 2), active->GetEndpoint(active->Size() - 2));
                active->segments.back().c.cp2 = GetMousePosition();
                active->segments.back().c.end = end;

                // Create first CP for upcoming segment
                Segment next;
                next.type = Segment::Type::UNFINISHED;
                next.unfinished.cp = GetMousePosition();
                active->segments.push_back(next);
            }
            // Finish path
            if (IsKeyPressed(KEY_ENTER))
            {
                FinishPath();
            }
            // Discard path
            if (IsKeyPressed(KEY_ESCAPE))
            {
                data.pop_back();
                delete active;
                FinishPath();
            }
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(WHITE);

            for (size_t i = 0; i < data.size(); ++i)
            {
                DrawPath(data[i], BLACK);
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    Save("Test.svg", { 0,0,(float)windowWidth,(float)windowHeight }, data);

    CloseWindow();

	return 0;
}