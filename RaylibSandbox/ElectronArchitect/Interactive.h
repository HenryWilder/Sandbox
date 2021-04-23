#pragma once
#include <raylib.h>

struct Draggable_Base
{
	virtual void Move() = 0;
	virtual bool IsHovering() = 0;
};

struct Draggable
{
	Draggable_Base* id;
};

enum DraggingSnap
{
	DRAG_SNAP_CENTER,
	DRAG_RELATIVE,
};

struct Dragger
{
	DraggingSnap snap;
};

struct InteractionData
{
	Dragger dragger;
	struct {
		Vector2 current;
		Vector2 previous;
		Vector2 delta;
	} mousePos;
	std::set<Draggable_Base*> interactive;
	std::set<Draggable_Base*> selection;
};
extern InteractionData g_IA;

void UpdateDragSettings(Dragger);
void TickDragging();

bool IsDraggable(Draggable object);
bool IsBeingDragged(Draggable object);
void BeginDragging(Draggable object);
void StopDragging(Draggable object);

/* Must be manually instantiated for each class you plan to make draggable.
  @Example:
*************************************************
*
*	// Type we want to make draggable:
*	struct Foo {
*		Rectangle collision
*		...
*	};
*
*	// Instantiation you would make for Foo:
*	struct DragDataC<Foo> {
*		static Vector2& GetCenter(T& source) {
*			return source.collision;
*		}
*	};
*
************************************************/
template<typename T> struct DragDataR
{
	static Rectangle& GetRectangle(T* source);
};
// Draggable rectangle
template<typename T>
struct DraggableR : public Draggable_Base
{
	using Functions = DragDataR<T>;
	inline static Rectangle& (*GetRectangle)(T*) = Functions::GetRectangle;

	T* object;

	void Move() override
	{
		Rectangle& rec = GetRectangle(object);
		switch (g_IA.dragger.snap)
		{
		case DRAG_SNAP_CENTER:
			rec.x = g_IA.mousePos.current.x - (rec.width * 0.5f);
			rec.y = g_IA.mousePos.current.y - (rec.height * 0.5f);
			break;
		case DRAG_RELATIVE:
			rec.x += g_IA.mousePos.delta.x;
			rec.y += g_IA.mousePos.delta.y;
			break;
		}
	}
	bool IsHovering() override
	{
		Vector2 pt = g_IA.mousePos.current;
		Rectangle rec = GetRectangle(object);
		return CheckCollisionPointRec(pt, rec);
	}
};
template<typename T>
Draggable LoadDraggableR(T* what)
{
	DraggableR<T>* object = new DraggableR<T>;
	object->object = what;
	g_IA.interactive.insert(object);
	return { object };
}

/* Must be manually instantiated for each class you plan to make draggable.
  @Example:
*************************************************
*
*	// Type we want to make draggable:
*	struct Foo {
*		Vector2 o;
*		float r;
*		...
*	};
*
*	// Instantiation you would make for Foo:
*	struct DragDataC<Foo> {
*		static Vector2& GetCenter(T& source) {
*			return source.o;
*		}
* 		static float GetRadius(T& source) {
*			return source.r;
*		}
*	};
*
************************************************/
template<typename T> struct DragDataC
{
	static Vector2& GetCenter(T* source);
	static float GetRadius(T* source);
};
// Draggable circle/point
template<typename T>
struct DraggableC : public Draggable_Base
{
	using Functions = DragDataC<T>;
	inline static Vector2& (*GetCenter)(T*) = Functions::GetCenter;
	inline static float (*GetRadius)(T*) = Functions::GetRadius;

	T* object;

	void Move() override
	{
		Vector2& center = GetCenter(object);
		center = Vector2Add(center, g_IA.mousePos.delta);
	}
	bool IsHovering() override
	{
		return CheckCollisionPointCircle(g_IA.mousePos.current, GetCenter(object), GetRadius(object));
	}
};
template<typename T>
Draggable LoadDraggableC(T* what)
{
	DraggableC<T>* object = new DraggableC<T>;
	object->object = what;
	g_IA.interactive.insert(object);
	return { object };
}

void UnloadDraggable(Draggable object);
void DeselectAll();
