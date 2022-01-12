#include <iostream>
#include <array>
#include <string>
#include <unordered_map>

typedef std::string ItemID_t;
#define ITEM_ID "%s"

ItemID_t g_items[] =
{
	"air",
	"wood",
	"grass",
	"stone",
	"cobble_stone",
	"coal",

	"pick_wood",
	"pick_stone",

	"unknown",
};

struct CraftResult
{
	ItemID_t item;
	unsigned int count;
};

std::unordered_map<std::string, CraftResult> g_recipes_shaped_3x3 =
{
	{
		std::string{
			"wood" "wood" "wood"
			"air"  "wood" "air" 
			"air"  "wood" "air" 
		},
		CraftResult{ "pick_wood", 1u }
	}
};
std::unordered_map<std::string, CraftResult> g_recipes_shaped_2x2 =
{

};
std::unordered_map<std::string, CraftResult> g_recipes_shapeless =
{

};

CraftResult Craft(const std::vector<std::string>& recipe)
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

int main()
{
	std::vector<std::string> items;
	std::string str;
	for (int i = 0; i < 9; ++i)
	{
		std::cin >> str;
		items.push_back(str);
	}
	CraftResult result = Craft(items);
	printf("\n" ITEM_ID " - %u\n", result.item.c_str(), result.count);

	return 0;
}