#include "Items.h"
#include "Fluids.h"
#include "Buildings.h"
#include "Crafting.h"

Ingredient::Ingredient(ItemID item, unsigned short count) :
    type(type_item), i({ item, count }) {}

Ingredient::Ingredient(FluidID fluid, float amount) :
    type(type_fluid), f({ fluid, amount }) {}

Recipe::Recipe(const char* _name, float _time, unsigned short _i_size, Ingredient(&& _i)[4], unsigned short _o_size, Ingredient(&& _o)[2]) :
    name(_name),
    time(_time),
    i_size(_i_size),
    o_size(_o_size),
    i{ (_i_size > 0 ? _i[0] : Ingredient()), (_i_size > 1 ? _i[1] : Ingredient()), (_i_size > 2 ? _i[2] : Ingredient()), (_i_size > 3 ? _i[3] : Ingredient()) },
    o{ (_o_size > 0 ? _o[0] : Ingredient()), (_o_size > 1 ? _o[1] : Ingredient()) },
{}

const Recipe g_recipes[recipes_count] =
{
    Recipe(
        "Iron Ore", 0.0f,
        {},
        {
            Ingredient(item_iron_ore, 1u),
        }
    ),
    Recipe(
        "Iron", 0.0f,
        {
            Ingredient(item_iron_ore, 1u),
        },
        {
            Ingredient(item_iron, 1u),
        }
    ),
    Recipe{
        "Iron Rod", 0.0f,
        { 
            Ingredient(item_iron, 1u),
        },
        {
            Ingredient(item_iron_rod, 1u),
        }
    },
    Recipe(
        "Iron Plate", 0.0f,
        {
            Ingredient(item_iron, 2u),
        },
        { 
            Ingredient(item_iron_plate, 3u),
        }
    ),
    Recipe(
        "Screws", 0.0f,
        {
            Ingredient(item_iron_rod, 1u),
        },
        {
            Ingredient(item_screw, 1u),
        }
    ),
};