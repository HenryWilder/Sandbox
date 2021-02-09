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

#pragma region Base classes

struct GUI_Elem_Base
{
protected:

	GUI_Elem_Base(Rectangle _collision) : collision(_collision), color_up(), color_hovered(), color_down(color_hovered), b_down(false) {};

	Rectangle collision;
	Color color_up, color_hovered, color_down;
	bool b_down;

	void SetIsDown(bool val)
	{
		b_down = val;
	}

	virtual bool IsHovered() const = 0;
	virtual bool IsPressed() const = 0;
	virtual bool IsDown() const = 0;
	virtual bool IsReleased() const = 0;
	virtual bool IsUp() const = 0;

	virtual void OnHover() = 0;
	virtual void OnPress() = 0;
	virtual void OnRelease() = 0;

	void Press() {
		SetIsDown(true);
		OnPress();
	}
	void Release() {
		SetIsDown(false);
		OnRelease();
	}

public:

	virtual void UpdateButton()
	{
		if (IsHovered()) OnHover();

		if (IsPressed()) OnPress();
		else if (IsReleased()) OnRelease();
	}

	virtual void Draw() const = 0;
};

struct Button_Circle_Base : public GUI_Elem_Base
{
protected:

	Button_Circle_Base() : GUI_Elem_Base(&circ_collisionShape), circ_collisionShape() {};
	Button_Circle_Base(Vector2 _center, float _radius) : GUI_Elem_Base(&circ_collisionShape), circ_collisionShape{ _center, _radius } {};

	Circle circ_collisionShape;

	bool IsHovered() const override {
		return CheckCollisionPointCircleC(GetMousePosition(), circ_collisionShape);
	}
	bool IsPressed() const override {
		bool b = ();
		if (!b_down) b_down = b;
		return IsHovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
	}
	bool IsDown() const override {
		return b_down;
	}
	bool IsReleased() const override {
		bool b = (b_down && IsMouseButtonReleased(MOUSE_LEFT_BUTTON));
		if (b_down) b_down = b;
		return b;
	}
	bool IsUp() const override {
		return !b_down;
	}

public:

	void Draw() const override
	{
		Color drawColor;
		if (IsHovered()) drawColor = color_hovered;
		else if (IsDown()) drawColor = color_down;
		else drawColor = color_up;
		DrawCircleC(circ_collisionShape, drawColor);
	}
};

class Dial_Base : public Button_Circle_Base
{
protected:

	Dial_Base(Vector2 _center, float _radius) : Button_Circle_Base(_center, _radius) {};

	void OnHover() override
	{

	}
	void OnPress() override
	{

	}
	void OnRelease() override
	{

	}
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

class Dial1 : public Dial_Base
{
public:

	Dial1(Vector2 _center, float _radius) : Dial_Base(_center, _radius) {};

	void OnHover() override
	{
		// TODO: Add hover function

		Dial_Base::OnHover();
	}
	void OnPress() override
	{
		// TODO: Add press function

		Dial_Base::OnPress();
	}
	void OnRelease() override
	{
		// TODO: Add release function

		Dial_Base::OnRelease();
	}
};

#pragma endregion

struct ScreenBufferNode // Quad-Tree
{
	ScreenBufferNode* children[4];
	std::vector<GUI_Elem_Base*> elements;
};

ScreenBufferNode g_GUIBuffer;