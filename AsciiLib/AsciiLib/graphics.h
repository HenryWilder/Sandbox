#pragma once

/**************
*
*	Defines:
* 
*	COLOR (defaults false)
*	Whether to draw the characters using the 24-bit RGB color pallet
*	as opposed to simply white
* 
*	BIG_RAMP (defaults true)
*	Whether to use the standard
*	" .\'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"
*	as opposed to the more convincing, but smaller
*	" .:-=+*#%@"
* 
***************/

#ifndef COLOR
#define COLOR 0
#endif // !TRUE_COLOR

#ifndef BIG_RAMP
#define BIG_RAMP 1
#endif // !BIG_RAMP

struct Color
{
	unsigned char r, g, b;
	char c; // Symbol to display with color applied (does not have to be in the ramp)
};

typedef unsigned char Gray;

#if COLOR
	#define _VALUE_t Color

	#define _ClearBackground ClearBackground_c
	#define _InitBuffer InitBuffer_c
	#define _FlushBuffer FlushBuffer_c

	#define _DrawBuffer DrawBuffer_c

#else // !COLOR
	#define _VALUE_t Gray

	#if BIG_RAMP

		#define _DrawBuffer DrawBufferb

	#else // !BIG_RAMP

		#define _DrawBuffer DrawBufferl

	#endif // BIG_RAMP

	#define _ClearBackground ClearBackground_g
	#define _InitBuffer InitBuffer_g
	#define _FlushBuffer FlushBuffer_g

#endif // COLOR

inline void ClearBackground(_VALUE_t val) { _ClearBackground(val); }
inline void InitBuffer(int width, int height) { _InitBuffer(width,height); }
inline void FlushBuffer() { _FlushBuffer(); }
inline void DrawBuffer() { _DrawBuffer(); }
