#include "fluimp.hh"

#include "win32.hh"
#include "shaders.hh"

#include <raylib.h>

void Fluimp::init() noexcept {
    SetTraceLogLevel(LOG_ALL);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT | FLAG_WINDOW_TRANSPARENT);

    InitWindow(500, 500, "fluimp");

    init_win32(GetWindowHandle());

    _tex = LoadTexture("1.jpg");
    _main_border = LoadTexture("main_border.png");
    _cover_border = LoadTexture("cover_border.png");
    
    _fs_corner_radius = LoadShaderFromMemory(NULL, fs_corner_radius);
}

void Fluimp::update() noexcept {

}

void Fluimp::render() noexcept {
    ClearBackground(Color{ 0, 0, 0, 0 });

    DrawTexture(_main_border, 1, 1, WHITE);

    BeginShaderMode(_fs_corner_radius);
    {
        float radiusVal[1] = { 8.0f };
        SetShaderValue(_fs_corner_radius, GetShaderLocation(_fs_corner_radius, "radius"), radiusVal, SHADER_UNIFORM_FLOAT);
        float texSize[2] = { (float)_tex.width, (float)_tex.height };
        SetShaderValue(_fs_corner_radius, GetShaderLocation(_fs_corner_radius, "texSize"), texSize, SHADER_UNIFORM_VEC2);
        DrawTexturePro(_tex, Rectangle{ 0, 0, (float)_tex.width, (float)_tex.height}, Rectangle{ 25, 25, 450, 450 }, Vector2{ 0, 0 }, 0.0, WHITE);
    }
    EndShaderMode();

    DrawTexture(_cover_border, 25, 25, WHITE);
}

void Fluimp::cleanup() noexcept {
    UnloadTexture(_tex);
    UnloadTexture(_main_border);

    UnloadShader(_fs_corner_radius);

    CloseWindow();

}
