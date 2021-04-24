#include <raymath.h>
#include "Interactive.h"

InteractionData g_IA;

void UpdateDragSettings(int data)
{
	g_IA.settings = data;
}

void TickDragging()
{
	g_IA.mousePos.previous = g_IA.mousePos.current;
	g_IA.mousePos.current = GetMousePosition();
	g_IA.mousePos.delta = Vector2Subtract(g_IA.mousePos.current, g_IA.mousePos.previous);

	for (Draggable_Base* draggable : g_IA.selection)
	{
		draggable->Move();
	}
}

bool IsHovered(Draggable object)
{
	return object.id->IsHovering();
}
bool IsBeingDragged(Draggable object)
{
	return (g_IA.selection.find(object.id) != g_IA.selection.end());
}
void BeginDragging(Draggable object)
{
	g_IA.selection.insert(object.id);
}
void StopDragging(Draggable object)
{
	g_IA.selection.erase(object.id);
}

void UnloadDraggable(Draggable object)
{
	g_IA.interactive.erase(object.id);
	delete object.id;
}
void DeselectAll()
{
	g_IA.selection.clear();
}