#pragma once

enum ItemID;
enum FluidID;

struct ItemIngredient
{
    ItemID item;
    unsigned short count;
};

struct FluidIngredient
{
    FluidID fluid;
    float amount;
};

enum IngredientType
{
    type_item,
    type_fluid
};
struct Ingredient
{
    Ingredient() = default;
    Ingredient(ItemID item, unsigned short count);
    Ingredient(FluidID fluid, float amount);

    IngredientType type;
    union
    {
        ItemIngredient i;
        FluidIngredient f;
    };
};

struct Recipe
{
    Recipe() = default;

    Recipe(const char* _name, float _time, unsigned short _i_size, Ingredient(&& _i)[4], unsigned short _o_size, Ingredient(&& _o)[2]);

    const char* name;
    float time;
    unsigned short i_size, o_size;
    Ingredient i[4], o[2];
};
enum RecipeID
{
    recipe_iron_ore,
    recipe_iron,
    recipe_iron_rod,
    recipe_iron_plate,
    recipe_screw,

    recipes_count
};
extern const Recipe g_recipes[recipes_count];