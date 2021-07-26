#include <raylib.h>
#include "Display.h"
#include "Items.h"

DisplayData g_itemsDisplay[items_count] =
{
    DisplayData("iron_ore.png",         "iron_ore.obj"          ),
    DisplayData("iron.png",             "iron.obj"              ),
    DisplayData("iron_rod.png",         "iron.obj"              ),
    DisplayData("iron_plate.png",       "iron_plate.obj"        ),
    DisplayData("scres.png",            "screws.obj"            ),
    DisplayData("reinforced_plate.png", "reinforced_plate.obj"  ),
};

const ItemBlueprint g_itemBPs[items_count] =
{
    ItemBlueprint{ "Iron ore",           100u, slot_none, g_itemsDisplay + 0 },
    ItemBlueprint{ "Iron",               100u, slot_none, g_itemsDisplay + 1 },
    ItemBlueprint{ "Iron Rod",           200u, slot_none, g_itemsDisplay + 2 },
    ItemBlueprint{ "Iron Plate",         100u, slot_none, g_itemsDisplay + 3 },
    ItemBlueprint{ "Screws",             240u, slot_none, g_itemsDisplay + 4 },
    ItemBlueprint{ "Reinforced Plate",   100u, slot_none, g_itemsDisplay + 5 },
};