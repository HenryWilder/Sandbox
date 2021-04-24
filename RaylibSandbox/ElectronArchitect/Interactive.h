#pragma once
#include <set>
#include <raylib.h>

struct Draggable_Base
{
	virtual void Move() = 0;
	virtual bool IsHovering() = 0;
};

// 0-flags mean default
enum DraggingFlags
{
	DRAG_SNAP_RELATIVE		= 0b00,
	DRAG_SNAP_CENTER		= 0b01,

	DRAG_COLLISION_IGNORE	= 0b00,
	DRAG_COLLISION_BLOCKS	= 0b10, // TODO
};

struct InteractionData
{
	int settings;
	struct {
		Vector2 current;
		Vector2 previous;
		Vector2 delta;
	} mousePos;
	std::set<Draggable_Base*> interactive;
	std::set<Draggable_Base*> selection;
};
extern InteractionData g_IA;

struct Draggable
{
	Draggable_Base* id;
};

void UpdateDragSettings(int settings);
void TickDragging(); // Call every frame
bool IsHovered(Draggable object); // Whether the mouse is hovering the object
bool IsBeingDragged(Draggable object); // Whether the object is in the selection
void BeginDragging(Draggable object);
void StopDragging(Draggable object);
template<typename T> Draggable LoadDraggableR(T* what); // Requires an instatiation of DragDataR<T>
template<typename T> Draggable LoadDraggableC(T* what); // Requires an instatiation of DragDataC<T>
void UnloadDraggable(Draggable object);
void DeselectAll();

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
		if (g_IA.settings & DRAG_SNAP_CENTER)
		{
			rec.x = g_IA.mousePos.current.x - (rec.width * 0.5f);
			rec.y = g_IA.mousePos.current.y - (rec.height * 0.5f);
		}
		else
		{
			rec.x += g_IA.mousePos.delta.x;
			rec.y += g_IA.mousePos.delta.y;
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
		if (g_IA.settings & DRAG_SNAP_CENTER)
		{
			center = g_IA.mousePos.current;
		}
		else
		{
			center = Vector2Add(center, g_IA.mousePos.delta);
		}
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

#define DEFINE_DRAG_DATA_RECTANGLE(TYPE,sourcePointerParameterName,collisionRectangle) \
template<> struct DragDataR<TYPE> { static Rectangle& GetRectangle(TYPE* sourcePointerParameterName) { return collisionRectangle; } }

#define DEFINE_DRAG_DATA_CIRCLE(TYPE,sourcePointerParameterName,centerPoint,radius) \
template<> struct DragDataC<TYPE> { static Vector2& GetCenter(TYPE* sourcePointerParameterName) { return centerPoint; } static float GetRadius(TYPE* sourcePointerParameterName) { return radius; } }