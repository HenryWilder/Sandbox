#pragma once
#include <raylib.h>
#include <vector>
#include <array>
#include <string>
#include <any>
#include <variant>

template<class T>
class Validator
{
public:
	Validator() : b_valid(false), m_value{} {};
	Validator(T _value) : b_valid(true), m_value(_value) {};

	operator bool() { return b_valid; }
	operator T() { return m_value; }

private:
	bool b_valid;
	T m_value;
};

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
class Widget_Base
{
public: // Constructors
	Widget_Base(bool _hidden = false, bool _volatile = false)
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

class WidgetEstate_Interface
{
public:
	WidgetEstate_Interface(Rectangle _estate) : estate(_estate) {};

	Rectangle GetEstate() const;

private:
	Rectangle estate;
};
bool CheckCollisionPointWidget(Vector2 pt, const Widget_Base* widget);

// Interface for enabling overall fade effects on a widget
class WidgetTransparency_Interface
{
protected:
	WidgetTransparency_Interface() : alpha(1.0f) {};
	WidgetTransparency_Interface(float _alpha) : alpha(_alpha) {};

protected:
	Color Apply(Color color);	// Applies transparency to an input tint

protected:
	float alpha;	// Opacity of the widget
};

// Widget_Base which can be hovered/clicked
class WidgetInteraction_Interface : public Widget_Base, WidgetEstate_Interface
{
public: // Constructors
	WidgetInteraction_Interface(Rectangle _shape)
		: Widget_Base(), WidgetEstate_Interface(_shape), b_hovered(false), b_down(false) {};
	WidgetInteraction_Interface(Rectangle _shape, bool _hidden)
		: Widget_Base(_hidden), WidgetEstate_Interface(_shape), b_hovered(false), b_down(false) {};

public: // Variable helper functions
	bool IsHovered() const;				// Test whether the widget is being hovered
	bool IsDown()	 const;				// Test whether the mouse press-button is down on the widget
	Rectangle GetEstate() const;

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
	bool
		b_hovered,						// Whether the widget is being hovered by the mouse
		b_down;							// Whether the mouse is down while hovering the widget
};

class WidgetMaterial_Interface : protected WidgetEstate_Interface
{
public:
	WidgetMaterial_Interface(Rectangle _estate, Color _tint) : WidgetEstate_Interface(_estate), tint(_tint) {};

protected: virtual void Draw(Rectangle) const = 0;
public:    void Draw() const;

protected:
	  Color tint;							// The color tint to apply to the texture when drawing
};

// Interface for a single texture that replaces the widget's rectangle at draw-time
class WidgetTexture_Interface : public WidgetMaterial_Interface
{
public: // Enums
	enum class TileMode : unsigned char	// How to handle texture-shape mismatch
	{
		None, Fill, Fit, Stretch, Repeat,
	};

public: // Constructors
	WidgetTexture_Interface(Rectangle _estate, Texture2D _tex, Color _tint = WHITE, TileMode _tileHandling = TileMode::None) :
		WidgetMaterial_Interface(_estate, _tint),
		tex(_tex),
		src({ 0, 0, (float)_tex.width, (float)_tex.height}),
		tileHandling(_tileHandling) {};

	WidgetTexture_Interface(Rectangle _estate, Texture2D _tex, Rectangle _src, Color _tint = WHITE, TileMode _tileHandling = TileMode::None) :
		WidgetMaterial_Interface(_estate, _tint),
		tex(_tex),
		src(_src),
		tileHandling(_tileHandling) {};

protected:
	void Draw(Rectangle dest) const override;

protected: // Vars
	Texture2D tex;						// The texture to draw the element with
	Rectangle src;						// What section of the texture to sample
	TileMode tileHandling;				// How to handle source-destination dimensions mismatch
};

class WidgetColor_Interface : public WidgetMaterial_Interface
{
public:
	WidgetColor_Interface(Rectangle _estate, Color _tint) : WidgetMaterial_Interface(_estate, _tint) {};
	void Draw(Rectangle shape) const override;
};

// Interface for text inside a box
class WidgetTextBox_Interface
{
public: // Constructors
	WidgetTextBox_Interface(Rectangle _box,
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
	void Crop();
	void CropRecursive();

	void Draw() const;

	bool CheckPointInside(Vector2 pt) const;

	WidgetCollecion* Traverse(Vector2 pt);
	Widget_Base* ContainingElement(Vector2 pt);
	Widget_Base* FindElementAt(Vector2 pt);

	void ContainingElement_Multi(std::vector<Widget_Base*>& out, Vector2 pt);
	void FindElementAt_Multi(std::vector<Widget_Base*>& out, Vector2 pt);

public:
	Rectangle realEstate;
	std::vector<Widget_Base*> elements;
	std::vector<WidgetCollecion> children;
};

class GUI
{
public:
	void Draw() const;
	void CheckCollisions(Vector2 cursor);
	void Crop();

public:
	RenderTexture2D uiBuffer;
	WidgetCollecion widgets;
};