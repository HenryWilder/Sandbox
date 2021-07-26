#include <raylib.h>
#include <raymath.h>
#include <string>
#include <unordered_map>

#define sign(x) (((x) > (decltype(x))(0)) - ((x) < (decltype(x))(0)))
#define UNPACK_REC(rec) (rec.x), (rec.y), (rec.width), (rec.height)
#define UNPACK_REC_F(FUN,rec) FUN(rec.x), FUN(rec.y), FUN(rec.width), FUN(rec.height)

using Vector1 = float;

#pragma region Vector math

inline Vector2  operator- (Vector2  vec)          { return      Vector2Negate(vec);             }
inline Vector2  operator+ (Vector2  a, Vector2 b) { return      Vector2Add(a, b);               }
inline Vector2& operator+=(Vector2& a, Vector2 b) { return (a = Vector2Add(a, b));              }
inline Vector2  operator+ (Vector2  a, float val) { return      Vector2AddValue(a, val);        }
inline Vector2& operator+=(Vector2& a, float val) { return (a = Vector2AddValue(a, val));       }
inline Vector2  operator- (Vector2  a, Vector2 b) { return      Vector2Subtract(a, b);          }
inline Vector2& operator-=(Vector2& a, Vector2 b) { return (a = Vector2Subtract(a, b));         }
inline Vector2  operator- (Vector2  a, float val) { return      Vector2SubtractValue(a, val);   }
inline Vector2& operator-=(Vector2& a, float val) { return (a = Vector2SubtractValue(a, val));  }
inline Vector2  operator* (Vector2  a, Vector2 b) { return      Vector2Multiply(a, b);          }
inline Vector2& operator*=(Vector2& a, Vector2 b) { return (a = Vector2Multiply(a, b));         }
inline Vector2  operator* (Vector2  a, float scl) { return      Vector2Scale(a, scl);           }
inline Vector2& operator*=(Vector2& a, float scl) { return (a = Vector2Scale(a, scl));          }
inline Vector2  operator/ (Vector2  a, Vector2 b) { return      Vector2Divide(a, b);            }
inline Vector2& operator/=(Vector2& a, Vector2 b) { return (a = Vector2Divide(a, b));           }
inline Vector2  operator/ (Vector2  a, float div) { return      Vector2Scale(a, 1.0f / div);    }
inline Vector2& operator/=(Vector2& a, float div) { return (a = Vector2Scale(a, 1.0f / div));   }

inline Vector3  operator- (Vector3  vec)          { return      Vector3Negate(vec);             }
inline Vector3  operator+ (Vector3  a, Vector3 b) { return      Vector3Add(a, b);               }
inline Vector3& operator+=(Vector3& a, Vector3 b) { return (a = Vector3Add(a, b));              }
inline Vector3  operator+ (Vector3  a, float val) { return      Vector3AddValue(a, val);        }
inline Vector3& operator+=(Vector3& a, float val) { return (a = Vector3AddValue(a, val));       }
inline Vector3  operator- (Vector3  a, Vector3 b) { return      Vector3Subtract(a, b);          }
inline Vector3& operator-=(Vector3& a, Vector3 b) { return (a = Vector3Subtract(a, b));         }
inline Vector3  operator- (Vector3  a, float val) { return      Vector3SubtractValue(a, val);   }
inline Vector3& operator-=(Vector3& a, float val) { return (a = Vector3SubtractValue(a, val));  }
inline Vector3  operator* (Vector3  a, Vector3 b) { return      Vector3Multiply(a, b);          }
inline Vector3& operator*=(Vector3& a, Vector3 b) { return (a = Vector3Multiply(a, b));         }
inline Vector3  operator* (Vector3  a, float scl) { return      Vector3Scale(a, scl);           }
inline Vector3& operator*=(Vector3& a, float scl) { return (a = Vector3Scale(a, scl));          }
inline Vector3  operator/ (Vector3  a, Vector3 b) { return      Vector3Divide(a, b);            }
inline Vector3& operator/=(Vector3& a, Vector3 b) { return (a = Vector3Divide(a, b));           }
inline Vector3  operator/ (Vector3  a, float div) { return      Vector3Scale(a, 1.0f / div);    }
inline Vector3& operator/=(Vector3& a, float div) { return (a = Vector3Scale(a, 1.0f / div));   }

#pragma endregion

inline Vector2 RectPosition(Rectangle rec)
{
	return *reinterpret_cast<Vector2*>(&rec);
}

