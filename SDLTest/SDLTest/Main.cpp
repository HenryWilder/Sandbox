#include <iostream>
#include <stdio.h>
#include <SDL.h>

SDL_Surface* g_surface;

struct Vec2
{
    int x, y;

    Uint32* ToSDLPixel()
    {
        // Pointer to the pixel array
        Uint32* pixelPtr = ((Uint32*)g_surface->pixels);

        // Change which pixel in the array we want
        pixelPtr += y;
        pixelPtr += x;

        return pixelPtr;
    }
};

struct Color
{
    Uint8 r, g, b;

    Uint32 ToSDLColor()
    {
        // Pointer to format
        SDL_PixelFormat* format = g_surface->format;

        // Color
        Uint32 colorOut = SDL_MapRGB(format, r, g, b);

        return colorOut;
    }
};

void DrawPixel(Vec2 pixel, Color color)
{
    // The pixel we want
    Uint32* pixPtr = (pixel.ToSDLPixel());

    // Modify the value of that pixel, dereferenced
    *pixPtr = color.ToSDLColor();
}

int main(int argc, char** args)
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        if (!SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer))
        {
            while (true)
            {
                for (int x = 0; x < 800; ++x) {
                    for (int y = 0; y < 600; ++y) {
                        try
                        {
                            if (SDL_RenderDrawPoint(renderer, x, y)) throw 20; //Renders on middle of screen.
                        }
                        catch (int e)
                        {
                            std::cout << "ya fucked up.";
                        }

                        SDL_RenderPresent(renderer);
                    }
                }
            }
        }
    }
    else
    {
        std::cout << "ya fucked up. Big time.";
    }

    /*
    Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    g_surface = SDL_CreateRGBSurface(0, 640, 480, 32, rmask, gmask, bmask, amask);

    SDL_Event event;
    int keypress = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    
    // Window mode MUST include SDL_WINDOW_OPENGL for use with OpenGL.
    SDL_Window* window = SDL_CreateWindow(
        "SDL Test", // Window name
        200, 200, // Position (offset from top-left)
        640, 480, // Size
        SDL_WINDOW_OPENGL // Needed
    | SDL_WINDOW_RESIZABLE); // Flags

    // Create an OpenGL context associated with the window.
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    while (true)
    {
        SDL_PumpEvents();
        // now you can make GL calls.
        //glClearColor(0, 0, 0, 1);
        //glClear(GL_COLOR_BUFFER_BIT);
        DrawPixel({20,20}, {255,255,255});
        SDL_GL_SwapWindow(window);
    }
    //glVertex2d(14,32);
    int input;
    std::cin >> input;

    // Once finished with OpenGL functions, the SDL_GLContext can be deleted.
    SDL_GL_DeleteContext(glcontext);
    */


    SDL_Quit();
    return 0;
}