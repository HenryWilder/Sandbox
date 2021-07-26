#include "Items.h"
#include "Crafting.h"
#include "Display.h"
#include "Buildings.h"

BuildingBlueprint::BuildingBlueprint(const char* _name, unsigned short _i_size, IngredientType(&& _i)[4], unsigned short _o_size, IngredientType(&& _o)[2], DisplayData* _disp) :
    name(_name),
    i_size(_i_size),
    o_size(_o_size),
    i{ (_i_size > 0 ? _i[0] : IngredientType()), (_i_size > 1 ? _i[1] : IngredientType()), (_i_size > 2 ? _i[2] : IngredientType()), (_i_size > 3 ? _i[3] : IngredientType()) },
    o{ (_o_size > 0 ? _o[0] : IngredientType()), (_o_size > 1 ? _o[1] : IngredientType()) },
    disp(_disp)
{}

DisplayData g_buildingsDisplay[buildings_count] =
{
    DisplayData("miner_mk1.png", "miner_mk1.obj"),
    DisplayData("miner_mk2.png", "miner_mk2.obj"),
    DisplayData("smelter.png",   "smelter.obj"),
    DisplayData("foundry.png",   "foundry.obj"),
};

const BuildingBlueprint g_buildings[buildings_count] =
{
    BuildingBlueprint("Miner Mk.1", 0, {},                       1, { type_item }, g_buildingsDisplay + 0),
    BuildingBlueprint("Miner Mk.2", 0, {},                       1, { type_item }, g_buildingsDisplay + 1),
    BuildingBlueprint("Smelter",    1, { type_item },            1, { type_item }, g_buildingsDisplay + 2),
    BuildingBlueprint("Foundry",    2, { type_item, type_item }, 1, { type_item }, g_buildingsDisplay + 3),
};