#include <raylib.h>
#include "Fluids.h"

const Color g_fluidColors[fluids_count] =
{
    BLUE,
    BLACK,
    PURPLE,
    ORANGE,
    RED,
    GREEN,
    WHITE,
    YELLOW,
    GRAY,
    WHITE,
};
const FluidBlueprint g_fluidBPs[fluids_count] =
{
    FluidBlueprint{ "Water",               0.0f },
    FluidBlueprint{ "Crude Oil",          10.0f },
    FluidBlueprint{ "Heavy Oil Residue",   5.0f },
    FluidBlueprint{ "Fuel",              100.0f },
    FluidBlueprint{ "Turbofuel",         200.0f },
    FluidBlueprint{ "Liquid Biofuel",     50.0f },
    FluidBlueprint{ "Alumina Solution",    0.0f },
    FluidBlueprint{ "Sulfuric Acid",       0.0f },
    FluidBlueprint{ "Nitrogen Gas",        0.0f },
    FluidBlueprint{ "Nitric Acid",         0.0f },
};

const Color& FluidColor(FluidID fluid)
{
    return g_fluidColors[fluid];
}
const FluidBlueprint& FluidBP(FluidID fluid)
{
    return g_fluidBPs[fluid];
}