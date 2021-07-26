#pragma once

struct Color;

struct FluidBlueprint
{
    const char* name;
    float fuel_burning_value = 0.0f;
};
enum FluidID
{
    fluid_water,
    fluid_crude_oil,
    fluid_heavy_oil_residue,
    fluid_fuel,
    fluid_turbofuel,
    fluid_liquid_biofuel,
    fluid_alumina_solution,
    fluid_sulfuric_acid,
    fluid_nitrogen_gas,
    fluid_nitric_acid,

    fluids_count
};
extern const Color g_fluidColors[fluids_count];
extern const FluidBlueprint g_fluidBPs[fluids_count];

const Color& FluidColor(FluidID fluid);
const FluidBlueprint& FluidBP(FluidID fluid);