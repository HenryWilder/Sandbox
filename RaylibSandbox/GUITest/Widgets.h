#pragma once
#include <raylib.h>
#include <vector>
#include <array>
#include <string>
#include <any>
#include <variant>

struct PaddedRec
{
	/*************************************************
	*
	*	Offsets are relative to the shape they pad.
	*
	*	          Top
	*	    .------|------.
	*	    |  |   v   |  |
	*	    |--+-------+--|
	*	Left-->|       |<--Right
	*	    |--+-------+--|
	*	    |  |   ^   |  |
	*	    '------|------'
	*	         Bottom
	*
	*************************************************/

	Rectangle GetSubRec();

	Rectangle rec;
	float left, right, top, bottom;
};
struct PaddedCirc
{
	Vector2 center;
	float radius;
	float inset;
};

// Base widget class
class Widget
{
public: // Constructors
	Widget(bool _hidden = false, bool _volatile = false)
		: b_hidden(_hidden), b_dirty(true), b_volatile(_volatile) {};

public: // Variable helper functions
	void Update() const;				// Force the widget to be redrawn at draw time
	bool IsHidden() const;				// Test whether the widget is hidden
	bool IsVisible() const;				// Test whether the widget is NOT hidden
	void Hide();						// Hide the widget
	void UnHide();						// Unhide the widget

	virtual void Draw() const = 0;		// Draws the widget
	void _Draw() const;					// Draws the widget (called by UI handler)

private: // Vars
	bool
		b_hidden,						// Whether to draw the widget
		b_volatile;						// Always redraw the widget, regardless of change
	mutable bool b_dirty;				// Whether the widget needs to be re-drawn this frame
};

// Interface for enabling overall fade effects on a widget
class WidgetTransparency
{
protected:
	WidgetTransparency() : alpha(1.0f) {};
	WidgetTransparency(float _alpha) : alpha(_alpha) {};

protected:
	Color Apply(Color color);	// Applies transparency to an input tint

protected:
	float alpha;	// Opacity of the widget
};

// Widget which can be hovered/clicked
class Widget_Interaction : public Widget
{
public: // Constructors
	Widget_Interaction(Rectangle _shape)
		: Widget(), collision(_shape), b_hovered(false), b_down(false) {};
	Widget_Interaction(Rectangle _shape, bool _hidden)
		: Widget(_hidden), collision(_shape), b_hovered(false), b_down(false) {};

public: // Variable helper functions
	bool IsHovered() const;				// Test whether the widget is being hovered
	bool IsDown()	 const;				// Test whether the mouse press-button is down on the widget

protected: // Bindings
	virtual void OnPress() = 0;			// Binding for when the widget gets pressed
	virtual void OnRelease() = 0;		// Binding for when the widget gets released
	virtual void OnMouseEnter() = 0;	// Binding for when the widget is hovered
	virtual void OnMouseLeave() = 0;	// Binding for when the widget is un-hovered

public: // UI-handler interaction functions
	bool CheckCollisionPoint(Vector2 pt);

	void Press();						// Tell the widget the mouse has pressed it
	void Release();						// Tell the widget the mouse has released it
	void MouseEnter();					// Tell the widget the mouse has entered its collision
	void MouseLeave();					// Tell the widget the mouse has left its collision

private: // Vars
	Rectangle collision;				// Space the mouse must be inside of to be considered a collision
	bool
		b_hovered,						// Whether the widget is being hovered by the mouse
		b_down;							// Whether the mouse is down while hovering the widget
};
bool CheckCollisionPointWidget(Vector2 pt, const Widget* widget);

// Interface for a single texture that replaces the widget's rectangle at draw-time
class Widget_Texture
{
public: // Enums
	enum class TileMode					// How to handle texture-shape mismatch
	{
		None, Fill, Fit, Stretch, Repeat,
	};

public: // Constructors
	Widget_Texture(Texture2D _tex, TileMode _tileHandling, Color _tint, Vector2 _scale)
		: tex(_tex), tileHandling(_tileHandling), tint(_tint), scale(_scale) {};

private: // Vars
	Texture2D tex;						// The texture to draw the element with
	TileMode tileHandling;
	Color tint;							// The color tint to apply to the texture when drawing
	Vector2 scale;						// The scaling to apply to the texture (applied after being matched to the shape)
};

// Interface for text inside a box
class TextBox
{
public: // Constructors
	TextBox(Rectangle _box,
			const std::string& _str = "",			// Leave _str as "" when the first value of str should come from a SetString() call
			PaddedRec _pad = {0.0f, 0.0f, 0.0f, 0.0f},
			float _fontSize = 8.0f) :
		box(_box),
		str(_str),
		pad(_pad),
		fontSize(_fontSize) {};

protected: // Bindings
	virtual void OnStringChange(					// Binding called when str is changed outside of the class
		std::string oldStr,							// Copy of the current value of str
		std::string& newStr							// Modifyable reference to the new value of str, passed prior to str being updated to match
	) = 0;
	virtual void OnResize(							// Binding called when box is changed outside of the class
		Rectangle oldShape,							// Copy of the current value of box
		Rectangle& newShape							// Modifyable reference to the new value of box, passed prior to box being updated to match
	) = 0;


public: // Variable helper functions
	std::string GetString_Cpy() const;				// Get a copy of str
	const std::string& GetString_ConstRef() const;	// Get a read-only reference to str
	std::string& GetString_Ref();					// Get a writable reference to str (WARNING: ignores OnStringChange())
	void SetString(std::string _str);				// Set the value of str, calling OnStringChange() in the process
	void Resize(Rectangle newShape);				// Change the shape of the element, calling OnResize() in the process

private: // Vars
	Rectangle box;									// The box the string will be displayed inside of
	std::string str;								// The text to display
	PaddedRec pad;									// Text padding (in pixels)
	float fontSize;									// Fontsize of the text when drawn
};

// Screen section containing multiple widgets
class WidgetCollecion
{
public:
	void Draw() const;

public:
	std::vector<WidgetCollecion*> children;
	std::vector<Widget*> elements;
};

class GUI
{
public:
	void Draw() const;

public:
	RenderTexture2D uiBuffer;
	WidgetCollecion widgets;
};