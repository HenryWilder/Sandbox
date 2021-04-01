#include <string>
#include <vector>
#include <set>
#include <map>
#include <array>
#include <queue>
#include <stack>
#include <limits>
#include <stdio.h>
#include <raylib.h>

struct Type {
	int a;
	float b;
	double c;
	bool d;
};

enum class MemoryImpact { Normal, StoresOnce, DoesntStore };
const struct { const std::vector<std::string> m_symbols; std::vector<MemoryImpact> m_impact; } g_keywords{
	{
		"struct ",
		"class ",
		"const ",
		"static ",
		"constexpr ",
	},
	{
		MemoryImpact::Normal,
		MemoryImpact::Normal,
		MemoryImpact::Normal,
		MemoryImpact::StoresOnce,
		MemoryImpact::DoesntStore,
	}
};

struct Primitive
{
	enum class Prefix
	{
		None,
		Signed,
		Unsigned,
	};
	enum class Type
	{
		Char = sizeof(char),
		Short = sizeof(short),
		Int = sizeof(int),
		Long = sizeof(long),
		LongLong = sizeof(long long),
	};
	enum class Usage
	{
		None,
		Int,
	};
	enum class Suffix
	{
		None,
		Ptr,
		Ref,
	};

	Prefix m_pre = Prefix::None;
	Type m_parts = Type::Int;
	Usage m_use = Usage::None;
	Suffix m_post = Suffix::None;
};

const struct { const std::vector<std::string> m_symbols; const std::vector<size_t> m_sizes; } g_types{
	// Symbols
	{
		"string ",

		"vector<bool> ",
		"vector ",
		"map ",
		"set ",
		"array ",

		"float ",
		"long double ",
		"double ",

		"unsigned long long int ",
		"signed long long int ",
		"unsigned long long ",
		"unsigned long int ",
		"signed long long ",
		"signed long int ",
		"unsigned long ",
		"signed long ",
		"long long int ",
		"long int ",
		"long long ",
		"long ",

		"unsigned char ",
		"signed char ",
		"char ",
		"bool ",
		"unsigned short int ",
		"signed short int ",
		"unsigned short ",
		"signed short ",
		"short int ",
		"short ",
		"unsigned int ",
		"signed int ",
		"int ",
	},
	// Sizes
	{
		sizeof(std::string),

		sizeof(std::vector<bool>),
		sizeof(std::vector<Type>),
		sizeof(std::map<Type, Type>),
		sizeof(std::set<Type>),
		sizeof(std::array<Type, 1>),

		sizeof(float),
		sizeof(long double),
		sizeof(double),

		sizeof(unsigned long long int),
		sizeof(signed long long int),
		sizeof(unsigned long long),
		sizeof(unsigned long int),
		sizeof(signed long long),
		sizeof(signed long int),
		sizeof(unsigned long),
		sizeof(signed long),
		sizeof(long long int),
		sizeof(long int),
		sizeof(long long),
		sizeof(long),

		sizeof(unsigned char),
		sizeof(signed char),
		sizeof(char),
		sizeof(bool),
		sizeof(unsigned short int),
		sizeof(signed short int),
		sizeof(unsigned short),
		sizeof(signed short),
		sizeof(short int),
		sizeof(short),
		sizeof(unsigned int),
		sizeof(signed int),
		sizeof(int),
	}
};

const std::string keywordList = "unsigned long long int ";

int CeilSnap(int val, int grid)
{
	int _val = (val / grid);
	if ((_val * grid) != val) ++_val;
	return _val * grid;
}

struct Int2 { int x, y; };
Int2& operator+=(Int2& a, const Int2& b) { a.x += b.x; a.y += b.y; return a; }

void CharPosInfluence(Int2& pos, char c)
{
	switch (c)
	{
	case '\n':
		pos.y++;
		pos.x = 0;
		break;

	case '\t':
		pos.x = CeilSnap(pos.x, 4);
		break;

	default:
		pos.x++;
		break;
	}
}
Int2 GetCoordOfChar(const char* context, int cursor, int line)
{
	int i = 0;
	Int2 pt = { cursor, line };

	return pt;
}

int PrevLine(const char* context, int cursor)
{
	return cursor;
}
int NextLine(const char* context, int cursor)
{
	return cursor;
}

void PrintPos(int cursor, int x, int y)
{
	DrawText(TextFormat("( %i )", cursor), x, y, 8, MAGENTA);
}
void PrintPos(Int2 pt, int x, int y)
{
	DrawText(TextFormat("{ %i, %i }", pt.x, pt.y), x, y, 8, MAGENTA);
}

