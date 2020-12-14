#include "Buffer.h"


Pixel::Pixel(Color col)
{
	CColor color = CColor(col);
	ESC[0] = '\x1b';
	ESC[1] = '[';
	ESC[2] = '4';
	ESC[3] = '8';
	ESC[4] = ';';
	ESC[5] = '2';
	ESC[6] = ';';
	pcolor[0] = color.r[0];
	pcolor[1] = color.r[1];
	pcolor[2] = color.r[2];
	pcolor[3] = ';';
	pcolor[4] = color.g[0];
	pcolor[5] = color.g[1];
	pcolor[6] = color.g[2];
	pcolor[7] = ';';
	pcolor[8] = color.b[0];
	pcolor[9] = color.b[1];
	pcolor[10] = color.b[2];
	endFormat = 'm';
	character[0] = ' ';
	character[1] = ' ';
}

Pixel::Pixel(Byte rIn, Byte gIn, Byte bIn)
{
	Pixel(Color(rIn, gIn, bIn));
}

Pixel::Pixel()
{
	Pixel(0, 0, 0);
}

void Pixel::Set(Color col)
{
	CColor color = CColor(col);
	pcolor[0] = color.r[0];
	pcolor[1] = color.r[1];
	pcolor[2] = color.r[2];
	pcolor[4] = color.g[0];
	pcolor[5] = color.g[1];
	pcolor[6] = color.g[2];
	pcolor[8] = color.b[0];
	pcolor[9] = color.b[1];
	pcolor[10] = color.b[2];
}

void Pixel::Set(Byte redd, Byte gren, Byte bluu)
{
	Set(Color(redd, gren, bluu));
}

void Pixel::Set(Byte grayScale)
{
	Set(Color(grayScale));
}

void Pixel::Init() {
	ESC[0] = '\x1b';
	ESC[1] = '[';
	ESC[2] = '4';
	ESC[3] = '8';
	ESC[4] = ';';
	ESC[5] = '2';
	ESC[6] = ';';
	Set(Color(0, 0, 0));
	endFormat = 'm';
	character[0] = ' ';
	character[1] = ' ';
}

void Pixel::EndRow() {
	character[0] = '\r';
	character[1] = '\n';
}

void Pixel::EndRow(bool isReturn) {
	if (isReturn)
	{
		character[0] = '\r';
		character[1] = '\n';
	}
	else
	{
		character[0] = ' ';
		character[1] = ' ';
	}
}

void Pixel::CutoffPix()
{
	for (int i = 0; i < 7; ++i)
	{
		ESC[i] = '\0';
	}
}

/*
  _____    ______   _   _   _____    ______   _____
 |  __ \  |  ____| | \ | | |  __ \  |  ____| |  __ \
 | |__) | | |__    |  \| | | |  | | | |__    | |__) |
 |  _  /  |  __|   | . ` | | |  | | |  __|   |  _  /
 | | \ \  | |____  | |\  | | |__| | | |____  | | \ \
 |_|  \_\ |______| |_| \_| |_____/  |______| |_|  \_\
 */

Rect FrameBuffer::g_framespace = Rect(g_frameWidth, g_frameHeight); // Framespace visibile area
Rect FrameBuffer::g_framespaceExt = Rect(g_frameExtWidth, g_frameHeight); // Framespace usible area

void FrameBuffer::CutRow(int row)
{
	// We are removing the input row and replacing it with a copy of the row above.
	Coord start = Coord(0, row - 1);
	Coord end = Coord(0,row);
	for (int x = 0; x < g_frameWidth; ++x)
	{
		ExchangePixel(start, end);
	}
}

void FrameBuffer::DropRow(int rowToDrop)
{
	for (int row = rowToDrop; row >= 0; --row)
	{
		CutRow(row);
	}
}

void FrameBuffer::Draw()
{
	std::cout << "\x1b[0;0H" << "\x1b[?;25;l"; // Initialize the position of the cursor, then hide it
	std::cout << reinterpret_cast<const char*>(m_frameBuffer); // Print the frame buffer
	std::cout << "\x1b[49;0m"; // Reset the color
}

FrameBuffer::FrameBuffer()
{
	for (int i = 0; i < g_frameTotalSize; ++i)
	{
		m_frameBuffer[i].Init();
		if (i % g_frameExtWidth == g_frameWidth)
			m_frameBuffer[i].EndRow();
	}
	m_frameBuffer[g_frameTotalSize].CutoffPix();
}

Pixel FrameBuffer::GetPixel(int index)
{
	return m_frameBuffer[index];
}

Pixel FrameBuffer::GetPixel(int x, int y)
{
	return GetPixel(Index(x,y));
}

Pixel FrameBuffer::GetPixel(Coord position)
{
	return GetPixel(position.x, position.y);
}

void FrameBuffer::SetPixel(int index, Pixel pixel)
{
	if (index <= g_frameTotalSize)
	{
		m_frameBuffer[index] = pixel;
	}
}

void FrameBuffer::SetPixel(int x, int y, Pixel pixel)
{
	if (Valid(x,y))
		SetPixel(Index(x, y), pixel);
}

