#include "fluimp.hh"

#include "win32.hh"
#include "shader_code.hh"

#include <raylib.h>

void Fluimp::init() noexcept {
    SetTraceLogLevel(LOG_ALL);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT | FLAG_WINDOW_TRANSPARENT);

    InitWindow(500, 500, "fluimp");

    init_win32(GetWindowHandle());

    _fs_corner_radius = LoadShaderFromMemory(NULL, fs_corner_radius);
}

void Fluimp::update() noexcept {

}

void Fluimp::render() noexcept {
    ClearBackground(Color{ 0, 0, 0, 0 });

    _textures("main_border.png", 1, 1, 498, 498);

    BeginShaderMode(_fs_corner_radius);
    {
        //float radiusVal[1] = { 8.0f };
        //SetShaderValue(_fs_corner_radius, GetShaderLocation(_fs_corner_radius, "radius"), radiusVal, SHADER_UNIFORM_FLOAT);
        //float texSize[2] = { (float)_tex.width, (float)_tex.height };
        //SetShaderValue(_fs_corner_radius, GetShaderLocation(_fs_corner_radius, "texSize"), texSize, SHADER_UNIFORM_VEC2);
        _textures("cover_example.jpg", 25, 25, 450, 450);
    }
    EndShaderMode();

    _textures("cover_border.png", 24, 24, 452, 452);
}

void Fluimp::cleanup() noexcept {
    UnloadShader(_fs_corner_radius);

    CloseWindow();
}
