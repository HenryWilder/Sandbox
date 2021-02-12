#include <raylib.h>
#include "Widgets.h"

namespace gui
{
	class Image : public Widget_Base, WidgetTransparency_Interface
	{
	public:
		Image(Texture2D _tex, Rectangle _estate) :
			Widget_Base(),
			WidgetTransparency_Interface(1.0f),
			mat(new WidgetTexture_Interface(_estate, _tex)) {};

		Image(Color _color, Rectangle _estate) :
			Widget_Base(),
			WidgetTransparency_Interface(1.0f),
			mat(new WidgetColor_Interface(_estate, _color)) {};

		~Image() {
			delete mat;
		}

		void Draw() const override
		{
			Draw();
		}
	private:
		WidgetMaterial_Interface* mat;
	};

	class Button : public Widget_Base, WidgetInteraction_Interface, WidgetTransparency_Interface
	{
	public:
		Button(Rectangle _collision, WidgetMaterial_Interface* _norm, WidgetMaterial_Interface* _hover, WidgetMaterial_Interface* _press, WidgetMaterial_Interface* _disabled) :
			Widget_Base(),
			WidgetInteraction_Interface(_collision),
			WidgetTransparency_Interface(),
			b_disabled(false),
			mat_normal(_norm),
			mat_hovered(_hover),
			mat_pressed(_press),
			mat_disabled(_disabled)
			{};

		void Draw() const override
		{
			if (b_disabled) mat_disabled->Draw();
			else if (WidgetInteraction_Interface::IsDown()) mat_pressed->Draw();
			else if (WidgetInteraction_Interface::IsHovered()) mat_hovered->Draw();
			else mat_normal->Draw();
		}

	protected: // Vars
		bool b_disabled;
		WidgetMaterial_Interface* mat_normal;
		WidgetMaterial_Interface* mat_hovered;
		WidgetMaterial_Interface* mat_pressed;
		WidgetMaterial_Interface* mat_disabled;
	};
}

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;
	InitWindow(windowWidth, windowHeight, "GUI test");
	SetTargetFPS(60);

	Texture2D test = LoadTexture("test.png");

	Image checker = GenImageChecked(2, 2, 1, 1, BLACK, MAGENTA);
	Texture2D missing = LoadTextureFromImage(checker);
	UnloadImage(checker);

	GUI g_ui // Entire GUI
	{
		LoadRenderTexture(windowWidth, windowHeight), // UI buffer

		WidgetCollecion // Widgets
		{
			Rectangle{}, // Collection area
			// Elements
			{
				new gui::Image(missing, Rectangle{0,0,128,128}),
				new gui::Button(Rectangle{128,0,128,128}, new WidgetColor_Interface(),),
			},
			// Children
			{
				// TODO
			}
		}
	};
	g_ui.Crop();
	
	*(Rectangle*)((reinterpret_cast<char*>(&(g_ui.widgets.elements[1]))) + sizeof(Widget_Base)) // Oh god

	while (!WindowShouldClose())
	{
		// Simulation
		// ----------
		// TODO

		// Drawing
		// -------
		BeginDrawing(); {

			ClearBackground(WHITE);
			
			g_ui.Draw();

		} EndDrawing();
	}

	UnloadTexture(missing);
	UnloadTexture(test);

	CloseWindow();

	return 0;
}