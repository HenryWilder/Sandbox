#include<iostream>
#include<cmath>
#include<stdio.h>
#include<chrono>
#include<thread>
#include "Windows.h"

using namespace std;

// Get a console handle
HWND window = GetConsoleWindow();
// Get a handle to device context
HDC hdc = GetDC(window);

void Sleep(int x) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(x));
}
void Sleep(double x) {
    std::this_thread::sleep_for(
        std::chrono::microseconds((int)(x * 1000.0)));
}

double Clamp(const double& in, double min = 0.0, double max = 1.0)
{
    double out = in;
}

struct Vec3
{
    double x, y, z;

    Vec3 operator+(const Vec3& p1) const
    {
        Vec3 out;
        out.x = x + p1.x;
        out.y = y + p1.y;
        out.z = z + p1.z;
        return out;
    }
    Vec3 operator-(const Vec3& p1) const
    {
        Vec3 out;
        out.x = x - p1.x;
        out.y = y - p1.y;
        out.z = z - p1.z;
        return out;
    }
    Vec3 operator*(const Vec3& p1) const
    {
        Vec3 out;
        out.x = x * p1.x;
        out.y = y * p1.y;
        out.z = z * p1.z;
        return out;
    }
    Vec3 operator/(const Vec3& p1) const
    {
        Vec3 out;
        out.x = x / p1.x;
        out.y = y / p1.y;
        out.z = z / p1.z;
        return out;
    }

    Vec3 operator*(const double scale) const
    {
        Vec3 out;
        out.x = x * scale;
        out.y = y * scale;
        out.z = z * scale;
        return out;
    }
    Vec3 operator/(const double scale) const
    {
        Vec3 out;
        out.x = x / scale;
        out.y = y / scale;
        out.z = z / scale;
        return out;
    }

    constexpr double Sum() const
    {
        return this->x + this->y + this->z;
    }

    constexpr double Length() const
    {
        return sqrt(((*this) * (*this)).Sum());
    }

    constexpr Vec3 Normal() const
    {
        return *this / Length();
    }
};

constexpr double Dot(const Vec3 a, const Vec3 b)
{
    return (a * b).Sum();
}

Vec3 Direction(const Vec3 a, const Vec3 b)
{
    return (a - b).Normal();
}

Vec3 Reflect(const Vec3 ray, const Vec3 normal)
{
    Vec3 reflection;
    reflection = normal * ray;
    reflection = reflection * 2;
    reflection = reflection * normal;
    reflection = reflection - ray;
    return reflection;
}

struct Color
{
    unsigned char r, g, b;

    // Color math
    Color operator+(const Color color2) const
    {
        Color out;
        out.r = this->r + color2.r;
        out.g = this->g + color2.g;
        out.b = this->b + color2.b;
        return out;
    }
    Color operator-(const Color color2) const
    {
        Color out;
        out.r = this->r - color2.r;
        out.g = this->g - color2.g;
        out.b = this->b - color2.b;
        return out;
    }

    // Scalar math
    Color operator*(double scale) const
    {
        Color out;
        out.r = (unsigned char)((double)(this->r) * scale);
        out.g = (unsigned char)((double)(this->g) * scale);
        out.b = (unsigned char)((double)(this->b) * scale);
        return out;
    }
    Color operator/(double scale) const
    {
        Color out;
        out.r = (unsigned char)((double)(this->r) / scale);
        out.g = (unsigned char)((double)(this->g) / scale);
        out.b = (unsigned char)((double)(this->b) / scale);
        return out;
    }

    // Multi-channel scalar math
    // rValue here is expected to be [0-1]
    Color operator*(const Vec3& scale) const
    {
        Color out;
        out.r = (unsigned char)((((double)(this->r) / 255.0) * scale.x) * 255.0);
        out.g = (unsigned char)((((double)(this->g) / 255.0) * scale.y) * 255.0);
        out.b = (unsigned char)((((double)(this->b) / 255.0) * scale.z) * 255.0);
        return out;
    }
    Color operator/(const Vec3& scale) const
    {
        Color out;
        out.r = (unsigned char)((((double)(this->r) / 255.0) / scale.x) * 255.0);
        out.g = (unsigned char)((((double)(this->g) / 255.0) / scale.y) * 255.0);
        out.b = (unsigned char)((((double)(this->b) / 255.0) / scale.z) * 255.0);
        return out;
    }
};

struct Light
{
    Light()
    {
        m_color = { 255,255,255 };
        m_position = { 0,0,0 };
        m_intensity = 100.0;
        m_specular = { 1,1,1 };
        m_diffuse = { 1,1,1 };
    }
    Light(Color color)
    {
        m_color = color;
        m_intensity = 100.0;
        m_position = { 0,0,0 };
        m_specular = { 1,1,1 };
        m_diffuse = { 1,1,1 };
    }
    Light(Color color, double intensity)
    {
        m_color = color;
        m_position = { 0,0,0 };
        m_intensity = intensity;
        m_specular = { 1,1,1 };
        m_diffuse = { 1,1,1 };
    }
    Light(Color color, double intensity, Vec3 position)
    {
        m_color = color;
        m_position = position;
        m_intensity = intensity;
        m_specular = { 1,1,1 };
        m_diffuse = { 1,1,1 };
    }
    Light(Color color, double intensity, Vec3 position, Vec3 specular, Vec3 diffuse)
    {
        m_color = color;
        m_position = position;
        m_intensity = intensity;
        m_specular = specular;
        m_diffuse = diffuse;
    }

