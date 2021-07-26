#pragma once

struct Texture;
typedef Texture Texture2D;
struct Model;

struct DisplayData
{
    DisplayData(const char*, const char*);

    const char* iconFileName;
    const char* modelFileName;
    Texture2D* icon;
    Model* model;
};
void LoadDisplayIcon(DisplayData& disp);
void UnloadDisplayIcon(DisplayData& disp);
void LoadDisplayModel(DisplayData& disp);
void UnloadDisplayModel(DisplayData& disp);