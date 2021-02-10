#pragma once
#include <raylib.h>

struct Circle
{
	Vector2 center;
	float radius;
};
bool CheckCollisionPointCircleC(Vector2 point, Circle circle)
{
	return CheckCollisionPointCircle(point, circle.center, circle.radius);
}
void DrawCircleC(Circle circle, Color color)
{
	DrawCircleV(circle.center, circle.radius, color);
}

#pragma region Base classes

class GUI_TextBox
{
protected:
	GUI_TextBox() :
		text(std::string("")),
		x(0),
		y(0),
		fontSize(8),
		textColor(WHITE) {};

	GUI_TextBox(const char* _text, int _x, int _y, Color _textColor) :
		text(_text),
		x(_x),
		y(_y),
		textColor(_textColor) {};

	GUI_TextBox(const std::string& _text, int _x, int _y, Color _textColor) :
		text(_text),
		x(_x),
		y(_y),
		textColor(_textColor) {};

	GUI_TextBox(const char* _text, Vector2 _pos, Color _textColor) :
		text(_text),
		x((int)_pos.x),
		y((int)_pos.y),
		textColor(_textColor) {};

	GUI_TextBox(const std::string& _text, Vector2 _pos, Color _textColor) :
		text(_text),
		x((int)_pos.x),
		y((int)_pos.y),
		textColor(_textColor) {};

public:
	void Draw()
	{
		DrawText(text.c_str(), x, y, fontSize, textColor);
	}

private:
	std::string text;
	int x, y;
	int fontSize;
	Color textColor;
};

class GUI_Elem_Base
{
protected:
	GUI_Elem_Base(Rectangle _collision, Color _color_up, Color _color_hovered, Color _color_down) :
		collision(_collision),
		color_up(_color_up),
		color_hovered(_color_hovered),
		color_down(_color_down),
		b_down(false),
		b_hovering(false) {};

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
	Rectangle collision;
	Color color_up, color_hovered, color_down;
	bool b_down, b_hovering;
};

#pragma endregion

#pragma region Specializations

class Button1 : public Button_Circle_Base
{
public:

	Button1(Vector2 _center, float _radius) : Button_Circle_Base(_center, _radius) {};

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

struct ScreenBufferNode // Quad-Tree
{
	ScreenBufferNode* children[4];
	std::vector<GUI_Elem_Base*> elements;
};

ScreenBufferNode g_GUIBuffer;