#include <string>
#include <map>
#include <raylib.h>

// Symbol name, size
std::map<std::string, int> symbols = {
	{ "char", 1 },
};

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
Int2 GetCoordOfChar(const char* context, int cursor)
{
	int i = 0;
	Int2 pt = { 0,0 };
	while (context[i] && (i < cursor))
	{
		char c = context[i];
		CharPosInfluence(pt, c);
		++i;
	}

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
		else return down[key];
	}
	bool& operator[](KeyboardKey key)
	{
		return down[key - KEY_RIGHT];
	}

	bool Update(bool hold = true) // Returns true if there was a change
	{
		bool temp[4] = { down[0], down[1], down[2], down[3] };

		for (KeyboardKey k = KEY_RIGHT; k < KEY_UP; k = (KeyboardKey)(k + 1))
		{
			(*this)[k] = (IsKeyPressed(k) || (IsKeyDown(k) && !hold));
		}

		return (Pack(temp) != Pack(down));
	}
};

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;

	InitWindow(windowWidth, windowHeight, "Struct Optimizer");
	SetTargetFPS(30);

	Font font = LoadFont("mono_font.png");

	std::string input = "";

	auto ConstructCode = [&]() {
		return TextFormat(
R"(struct MyType
{
    %s
};)", input.c_str());
	};

	int toggle = 0;
	int cursor = 0;
	Int2 charDim = { 16, 32 };
	bool b_inputChanged = true;
	constexpr int c_holdTimeLength = 15;
	int holdCountdown = c_holdTimeLength;

	std::string code = "";
	
	DirKeys dirKeyBuffer;

	while (!WindowShouldClose())
	{
		bool b_dKeyChange = dirKeyBuffer.Update();

		if ((dirKeyBuffer[KEY_LEFT] && cursor > 0) || (dirKeyBuffer[KEY_RIGHT] && cursor < (code.size() - 1)))
		{
			if (dirKeyBuffer[KEY_RIGHT]) cursor++;
			if (dirKeyBuffer[KEY_LEFT]) cursor--;
			toggle = 0; // Make the cursor blink immediately
		}

		int charPressed = GetCharPressed();
		if (charPressed)
		{
			char insert[] = "A";
			insert[0] = (char)charPressed;
			TextAppend(code.data(), insert, &cursor);
			if (cursor < code.size()) cursor++;
		}

		Int2 pos = GetCoordOfChar(code.c_str(), cursor);
		pos.x *= (charDim.x + 2);
		pos.y *= (charDim.y + 16);
		//PrintPos(pos, 256, 0);

		if (b_inputChanged)
		{
			code = ConstructCode();
			b_inputChanged = false;
		}

		if (toggle < 15) DrawRectangle(pos.x, pos.y, charDim.x, charDim.y, ColorAlpha(WHITE, 0.5));
		
		BeginDrawing(); {

			ClearBackground(BLACK);
			DrawTextEx(font, code.c_str(), { 0,0 }, 32.0f, 2, WHITE);

		} EndDrawing();

		toggle = (toggle + 1) % 30;
	}

	CloseWindow();

	return 0;
}