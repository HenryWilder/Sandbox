#pragma once
#define _ARG_COUNT(x,x16,x15,x14,x13,x12,x11,x10,x9,x8,x7,x6,x5,x4,x3,x2,x1,n,...) n
#define NUM_ARGS(...) _ARG_COUNT(0, __VA_ARGS__ ,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define _APPEND(a,b) a##b
#define APPEND(a,b) _APPEND(a,b)
// Adds an underscore to the start of the input, making it follow the internal/temporary name format
#define _(x) _APPEND(_, x )

// ~~~~~~~~
// | AXIS |
// ~~~~~~~~

#define AXIS_0(...) static_assert(false, "Incorrect number of args!")
#define AXIS_1(key,scale) ( (float)IsKeyDown(key) * scale )
#define AXIS_2(a,a1,b,b1) AXIS_1(a,a1) + AXIS_1(b,b1)
#define AXIS_3(a,a1,...)  AXIS_1(a,a1) + AXIS_2(__VA_ARGS__)
#define AXIS_4(a,a1,...)  AXIS_1(a,a1) + AXIS_3(__VA_ARGS__)
#define AXIS_5(a,a1,...)  AXIS_1(a,a1) + AXIS_4(__VA_ARGS__)
#define AXIS_6(a,a1,...)  AXIS_1(a,a1) + AXIS_5(__VA_ARGS__)
#define AXIS_7(a,a1,...)  AXIS_1(a,a1) + AXIS_6(__VA_ARGS__)
#define AXIS_8(a,a1,...)  AXIS_1(a,a1) + AXIS_7(__VA_ARGS__)
#define AXIS_9(a,a1,...)  AXIS_1(a,a1) + AXIS_8(__VA_ARGS__)
#define AXIS_10(a,a1,...) AXIS_1(a,a1) + AXIS_9(__VA_ARGS__)
#define AXIS_11(a,a1,...) AXIS_1(a,a1) + AXIS_10(__VA_ARGS__)
#define AXIS_12(a,a1,...) AXIS_1(a,a1) + AXIS_11(__VA_ARGS__)
#define AXIS_13(a,a1,...) AXIS_1(a,a1) + AXIS_12(__VA_ARGS__)
#define AXIS_14(a,a1,...) AXIS_1(a,a1) + AXIS_13(__VA_ARGS__)
#define AXIS_15(a,a1,...) AXIS_1(a,a1) + AXIS_14(__VA_ARGS__)
#define AXIS_16(a,a1,...) AXIS_1(a,a1) + AXIS_15(__VA_ARGS__)

#define _AXIS_N(n,...) AXIS_##n##(__VA_ARGS__)
#define AXIS_N(n,...) _AXIS_N(n, __VA_ARGS__)

#define AXIS_VALUE(name) Axis_##name##_Value()
// Event happens once per tick anyway. here is no AXIS_TICK() macro/function.
#define AXIS_EVENT(name) Axis_##name##_Event()

#define _ARG_COUNT_AXIS(x,x32,x31,x30,x29,x28,x27,x26,x25,x24,x23,x22,x21,x20,x19,x18,x17,x16,x15,x14,x13,x12,x11,x10,x9,x8,x7,x6,x5,x4,x3,x2,x1,n,...) n
#define NUM_ARGS_AXIS(...) _ARG_COUNT_AXIS(0, __VA_ARGS__ ,16,0,15,0,14,0,13,0,12,0,11,0,10,0,9,0,8,0,7,0,6,0,5,0,4,0,3,0,2,0,1,0,0)

/***********************************************
*
*	// Value of the axis
*	float Axis_Name_Value()
*	{
*		// Returns the sum of the (scales multiplied by the values)
*		return (((float)IsKeyDown(key1) * key1Scale) +
*				((float)IsKeyDown(key2) * key2Scale));
*	}
*	// Function called per tick
*	void Axis_Name_Event()
*	{
*		// Defined by the user
*	}
*
***********************************************/
// Binds keys to an input axis
#define INPUT_AXIS(name,...)\
	float AXIS_VALUE(name)\
	{\
		return AXIS_N(NUM_ARGS_AXIS(__VA_ARGS__), __VA_ARGS__);\
	}\
	void AXIS_EVENT(name)

// ~~~~~~~~~~
// | ACTION |
// ~~~~~~~~~~

#define ACTION_0(...) static_assert(false, "Incorrect number of args!")
#define ACTION_1(key) IsKeyPressed(key)
#define ACTION_2(a,b)    ACTION_1(a) || ACTION_1(b)
#define ACTION_3(a,...)  ACTION_1(a) || ACTION_2(__VA_ARGS__)
#define ACTION_4(a,...)  ACTION_1(a) || ACTION_3(__VA_ARGS__)
#define ACTION_5(a,...)  ACTION_1(a) || ACTION_4(__VA_ARGS__)
#define ACTION_6(a,...)  ACTION_1(a) || ACTION_5(__VA_ARGS__)
#define ACTION_7(a,...)  ACTION_1(a) || ACTION_6(__VA_ARGS__)
#define ACTION_8(a,...)  ACTION_1(a) || ACTION_7(__VA_ARGS__)
#define ACTION_9(a,...)  ACTION_1(a) || ACTION_8(__VA_ARGS__)
#define ACTION_10(a,...) ACTION_1(a) || ACTION_9(__VA_ARGS__)
#define ACTION_11(a,...) ACTION_1(a) || ACTION_10(__VA_ARGS__)
#define ACTION_12(a,...) ACTION_1(a) || ACTION_11(__VA_ARGS__)
#define ACTION_13(a,...) ACTION_1(a) || ACTION_12(__VA_ARGS__)
#define ACTION_14(a,...) ACTION_1(a) || ACTION_13(__VA_ARGS__)
#define ACTION_15(a,...) ACTION_1(a) || ACTION_14(__VA_ARGS__)
#define ACTION_16(a,...) ACTION_1(a) || ACTION_15(__VA_ARGS__)

#define _ACTION_N(n,...) ACTION_##n##(__VA_ARGS__)
#define ACTION_N(n,...) _ACTION_N(n, __VA_ARGS__)

#define ACTION_VALUE(name) Action_##name##_Value()
#define ACTION_TICK(name) Action_##name##_Tick()
#define ACTION_EVENT(name) Action_##name##_Event()

/***********************************************
*
*	// Whether the input has been pressed
*	bool Action_Name_Value()
*	{
*		return (IsKeyPressed(KEY_NAME));
*	}
*
*	// Forward-declare
*	void Action_Name_Event();
*
*	// Function called per-tick
*	void Action_Name_Tick()
*	{
*		// If the input key is pressed
*		if (Action_Name_Value())
*		{
*			// Dispatch input event
*			Action_Name_Event();
*		}
*	}
*
*	// Function called on event
*	void Action_Name_Event()
*	{
*		// Defined by the user
*	}
*
***********************************************/
// Binds keys to an input action
#define INPUT_ACTION(name,...)\
	bool ACTION_VALUE(name)\
	{\
		return ( ACTION_N(NUM_ARGS(__VA_ARGS__), __VA_ARGS__) );\
	}\
	void ACTION_EVENT(name);\
	void ACTION_TICK(name)\
	{\
		if (ACTION_VALUE(name)) { ACTION_EVENT(name); }\
	}\
	void ACTION_EVENT(name)
