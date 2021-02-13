#include <raylib.h>
#include "Widgets.h"

Rectangle PaddedRec::GetSubRec()
{
	return Rectangle{
		rec.x + left,
		rec.y + top,
		rec.width - right,
		rec.height - bottom
	};
}

#pragma region Widget_Base

void Widget_Base::Update() const
{
	b_dirty = true;
}

// Variable helper functions
bool Widget_Base::IsHidden() const { return b_hidden; }
bool Widget_Base::IsVisible() const { return !b_hidden; }
void Widget_Base::Hide() { b_hidden = true; }
void Widget_Base::UnHide() { b_hidden = false; }

void Widget_Base::_Draw() const
{
	if (b_dirty || b_volatile) Draw();
	b_dirty = false;
}

#pragma endregion

Validator<Rectangle> TryGetWidgetEstate(Widget_Base* const widget)
{
	WidgetEstate_Interface* estateWidget = dynamic_cast<WidgetEstate_Interface*>(widget);
	if (estateWidget) return Validator<Rectangle>(estateWidget->GetEstate());
	else return Validator<Rectangle>();
}

bool CheckCollisionPointWidget(Vector2 pt, Widget_Base* const widget)
{
	Validator<Rectangle> rec = TryGetWidgetEstate(widget);
	if (rec) return CheckCollisionPointRec(pt, rec);
	else return false;
}

#pragma region WidgetInteraction_Interface

// Variable helper functions
bool WidgetInteraction_Interface::IsHovered() const { return b_hovered; }

bool WidgetInteraction_Interface::IsDown() const { return b_down; }

Rectangle WidgetInteraction_Interface::GetEstate() const
{
	return WidgetEstate_Interface::GetEstate();
}

bool WidgetInteraction_Interface::CheckCollisionPoint(Vector2 pt)
{
	if (this) return CheckCollisionPointRec(pt, GetEstate());
	else return false;
}

// Interaction functions (To be called by the UI handler)
void WidgetInteraction_Interface::Press() { b_down = true;  OnPress(); }
void WidgetInteraction_Interface::Release() { b_down = false; OnRelease(); }
void WidgetInteraction_Interface::MouseEnter() { b_hovered = true;  OnMouseEnter(); }
void WidgetInteraction_Interface::MouseLeave() { b_hovered = false; OnMouseEnter(); }

#pragma endregion

#pragma region WidgetTexture_Interface

void WidgetTexture_Interface::Draw(Rectangle dest) const
{
	switch (tileHandling)
	{
	case WidgetTexture_Interface::TileMode::None:
		DrawTexturePro(tex, src, { 0.0f, 0.0f, dest.width, dest.height }, { dest.x, dest.y }, 0.0f, tint);
		break;
	case WidgetTexture_Interface::TileMode::Fill:
		// TODO
		break;
	case WidgetTexture_Interface::TileMode::Fit:
		// TODO
		break;
	case WidgetTexture_Interface::TileMode::Stretch:
		// TODO
		break;
	case WidgetTexture_Interface::TileMode::Repeat:
		DrawTextureTiled(tex, src, src, { dest.x, dest.y }, 0.0f, 1.0f, tint);
		break;
	default:
		break;
	}
}

#pragma endregion

#pragma region WidgetTextBox_Interface

std::string WidgetTextBox_Interface::GetString_Cpy() const { return str; }
const std::string& WidgetTextBox_Interface::GetString_ConstRef() const { return str; }
std::string& WidgetTextBox_Interface::GetString_Ref() { return str; }
void WidgetTextBox_Interface::SetString(std::string _str) {
	OnStringChange(str, _str);
	str = _str;
}

void WidgetTextBox_Interface::Resize(Rectangle newShape)
{
	OnResize(pad.rec, newShape);
	pad.rec = newShape;
}

#pragma endregion

#pragma region WidgetTransparency_Interface

Color WidgetTransparency_Interface::Apply(Color color) { return ColorAlpha(color, alpha); }

#pragma endregion

#pragma region WidgetCollecion

void WidgetCollecion::Crop()
{
	Rectangle guess = Rectangle{ (float)GetScreenWidth(), (float)GetScreenHeight(), 0.0f, 0.0f };

	for (Widget_Base* widget : elements)
	{
		Rectangle space;
		WidgetEstate_Interface* estate = dynamic_cast<WidgetEstate_Interface*>(widget);
		if (!estate) continue;
		space = estate->GetEstate();

		if (space.x < guess.x) guess.x = space.x;
		if (space.y < guess.y) guess.y = space.y;
		float absoluteSize = space.x + space.width;
		if (absoluteSize > (guess.x + guess.width)) guess.width = absoluteSize - guess.x;
		absoluteSize = space.y + space.height;
		if (absoluteSize > (guess.y + guess.height)) guess.height = absoluteSize - guess.y;
	}

	realEstate = guess;
}

void WidgetCollecion::CropRecursive()
{
	Crop();
	for (WidgetCollecion child : children)
	{
		child.CropRecursive();
	}
}

void WidgetCollecion::Draw() const
{
	ClearBackground({ 0,0,0, 0 });

	for (Widget_Base* widget : elements)
	{
		widget->Draw();
	}
}

bool WidgetCollecion::CheckPointInside(Vector2 pt) const
{
	return CheckCollisionPointRec(pt, realEstate);
}

WidgetCollecion* WidgetCollecion::Traverse(Vector2 pt)
{
	if (this) {
		for (WidgetCollecion& node : children) {
			if (node.CheckPointInside(pt)) return node.Traverse(pt);
		}
	}
	return this;
}

Widget_Base* WidgetCollecion::ContainingElement(Vector2 pt)
{
	for (Widget_Base* elem : elements) {
		if (CheckCollisionPointWidget(pt, elem)) return elem;
	}
	return nullptr;
}

Widget_Base* WidgetCollecion::FindElementAt(Vector2 pt)
{
	return Traverse(pt)->ContainingElement(pt);
}

void WidgetCollecion::ContainingElement_Multi(std::vector<Widget_Base*>& out, Vector2 pt)
{
	for (Widget_Base* elem : elements) {
		if (CheckCollisionPointWidget(pt, elem)) out.push_back(elem);
	}
}

void WidgetCollecion::FindElementAt_Multi(std::vector<Widget_Base*>& out, Vector2 pt)
{
	return Traverse(pt)->ContainingElement_Multi(out, pt);
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

void GUI::CheckCollisions(Vector2 cursor)
{
	widgets.FindElementAt(cursor);
}

void GUI::Crop()
{
	widgets.CropRecursive();
}

#pragma endregion

Rectangle WidgetEstate_Interface::GetEstate() const
{
	return estate;
}

void WidgetMaterial_Interface::Draw() const
{
	Draw(GetEstate());
}

void WidgetColor_Interface::Draw(Rectangle collision) const
{
	DrawRectangleRec(GetEstate(), tint);
}
