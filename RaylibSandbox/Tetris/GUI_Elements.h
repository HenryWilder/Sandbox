#pragma once
#include <raylib.h>

#pragma region Base classes

class Widget
{
protected:
	Widget(Rectangle _collision, Color _tint_up, Color _tint_hovered, Color _tint_down, Texture2D _texture) :
		collision(_collision),
		tint_up(_tint_up),
		tint_hovered(_tint_hovered),
		tint_down(_tint_down),
		b_down(false),
		b_hovering(false),
		texture(_texture) {};

public:
	bool IsHovered() const {
		return b_hovering;
	}
	virtual bool IsDown() const {
		return b_down;
	}

protected:
	virtual void OnHover() = 0;
	virtual void OnEndHover() = 0;
	virtual void OnPress() = 0;
	virtual void OnRelease() = 0;

public:
	void Press() {
		b_down = true;
		OnPress();
	}
	void Release() {
		b_down = false;
		OnRelease();
	}
	void MouseEnter() {
		b_hovering = true;
		OnHover();
	}
	void MouseExit() {
		b_hovering = false;
		OnEndHover();
	}

	virtual void Draw() const = 0;

protected:
	Rectangle collision; // 16 bytes
	Color tint_up, tint_hovered, tint_down; // 3 * 4 (12) bytes
	bool b_down, b_hovering; // 2 * 1 (2) bytes
	Texture texture; // 20 bytes
};

#pragma endregion

#pragma region Specializations

class Button1 : public Widget
{
public:

	Button1(Vector2 _center, float _radius) : Widget(_center, _radius) {};

	void OnHover() override
	{
		// TODO: Add hover function
	}
	void OnPress() override
	{
		// TODO: Add press function
	}
	void OnRelease() override
	{
		// TODO: Add release function
	}
};

#pragma endregion

struct WidgetCollection // Quad-Tree
{
	WidgetCollection* children[4];
	std::vector<GUI_Elem_Base*> elements;
};

static WidgetCollection g_ui{

};