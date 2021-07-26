#include <raylib.h>
#include "Display.h"

DisplayData::DisplayData(const char* texFile, const char* modFile) :
    iconFileName(texFile), modelFileName(modFile), icon(nullptr), model(nullptr) {}

void LoadDisplayIcon(DisplayData& disp)
{
    if (!disp.icon)
        disp.icon = new Texture2D(LoadTexture(disp.iconFileName));
}

void UnloadDisplayIcon(DisplayData& disp)
{
    if (disp.icon)
    {
        UnloadTexture(*disp.icon);
        delete disp.icon;
        disp.icon = nullptr;
    }
}

void LoadDisplayModel(DisplayData& disp)
{
    if (!disp.model)
        disp.model = new Model(LoadModel(disp.modelFileName));
}

void UnloadDisplayModel(DisplayData& disp)
{
    if (disp.model)
    {
        UnloadModel(*disp.model);
        delete disp.model;
        disp.model = nullptr;
    }
}