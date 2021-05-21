#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Math.h"
#include "Button.h"

Button::Button(ButtonData data) :
	m_buttonTrueColor(data.buttonTrueColor),
	m_position(data.position),
	b_state(data.defaultState),
	b_enabled(data.enabled),
	b_visible(data.visible)
{}

bool Button::GetValue() const {
	return b_state;
}

void Button::ToggleValue() {
	b_state = !b_state;
}

constexpr float Button::GetButtonWidth() {
	return (float)s_buttonWidth;
}

constexpr float Button::GetButtonHeight() {
	return (float)s_buttonHeight;
}

constexpr float Button::GetButtonHalfWidth() {
	return (float)(s_buttonWidth / 2);
}

constexpr float Button::GetButtonHalfHeight() {
	return (float)(s_buttonHeight / 2);
}

float Button::GetButtonX() const {
	return m_position.x;
}
float Button::GetButtonY() const {
	return m_position.y;
}

Vector2 Button::GetButtonPosition() const {
	return m_position;
}

Vector2 Button::GetButtonCenterPosition() const {
	return {
		GetButtonX() + GetButtonHalfWidth(),
		GetButtonY() + GetButtonHalfHeight()
	};
}

Rectangle Button::GetButtonRect() const {
	return Rectangle{
		GetButtonX(),
		GetButtonY(),
		GetButtonWidth(),
		GetButtonHeight()
	};
}

bool Button::IsHovered() const {
	return CheckCollisionPointRec(
		GetMousePosition(),
		GetButtonRect()
		);
}

Color Button::GetButtonColor() const {
	return m_buttonTrueColor;
}

void Button::Update(std::vector<Button>& vec) {
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		for (Button& button : vec) {

			if (!button.b_enabled || !button.b_visible)
				continue;

			if (button.IsHovered()) {
				button.b_state = !button.b_state;
				break;
			}
		}
	}
}

void Button::Draw() const {
	Rectangle rec = GetButtonRect();
	DrawRectangleRec(rec, m_buttonTrueColor);
	if (!GetValue()) {
		rec.x += 1.0f;
		rec.y += 1.0f;
		rec.width -= 2.0f;
		rec.height -= 2.0f;
		DrawRectangleRec(rec, s_buttonFalseColor);
	}
}

void Button::Draw(std::vector<Button>& vec) {
	for (Button& button : vec) {

		if (!button.b_visible)
			continue;

		button.Draw();
	}
}
