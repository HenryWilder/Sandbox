#include "Thread.h"
#include "Notecard.h"
#include <raymath.h>

Vector2 Thread::StartPosition() const
{
    return start->PinPosition();
}

Vector2 Thread::EndPosition() const
{
    return end->PinPosition();
}

void Thread::Draw() const
{
    Vector2 shadowOffset = {-2,2};

    Vector2 startPos = StartPosition();
    Vector2 endPos = EndPosition();

    // Shadow
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawLineEx(Vector2Add(startPos, shadowOffset), Vector2Add(endPos, shadowOffset), thickness, {0,0,0,100});
    EndBlendMode();

    DrawLineEx(startPos, endPos, thickness, color);
}

bool Thread::IsHovered(Vector2 point) const
{
    Vector2 startPos = StartPosition();
    Vector2 endPos = EndPosition();

    float lengthSquared = Vector2LengthSqr(Vector2Subtract(endPos, startPos));

    // Start and end overlap
    if (lengthSquared == 0.0f)
    {
        bool pointMatches =
            startPos.x == point.x &&
            startPos.y == point.y;

        return pointMatches;
    }

    Vector2 endRelativeToStart = Vector2Subtract(endPos, startPos);
    float t = Clamp(Vector2DotProduct(Vector2Subtract(point, startPos), endRelativeToStart) / lengthSquared, 0, 1);
    Vector2 projection = Vector2Add(startPos, Vector2Scale(endRelativeToStart, t));

    bool hovering = Vector2Distance(point, projection) <= thickness;

    return hovering;
}

std::vector<Thread*> g_threads;

void CreateThread(Color color, Notecard* start, Notecard* end)
{
    Thread* thread = new Thread(color, start, end);
	g_threads.push_back(thread);
    start->AddThreadConnection(thread);
    end->AddThreadConnection(thread);
}

void DestroyThread(Thread* thread)
{
    auto it = std::find(g_threads.begin(), g_threads.end(), thread);
    if (it != g_threads.end())
    {
        g_threads.erase(it);
        thread->start->RemoveThreadConnection(thread);
        thread->end->RemoveThreadConnection(thread);
        delete thread;
    }
    else
    {
        _ASSERT_EXPR(false, L"Thread was created by unorthodox means, or has already been destroyed.");
    }
}
