#include "fluimp.hh"

#include "win32.hh"
#include "shader_code.hh"

#include <raylib.h>

void Fluimp::init() noexcept {
    SetTraceLogLevel(LOG_ALL);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT | FLAG_WINDOW_TRANSPARENT | FLAG_MSAA_4X_HINT);

    InitWindow(500, 500, "fluimp");

    init_win32(GetWindowHandle());

    _cover = LoadImage("cover_example.jpg");
    ImageResize(&_cover, 450, 450);
    _mask = LoadImage("cover_mask.png");
    ImageAlphaMask(&_cover, _mask);
    _cover_texture = LoadTextureFromImage(_cover);

    _cover_blurred = ImageCopy(_cover);
    ImageBlurGaussian(&_cover_blurred, 7);
    _cover_blurred_texture = LoadTextureFromImage(_cover_blurred);

    _blur = LoadShaderFromMemory(NULL, blur_shader);
}

void Fluimp::update() noexcept {

}

void Fluimp::render() noexcept {
    ClearBackground(Color{ 0, 0, 0, 0 });

    _textures.file("main_border.png", 1, 1, 498, 498);

    _textures.file("cover_shadow.png", 0, 0, 500, 500);

    DrawTexture(_cover_texture, 25, 25, WHITE);

    _textures.file("cover_border.png", 24, 24, 452, 452);


}

void Fluimp::cleanup() noexcept {
    UnloadTexture(_cover_texture);
    UnloadTexture(_cover_blurred_texture);

    UnloadImage(_cover);
    UnloadImage(_mask);
    UnloadImage(_cover_blurred);

    UnloadShader(_blur);

    CloseWindow();
}