    Color m_color; // The color of the material
    double m_intensity; // Overall strength of the light
    Vec3 m_position; // Position of the light in the scene
    Vec3 m_specular; // Specular intensity [0-1]
    Vec3 m_diffuse; // Diffuse intensity [0-1]
};

struct Material
{
    Color m_color; // The actual color of the material
    double m_shine; // Shininess constant [0-1]
    Vec3 m_specular; // Specular reflection constant [0-1]
    Vec3 m_diffuse; // Diffuse reflection constant [0-1]
    Vec3 m_ambient; // Ambient reflection constant [0-1]
};

int size = alignof(Material);

Light g_ambientLight;

Vec3 g_cameraPos = { 0,0,100 };

Color ToColor(const Vec3 vec)
{
    Color colorOut;
    colorOut.r = (unsigned char)(Clamp(vec.x) * 255.0);
    colorOut.g = (unsigned char)(Clamp(vec.y) * 255.0);
    colorOut.b = (unsigned char)(Clamp(vec.z) * 255.0);
    return colorOut;
}

Vec3 ToVector(const Color col)
{
    Vec3 vecOut;
    vecOut.x = ((double)col.r / 255.0);
    vecOut.y = ((double)col.g / 255.0);
    vecOut.z = ((double)col.b / 255.0);
    return vecOut;
}

// https://en.wikipedia.org/wiki/Phong_reflection_model
Color Phong(Vec3 pos, Vec3 normal, Light* lights, int lightCount, Material& mat)
{
    Vec3 ambient = (ToVector(g_ambientLight.m_color) * g_ambientLight.m_intensity) * mat.m_ambient;

    Vec3 illum = ambient;
    
    for (int m = 0; m < lightCount; ++m)
    {
        Light* thisLight = (lights + m); // This makes it easier to shorthand

        Vec3 diffIntensity = mat.m_diffuse * thisLight->m_diffuse;
        Vec3 specIntensity = mat.m_specular * thisLight->m_specular;

        Vec3 diffuse;
        Vec3 specular;

        Vec3 directToLight = Direction(pos, thisLight->m_position); // L

        diffuse = Dot(directToLight, normal); // Diffuse

        Vec3 perfectReflect = Reflect(directToLight, normal); // R
        Vec3 directToViewer = Direction(pos, g_cameraPos); // V

        specular = Dot(perfectReflect, directToViewer); // Specular

        Vec3 thisLightEffect = ((diffuse * diffIntensity) + (specular * specIntensity));

        illum = illum + thisLightEffect;
    }

    return ToColor(illum);
}

unsigned char Shade(Vec3 pos, Vec3 normal, Vec3 lightPos)
{
    Vec3 toLight = lightPos - pos;

    double dist = toLight.Length();

    double val = Dot(pos.Normal(), toLight.Normal());

    val = (val >= 0 ? val : 0); // no negative values

    //val = reflectivity * intensity * val / (dist * dist);

    return (unsigned char)(val * 255);
}

constexpr int g_width = 64;

void DrawPixel(int x, int y, Color color)
{
    SetPixelV(hdc, x, y, RGB(color.r, color.g, color.b));
}

int main()
{
    double radius = 30; // Sphere radius

    double fpRot = 60; // Frames per rotation

    Light light;
    light.m_position.y = 15;
    light.m_intensity = 170;
    light.m_diffuse;
    light.m_specular;

    int frame = 0;

    printf("\x1b[?25l"); // Hide the cursor

    Material sphereMat;
    sphereMat.m_color = { 10,20,230 };
    sphereMat.m_diffuse = { 0.8,0.8,0.8 };
    sphereMat.m_specular = { 0.8,0.8,0.8 };
    sphereMat.m_shine = 1;

    while (true) {

        Sleep(2);

        ++frame;

        light.m_position.x = 60 * sin((double)frame / fpRot);
        light.m_position.z = 60 * cos((double)frame / fpRot);
        light.m_position.y = 60 * sin((double)frame / fpRot);

        Vec3 pos;

        for (int b = -1 * (g_width/2); b < (g_width/2); ++b) {

            pos.y = (double)b;

            for (int a = -1 * (g_width/2); a < (g_width/2); ++a) {

                pos.x = (double)a;

                Vec3 fromLight = pos - light.m_position;

                // Behind
                if (abs(fromLight.x) < .5 &&
                    abs(fromLight.y) < .5)
                {
                    DrawPixel((int)pos.x + g_width, (int)pos.y + g_width, { 255, 255 });
                }
                
                if (((pos.x * pos.x) + (pos.y * pos.y)) <= (radius * radius))
                {
                    pos.z = sqrt((radius * radius) - ((pos.x * pos.x) + (pos.y * pos.y))); // Don't calculate z till we need it

                    //cout << Value(ival);
                    DrawPixel((int)pos.x + g_width, (int)pos.y + g_width, Phong(pos, pos, &light, 1, sphereMat));
                }
                
                // In front
                if (abs(fromLight.x) < .5 &&
                    abs(fromLight.y) < .5 &&
                    light.m_position.z > pos.z)
                {
                    DrawPixel((int)pos.x + g_width, (int)pos.y + g_width, { 255, 255 });
                }
                
            }
        }
    }
    return 0;
}