int Pack(const bool (&buff)[4])
{
	int out = 0;
	for (int i = 0; i < 4; ++i)
	{
		out |= (buff[i] << i);
	}
	return out;
}

enum DirectionKey {
	DIRKEY_RIGHT,
	DIRKEY_LEFT,
	DIRKEY_DOWN,
	DIRKEY_UP,

	DIRKEY_ALL,
	DIRKEY_ANY,
};
struct DirKeys
{
	bool down[4];

	bool operator[](DirectionKey key) const
	{
		if (key == DIRKEY_ALL || key == DIRKEY_ANY) return Pack(down);
		else {
			switch (key)
			{
			case DIRKEY_RIGHT:	return down[0];
			case DIRKEY_LEFT:	return down[1];
			case DIRKEY_DOWN:	return down[2];
			case DIRKEY_UP:		return down[3];
			default: return false;
			}
		}
	}
	bool operator[](KeyboardKey key)
	{
		switch (key)
		{
		case KEY_RIGHT:	return down[0];
		case KEY_LEFT:	return down[1];
		case KEY_DOWN:	return down[2];
		case KEY_UP:	return down[3];
		default: return false;
		}
	}

	bool Update(bool hold = true) // Returns true if there was a change
	{
		bool temp[4] = { down[0], down[1], down[2], down[3] };

		down[0] = (IsKeyPressed(KEY_RIGHT) || (IsKeyDown(KEY_RIGHT) && !hold));
		down[1] = (IsKeyPressed(KEY_LEFT)  || (IsKeyDown(KEY_LEFT)  && !hold));
		down[2] = (IsKeyPressed(KEY_DOWN)  || (IsKeyDown(KEY_DOWN)  && !hold));
		down[3] = (IsKeyPressed(KEY_UP)    || (IsKeyDown(KEY_UP)    && !hold));

		return (Pack(temp) != Pack(down));
	}
};

void ConstructCode(std::string& output, std::vector<std::string>& combine)
{
	output = "";
	for (auto& str : combine)
	{
		output += (str + "\n");
	}
}

size_t MinSize(size_t size, size_t align) {
	if (!size || !align) return 0;
	size_t round = ((size / align) * align);
	if (round == size) return size;
	else return round + align;
}

Font font;

// A string with formatting
struct Text
{
	Text() : m_color(WHITE), m_str("") {};
	Text(const std::string& str) : m_color(WHITE), m_str(str) {};
	Text(const std::string& str, Color color) : m_color(color), m_str(str) {};

	Color m_color;
	std::string m_str;
};

void PrintText(const Font& font, const std::vector<Text>& text)
{
	Vector2 position = { 0,0 };
	for (Text chunk : text)
	{
		DrawTextEx(font, chunk.m_str.c_str(), position, 32.0f, 2.0f, chunk.m_color);
	}
}

void Format(std::vector<std::string> code, std::vector<Text>& output)
{
	output.clear();

	// Pre-format with indentation
	{
		int depth = 0;
		for (std::string& line : code)
		{
			{
				depth += ((int)(line.find("{") != line.npos) - (int)(line.find("}") != line.npos));
			}

			{
				size_t start = (line.find_first_not_of(" \t"));
				if (start != line.npos)
				{
					auto it = line.begin();
					line.erase(it, it + start);
				}
			}

			{
				int depthCpy = depth;
				while (depthCpy--)
				{
					line.insert(0, "    ");
				}
			}
		}
	}
	// Locate keywords n stuff
	{
		for (std::string& line : code)
		{
			const size_t end = line.find_last_of(";");
			size_t offset = 0Ui64;
			size_t stop;
			while ((stop = line.find(";", offset)) != end)
			{
				line.find_first_of("int", offset);
				offset = stop; // Update position
			}
		}
	}
}

