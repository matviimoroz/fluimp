#include "fluimp.hh"

#include "win32.hh"
#include "fs_corner_radius.hh"

#include <raylib.h>

void Fluimp::init() noexcept {
    SetTraceLogLevel(LOG_ALL);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT | FLAG_WINDOW_TRANSPARENT);

    InitWindow(500, 500, "fluimp");

    init_win32(GetWindowHandle());

    _cover = LoadImage("1.jpg");
    ImageResize(&_cover, 450, 450);

    _tex = LoadTextureFromImage(_cover);

    UnloadImage(_cover);

    _fs_corner_radius = LoadShaderFromMemory(NULL, fs_corner_radius);

    float texSize[2] = { (float)_tex.width, (float)_tex.height };
    SetShaderValue(_fs_corner_radius, GetShaderLocation(_fs_corner_radius, "texSize"), texSize, SHADER_UNIFORM_VEC2);

    float radiusVal[1] = { 8.0f };
    SetShaderValue(_fs_corner_radius, GetShaderLocation(_fs_corner_radius, "radius"), radiusVal, SHADER_UNIFORM_FLOAT);
}

void Fluimp::update() noexcept {

}

void Fluimp::render() noexcept {
    ClearBackground(Color{ 0, 0, 0, 0 });

    BeginShaderMode(_fs_corner_radius);
    {
        DrawTexture(_tex, 25, 25, WHITE);
    }
    EndShaderMode();
}

void Fluimp::cleanup() noexcept {
    UnloadTexture(_tex);
    UnloadShader(_fs_corner_radius);

    CloseWindow();

}
