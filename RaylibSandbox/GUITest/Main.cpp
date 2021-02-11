#include <raylib.h>
#include "Widgets.h"

class A
{
public:
	A(int v = 0) : m_val(v) {};

	int m_val;

protected:
	virtual void BindModify(int oldVal, int& newVal) { printf("Called class A binding.\n"); };

public:
	void Modify(int newVal) {
		printf("new value: %i\n", newVal);
		BindModify(m_val, newVal);
		printf("final value: %i\n", newVal);
		m_val = newVal;
		printf("m_val updated to: %i\n", m_val);
	}
};
class B : public A
{
public:
	B(int v = 0) : A(v) {};
};
class C : public A
{
public:
	C(int v = 0) : A(v) {};

	void BindModify(int oldVal, int& newVal) override {
		newVal = 22; // I don't want it to change.
		printf("Called class C binding.\n");
		printf("value changed to %i\n", newVal);
	};
};

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;

	A* testB = new B(1);
	A* testC = new C(1);

	testB->Modify(3);
	testC->Modify(3);

	printf("%i\n", (testB->m_val));
	printf("%i\n", (testC->m_val));

	delete testB;
	delete testC;

	return 0;

	InitWindow(1280, 720, "GUI test");
	SetTargetFPS(60);

	GUI g_ui{
		// TODO: Fill out widgets
	};

	while (!WindowShouldClose())
	{
		// Simulation
		// ----------
		// TODO

		// Drawing
		// -------
		BeginDrawing(); {

			ClearBackground(RAYWHITE);
			g_ui.Draw();

		} EndDrawing();
	}

	CloseWindow();

	return 0;
}