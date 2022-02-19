#pragma once

class ITool
{
	enum class Input
	{
		MouseLeft,
		MouseRight,
		MouseMiddle
	};
	void Input_Event();
	void Input_Axis();
};

class Tool_Select
{

};

class Tool_