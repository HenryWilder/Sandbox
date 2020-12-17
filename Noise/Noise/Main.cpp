#include <iostream>
#include "Rendering.h"
#include "NoiseGenerators.h"

using namespace std;

void DrawNoiseNodes(NoiseSpace2& data)
{
    for (Whole i = 0; i < data.m_size; ++i)
    {
        AbstrPos node = data.m_node[i];
        g_frameBuffer.DrawToBuffer({ (Whole)node.x,(Whole)node.y }, { 255,0,0 });
    }
}

GrayscaleOutput NoiseToRender(int in)
{
    switch (in)
    {
    default:
        return &Whirly;
        break;
    case 1:
        return &Voronoi;
        break;
    case 2:
        return &Cell;
        break;
    }
}

int main()
{
    GrayscaleOutput mat_Noise;

    mat_Noise = NoiseToRender(1);

    for (unsigned int i = 0; i < 100; ++i)
    {
        NoiseSpace2 noiseData = NoiseSpace2(100U, i);
        g_frameBuffer.ApplyGrayscaleShader(mat_Noise, &noiseData);
        g_frameBuffer.Draw();
    }

    ReleaseDC(window, hdc);
    cin.ignore();

    return 0;
}