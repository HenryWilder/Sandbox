#pragma once
#include "Button.h"
#include "Slider.h"

namespace std {
	// Declaration
	template<class _Ty, class _Alloc = _Ty>
	class vector;
}

class InputWidgetPtr {
	enum {
		input_slider,
		input_button,
		input_select,
	} toi; // Type of input

	union {
		Slider* s;
		Button* b;
	};
};
void Update(std::vector<InputWidgetPtr> inputs);
void Draw(std::vector<InputWidgetPtr> inputs);

class InputWidget {
	enum {
		input_slider,
		input_button,
		input_select,
	} toi; // Type of input

	union {
		Slider s;
		Button b;
	};
};
void Update(std::vector<InputWidget> inputs);
void Draw(std::vector<InputWidget> inputs);