void FrameBuffer::SetPixel(Coord position, Pixel pixel)
{
	SetPixel(position.x, position.y, pixel);
}

void FrameBuffer::ExchangePixel(int startIndex, int endIndex)
{
	SetPixel(startIndex, GetPixel(endIndex));
}

void FrameBuffer::ExchangePixel(int startX, int startY, int endX, int endY)
{
	ExchangePixel(Index(startX, startY), Index(endX, endY));
}

void FrameBuffer::ExchangePixel(Coord start, Coord end)
{
	ExchangePixel(start.x, start.y, end.x, end.y);
}

int FrameBuffer::Index(int x, int y)
{
	return Index(Coord(x, y));
}

int FrameBuffer::Index(Coord position)
{
	return position.Index(g_frameExtWidth);
}

bool FrameBuffer::Valid(int x, int y)
{
	return Valid(Coord(x, y));
}

bool FrameBuffer::Valid(Coord position)
{
	return g_framespace.Has(position);
}

void FrameBuffer::SetPixelColor(int index, Color color)
{
	m_frameBuffer[index].Set(color);
}

void FrameBuffer::SetPixelColor(int x, int y, Color color)
{
	if (Valid(x,y))
		SetPixelColor(Index(x, y), color);
}

void FrameBuffer::SetPixelColor(Coord position, Color color)
{
	if (Valid(position))
		SetPixelColor(Index(position), color);
}

FrameBuffer g_frame;

/*
   _____    ____    _        _        _____    _____   _____    ____    _   _
  / ____|  / __ \  | |      | |      |_   _|  / ____| |_   _|  / __ \  | \ | |
 | |      | |  | | | |      | |        | |   | (___     | |   | |  | | |  \| |
 | |      | |  | | | |      | |        | |    \___ \    | |   | |  | | | . ` |
 | |____  | |__| | | |____  | |____   _| |_   ____) |  _| |_  | |__| | | |\  |
  \_____|  \____/  |______| |______| |_____| |_____/  |_____|  \____/  |_| \_|

*/

Coord CollisionBuffer::g_playspaceOffset = Coord(1, 0);
Rect CollisionBuffer::g_playspace = Rect(
	g_playspaceWidth,
	g_playspaceHeight,
	g_playspaceOffset);

int CollisionBuffer::Index(int x, int y)
{
	return Index(Coord(x,y));
}

int CollisionBuffer::Index(Coord position)
{
	return g_playspace.Index(position);
}

bool CollisionBuffer::Valid(int x, int y)
{
	return Valid(Coord(x,y));
}

bool CollisionBuffer::Valid(Coord position)
{
	return g_playspace.Has(position);
}

Color::Color(Byte rIn, Byte gIn, Byte bIn)
{
	r = rIn;
	g = gIn;
	b = bIn;
}

Color::Color(Byte value)
{
	r = value;
	g = value;
	b = value;
}

// Creates a color with value black

Color::Color()
{
	r = 0;
	g = 0;
	b = 0;
}

CColor::CColor(Color color)
{
	r[0] = color.r / 100;
	g[0] = color.g / 100;
	b[0] = color.b / 100;
	r[0] = ((color.r / 10) % 10);
	g[0] = ((color.g / 10) % 10);
	b[0] = ((color.b / 10) % 10);
	r[0] = (color.r % 10);
	g[0] = (color.g % 10);
	b[0] = (color.b % 10);
}

CColor::CColor(FColor color)
{
	CColor(color.ToColor());
}

CColor::CColor()
{
	for (Byte i = 0; i < 3; ++i)
	{
		r[i] = 0;
		g[i] = 0;
		b[i] = 0;
	}
}

Color CColor::ToColor()
{
	Color out;
	out.r = Whole(
		((r[0] - '0') * 100) +
		((r[1] - '0') * 10) +
		(r[2] - '0'));
	out.g = Whole(
		((g[0] - '0') * 100) +
		((g[1] - '0') * 10) +
		(g[2] - '0'));
	out.b = Whole(
		((b[0] - '0') * 100) +
		((b[1] - '0') * 10) +
		(b[2] - '0'));
	return out;
}

char CColor::Comp(Byte i)
{
	char out = '0';
	if (Range(0, 2).Has(i))
		out = r[i];
	if (Range(4, 6).Has(i))
		out = g[i];
	if (Range(8, 10).Has(i))
		out = b[i];
	if (i == 3 || i == 7)
		out = ';';
	return out;
}

FColor::FColor(Color color)
{
	r = float(color.r) / float(255);
	g = float(color.g) / float(255);
	b = float(color.b) / float(255);
}

FColor::FColor(float rIn, float gIn, float bIn)
{
	r = rIn;
	g = gIn;
	b = bIn;
}

FColor::FColor(float value)
{
	r = value;
	g = value;
	b = value;
}

FColor::FColor()
{
	r = 0;
	g = 0;
	b = 0;
}

Color FColor::ToColor()
{
	Color out;
	out.r = Byte(r * 255);
	out.g = Byte(g * 255);
	out.b = Byte(b * 255);
	return out;
}
