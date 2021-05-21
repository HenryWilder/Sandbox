#include <raylib.h>
#include <raymath.h>
#include <assert.h>
#include <type_traits>
#include <vector>
#include "InputUI.h"

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    float value = 0.0f;

    std::vector<Slider> sliders;
    sliders.reserve(3);

    SliderData slider;

    slider.position = { 20, 20 };
    slider.min = -3.0f;
    slider.max = 3.0f;
    slider.increment = 0.0f;
    sliders.push_back(Slider(slider));

    slider.position.y = 40;
    slider.min = -3.0f;
    slider.max = 10.0f;
    slider.increment = 1.0f;
    sliders.push_back(Slider(slider));

    slider.position.y = 60;
    slider.min = 1.0f;
    slider.max = 8.0f;
    slider.increment = 0.0f;
    slider.defaultValue = 2.0f;
    sliders.push_back(Slider(slider));

    sliders.push_back(Slider());
    sliders.push_back(Slider());
    sliders.push_back(Slider());

    ColorSlider colorCtrl({ 20,80 }, sliders);

    std::vector<Button> buttons;
    buttons.push_back(Button(ButtonData{ { 140, 20 } }));

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        Slider::Update(sliders);
        Button::Update(buttons);

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            Slider::Draw(sliders);
            Button::Draw(buttons);

            DrawRectangle(138, 78, 68, 68, WHITE);
            DrawRectangle(140, 80, 64, 64, colorCtrl.GetValue());

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}