typedef std::string ItemID_t;
constexpr uint32_t g_TextureSize = 16;

Texture2D g_itemTextures;

struct Item
{
	unsigned int texX, texY;
};

void DrawItem(const Item& item, Vector2 position)
{
	Rectangle source{
		(float)(item.texX * g_TextureSize),
		(float)(item.texY * g_TextureSize),
		(float)g_TextureSize,
		(float)g_TextureSize
	};
	DrawTexturePro(g_itemTextures, source, { position.x, position.y, (float)g_TextureSize, (float)g_TextureSize }, { 0,0 }, 0.0f, WHITE);	
}

ItemID_t g_items[] =
{
	"air",
	"wood",
	"wood_planks",
	"stick",
	"dirt",
	"grass",
	"stone",
	"cobble_stone",
	"coal",

	"pick_wood",
	"pick_stone",
	"pick_iron",
	"pick_diamond",
};
const std::unordered_map<std::string, Item> g_itemMap =
{
	{ "wood",			Item{} },
	{ "wood_planks",	Item{} },
	{ "stick",			Item{} },
	{ "dirt",			Item{} },
	{ "grass",			Item{} },
	{ "stone",			Item{} },
	{ "cobble_stone",	Item{} },
	{ "coal",			Item{} },

	{ "pick_wood",		Item{} },
	{ "pick_stone",		Item{} },
	{ "pick_iron",		Item{} },
	{ "pick_diamond",	Item{} },

	{ "unknown",		Item{} },
};

struct Slot
{
	ItemID_t item = "air";
	unsigned short count = 0u;
};

std::unordered_map<std::string, Slot> g_recipes_shaped_3x3 =
{
	{
		std::string{
			"wood" "wood" "wood"
			"air"  "wood" "air"
			"air"  "wood" "air"
		},
		Slot{ "pick_wood", 1u }
	}
};
std::unordered_map<std::string, Slot> g_recipes_shaped_2x2 =
{

};
std::unordered_map<std::string, Slot> g_recipes_shapeless =
{

};

Slot Craft(const std::vector<std::string>& recipe)
{
	std::string recipeStr;
	for (const std::string& item : recipe)
	{
		recipeStr += item;
	}
	if (recipe.size() == 9)
	{
		auto it3x3 = g_recipes_shaped_3x3.find(recipeStr);
		if (it3x3 != g_recipes_shaped_3x3.end())
			return it3x3->second;
	}
	if (recipe.size() == 4)
	{
		auto it2x2 = g_recipes_shaped_2x2.find(recipeStr);
		if (it2x2 != g_recipes_shaped_2x2.end())
			return it2x2->second;
	}
	{
		auto itShapless = g_recipes_shapeless.find(recipeStr);
		if (itShapless != g_recipes_shapeless.end())
			return itShapless->second;
	}
	return { "air", 0 };
}


struct UISlot
{
	Rectangle rec;
	Slot* slot;
};

void Draw(const UISlot& uiSlot, Color bkc)
{
	DrawRectangle(UNPACK_REC_F((int), uiSlot.rec), bkc);
	auto it = g_itemMap.find(uiSlot->slot.item);
	if (it != g_itemMap.end())
		DrawItem(it->second, RectPosition(uiSlot.rec));
	DrawRectangleLines(UNPACK_REC_F((int), uiSlot.rec), GRAY);
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

	UISlot slots[3 * 3 + 1];
	for (int y = 0; y < 3; ++y)
	{
		for (int x = 0; x < 3; ++x)
		{
			slots[y * 3 + x] = { { (float)(x * g_TextureSize), (float)(y * g_TextureSize), (float)(g_TextureSize), (float)(g_TextureSize) }, Slot{ "air", 0u } };
		}
	}
	slots[9] = { { (float)(4 * g_TextureSize), (float)(g_TextureSize), (float)(g_TextureSize), (float)(g_TextureSize) }, Slot{ "air", 0u } };

	UISlot* hovered = nullptr;
	Slot* held = nullptr;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/
		Vector2 mousePos = GetMousePosition();
		hovered = nullptr;
		for (UISlot& slot : slots)
		{
			if (CheckCollisionPointRec(mousePos, slot.rec))
			{
				hovered = &slot;
				break;
			}
		}
		if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			held = &hovered->slot;
			hovered->slot = nullptr;
		}

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

			for (const UISlot& slot : slots)
			{
				Draw(slot, LIGHTGRAY);
			}
			if (hovered)
				Draw(*hovered, WHITE);

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}