#pragma once

enum IngredientType;
struct DisplayData;

struct BuildingBlueprint
{
    BuildingBlueprint(const char* _name, unsigned short _i_size, IngredientType (&&_i)[4], unsigned short _o_size, IngredientType (&&_o)[2], DisplayData* _disp);

    const char* name;
    unsigned short i_size, o_size;
    IngredientType i[4], o[2];
    DisplayData* disp;
};
enum BuildingID
{
    building_miner_1,
    building_miner_2,
    building_smelter,
    building_foundry,

    buildings_count
};
extern DisplayData g_buildingsDisplay[buildings_count];
extern const BuildingBlueprint g_buildings[buildings_count];
