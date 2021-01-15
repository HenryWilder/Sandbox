
#include <raylib.h>

int main(void)
{
    const int WindowWidth = 1280;
    const int WindowHeight = 720;

    InitWindow(WindowWidth, WindowHeight, "RaylibApp");

    Shader simShader = LoadShader(NULL, "sim.frag");
    Shader initShader = LoadShader(NULL, "initializer.frag");
    Shader postProcShader = LoadShader(NULL, "post_process.frag");

    Image inImg = LoadImage("start_conditions.PNG");

    Texture2D startingCond = LoadTextureFromImage(inImg);

    UnloadImage(inImg);

    RenderTexture2D input = LoadRenderTexture(WindowWidth, WindowHeight);
    RenderTexture2D output = LoadRenderTexture(WindowWidth, WindowHeight);

    BeginTextureMode(input);

        BeginShaderMode(initShader);

            DrawTexture(startingCond, 0, 0, WHITE);

        EndShaderMode();
    
    EndTextureMode();

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // Simulate the frame
            BeginTextureMode(output);

                BeginShaderMode(simShader);

                    DrawTextureRec(input.texture, Rectangle{ 0.f, 0.f, (float)input.texture.width, -1.0f * (float)input.texture.height }, Vector2{ 0.f, 0.f }, WHITE);

                EndShaderMode();

            EndTextureMode();

            // Draw this frame's output to the next frame's input
            BeginTextureMode(input);

                DrawTextureRec(output.texture, Rectangle{ 0.f, 0.f, (float)output.texture.width, -1.0f * (float)output.texture.height }, Vector2{ 0.f, 0.f }, WHITE);

            EndTextureMode();

            // Post processing
            BeginShaderMode(postProcShader);

                DrawTextureRec(output.texture, Rectangle{ 0.f, 0.f, (float)output.texture.width, -1.0f * (float)output.texture.height }, Vector2{ 0.f, 0.f }, WHITE);

            EndShaderMode();

        EndDrawing();
    }

    UnloadShader(simShader);
    UnloadShader(initShader);
    UnloadShader(postProcShader);

    UnloadRenderTexture(input);
    UnloadRenderTexture(output);

    UnloadTexture(startingCond);

    return 0;
}


