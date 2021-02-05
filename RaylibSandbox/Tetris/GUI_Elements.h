#pragma once
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

struct GUI_Elem_Base
{
	GUI_Elem_Base(void* _collision) : collision(_collision), color_up(), color_hovered(), color_down(color_hovered) {};

	void* collision;
	Color color_up, color_hovered, color_down;

	virtual bool IsHovered() const = 0;
	virtual bool IsPressed() const = 0;
	virtual bool IsDown() const = 0;
	virtual bool IsReleased() const = 0;
	virtual bool IsUp() const = 0;

	virtual void Draw() const = 0;
};

struct Button_Circle : public GUI_Elem_Base
{
	Button_Circle() : GUI_Elem_Base(&circ_collisionShape), circ_collisionShape() {};
	Button_Circle(Vector2 position, float radius) : GUI_Elem_Base(&circ_collisionShape), circ_collisionShape{ position, radius } {};

	Circle circ_collisionShape;

	bool IsHovered() const override {
		return CheckCollisionPointCircleC(GetMousePosition(), circ_collisionShape);
	}
	bool IsPressed() const override {
		return (IsHovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
	}
	bool IsDown() const override {
		return (IsHovered() && IsMouseButtonDown(MOUSE_LEFT_BUTTON));
	}
	bool IsReleased() const override {
		return (IsHovered() && IsMouseButtonReleased(MOUSE_LEFT_BUTTON));
	}
	bool IsUp() const override {
		return (IsHovered() && IsMouseButtonUp(MOUSE_LEFT_BUTTON));
	}
	void Draw() const override
	{
		Color drawColor;
		if (IsHovered()) {
			if (IsDown()) drawColor = color_down;
			else drawColor = color_hovered;
		} else drawColor = color_up;
		DrawCircleC(circ_collisionShape, drawColor);
	}
};

class Button1 : public Button_Circle
{

};

struct Dial : public GUI_Elem_Base
{

};