constexpr int c_holdTimeLength = 15;

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;

	InitWindow(windowWidth, windowHeight, "Struct Optimizer");
	SetTargetFPS(60);

	font = LoadFont("mono_font.png");

	int toggle = 0;
	int cursorTarget = 4; // Store for when changing lines
	int cursor = 4;
	int line = 2;
	int holdCountdown = c_holdTimeLength;
	Int2 charDim = { 16, 32 };
	bool b_inputChanged = true;
	bool b_dirty = true;
	DirKeys dirKeyBuffer;

	std::vector<std::string> code{ {"struct MyType"}, {"{"}, {"    "}, {"};"} };
	std::vector<Text> display{};
	std::string conglom; // Conglomerate

	// End of line
	auto EOL = [&code, &line] { return code[line].size(); };
	// "Start of line" (first non-whitespace character)
	auto SOL = [&code, &line, &EOL] {
		int start = (code[line].find_first_not_of(" \t"));
		if (start < 0) return EOL();
		else return (size_t)start;
	};
	// End of document
	auto EOD = [&code] { return (code.size() - 1); };

	while (!WindowShouldClose())
	{
		bool b_dKeyChange = dirKeyBuffer.Update();

		if (dirKeyBuffer[DIRKEY_ANY])
		{
			if (dirKeyBuffer[KEY_RIGHT]) {
				if (cursor < EOL()) cursor++;
				else if (line < EOD()) {
					line++;
					cursor = 0;
				}
			}
			if (dirKeyBuffer[KEY_LEFT]) {
				if (cursor > 0) cursor--;
				else if (line > 0) {
					line--;
					cursor = EOL();
				}
			}
			if (dirKeyBuffer[KEY_DOWN] && line < EOD() || dirKeyBuffer[KEY_UP] && line > 0) {
				if (dirKeyBuffer[KEY_DOWN] && line < EOD())	line++;
				if (dirKeyBuffer[KEY_UP] && line > 0)		line--;
				if (cursor > EOL()) cursor = EOL();
				if (cursor < SOL()) cursor = SOL();
			}
			toggle = 0; // Make the cursor blink immediately
		}

		if (IsKeyPressed(KEY_BACKSPACE)) {
			if (!(cursor == 0 && line == 0)) {
				if (cursor == 0 && line > 0) {
					--line;
					cursor = EOL();
					code[line] += code[line + 1];
					code.erase(code.begin() + line + 1);
				}
				else {
					code[line].erase(--cursor, 1);
				}
				b_dirty = true;
			}
		}
		if (IsKeyPressed(KEY_DELETE)) {
			if (!(cursor == EOL() && line == EOD())) {
				if (cursor == EOL() && line < EOD()) {
					code[line] += code[line + 1];
					code.erase(code.begin() + line + 1);
				}
				else {
					code[line].erase(cursor, 1);
				}
				b_dirty = true;
			}
		}
		if (IsKeyPressed(KEY_ENTER)) {
			if (cursor == EOL()) { // At end
				code.insert(code.begin() + line + 1, { "" });
			}
			else if (cursor == 0) { // At start
				code.insert(code.begin() + line, { "" });
			}
			else {
				code.insert(code.begin() + line + 1, { code[line].substr(cursor) });
				code[line].erase(cursor);
			}
			size_t indent = SOL();
			++line;
			for (size_t i = 0; i < indent; ++i) {
				code[line] += " ";
			}
			cursor = SOL();
		}

		int charPressed = GetCharPressed();
		if (charPressed)
		{
			char insert[2] = "A";
			insert[0] = (char)charPressed;
			code[line].insert(cursor, insert);
			if (cursor < code[line].size()) cursor++;
		}

		Int2 pos = { cursor, line };
		PrintPos(pos, 256, 0);
		pos.x *= (charDim.x + 2);
		pos.y *= (charDim.y + 16);

		if (toggle < 30) DrawRectangle(pos.x, pos.y, 3, charDim.y, ColorAlpha(WHITE, 0.5));

		BeginDrawing(); {

			ClearBackground(BLACK);
			{
				if (b_dirty)
				{
					Format(code);
					ConstructCode(conglom, code);

					size_t align = 0;
					size_t paddedBytes = 0;
					size_t bytes = 0;

					// Search
					{
						const auto end = conglom.find(";");
						auto searchEnd = conglom.find(";");
						while (searchEnd != end)
						{

							for (auto& lineStr : code) {
								for (auto& pair : symbols) {
									size_t pos = lineStr.find(pair.first, 0);
									if (pos != std::string::npos) {
										bytes += pair.second;
										if (pair.second > align) align = pair.second;
										break;
									}
								}
							}
						}
					}

					conglom += TextFormat("// %zu bytes", MinSize(bytes, align));
					b_dirty = false;
				}

				PrintText(font, display);
			}
			DrawFPS(0,0);

		} EndDrawing();
		toggle = (toggle + 1) % 60;
	}
	UnloadFont(font);

	CloseWindow();

	return 0;
}