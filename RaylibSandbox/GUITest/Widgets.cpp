#include <raylib.h>
#include "Widgets.h"

#pragma region Widget

void Widget::Update() const
{
	b_dirty = true;
}

// Variable helper functions
bool Widget::IsHidden() const { return b_hidden; }
bool Widget::IsVisible() const { return !b_hidden; }
void Widget::Hide() { b_hidden = true; }
void Widget::UnHide() { b_hidden = false; }

void Widget::_Draw() const
{
	if (b_dirty || b_volatile) Draw();
	b_dirty = false;
}

#pragma endregion

#pragma region InteractableWidget

// Variable helper functions
bool Widget_Interaction::IsHovered() const { return b_hovered; }

bool Widget_Interaction::IsDown() const { return b_down; }

bool Widget_Interaction::CheckCollisionPoint(Vector2 pt)
{
	return CheckCollisionPointRec(pt, collision);
}

// Interaction functions (To be called by the UI handler)
void Widget_Interaction::Press() { b_down = true;  OnPress(); }
void Widget_Interaction::Release() { b_down = false; OnRelease(); }
void Widget_Interaction::MouseEnter() { b_hovered = true;  OnMouseEnter(); }
void Widget_Interaction::MouseLeave() { b_hovered = false; OnMouseEnter(); }

bool CheckCollisionPointWidget(Vector2 pt, Widget* const widget)
{
	Widget_Interaction* wi = dynamic_cast<Widget_Interaction*>(widget);
	if (wi) return wi->CheckCollisionPoint(pt);
	else return false;
}
#pragma endregion

#pragma region TextBox

std::string TextBox::GetString_Cpy() const { return str; }
const std::string& TextBox::GetString_ConstRef() const { return str; }
std::string& TextBox::GetString_Ref() { return str; }
void TextBox::SetString(std::string _str) {
	OnStringChange(str, _str);
	str = _str;
}

void TextBox::Resize(Rectangle newShape)
{
	OnResize(pad.rec, newShape);
	pad.rec = newShape;
}

#pragma endregion

Rectangle PaddedRec::GetSubRec()
{
	return Rectangle{
		rec.x + left,
		rec.y + top,
		rec.width - right,
		rec.height - bottom
	};
}

#pragma region WidgetTransparency

Color WidgetTransparency::Apply(Color color) { return ColorAlpha(color, alpha); }

#pragma endregion

#pragma region WidgetCollecion

void WidgetCollecion::Draw() const
{
}

#pragma endregion

#pragma region GUI

void GUI::Draw() const
{
	BeginTextureMode(uiBuffer); {

		widgets.Draw();

	} EndTextureMode();

	DrawTextureRec(uiBuffer.texture, { 0, 0, (float)uiBuffer.texture.width, -(float)uiBuffer.texture.height }, {0,0}, WHITE);
}

#pragma endregion