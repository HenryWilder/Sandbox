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
    NoiseSpace2 noiseData = NoiseSpace2(10U, 0U);

    GrayscaleOutput mat_Noise;

    mat_Noise = NoiseToRender(1);

    g_frameBuffer.ApplyGrayscaleShader(mat_Noise, &noiseData);

    //DrawNoiseNodes(noiseData);

    g_frameBuffer.Draw();

    ReleaseDC(window, context);
    cin.ignore();

    return 0;
}