#pragma once

struct DisplayData;

enum SlotType
{
    slot_none,
    slot_wearable,
    slot_utility,
};
struct ItemBlueprint
{
    const char* name;
    unsigned short maxStack;
    SlotType fittingSlot;
    DisplayData* disp;
};

// Indices to the global item table
enum ItemID
{
    item_iron_ore,
    item_iron,
    item_iron_rod,
    item_iron_plate,
    item_screw,
    item_reinforced_plate,

    items_count
};
extern DisplayData g_itemsDisplay[items_count];
extern const ItemBlueprint g_itemBPs[items_count];

struct ItemSlot
{
    ItemID item;
    unsigned short count;
};