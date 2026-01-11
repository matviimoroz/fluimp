#include "fluimp.hh"

#include "win32.hh"
#include "shader_code.hh"

#include <algorithm>
#include <raylib.h>

void Fluimp::init() noexcept {
    SetTraceLogLevel(LOG_ALL);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | /*FLAG_VSYNC_HINT |*/  FLAG_WINDOW_TRANSPARENT | FLAG_MSAA_4X_HINT);

    InitWindow(500, 500, "fluimp");

    init_win32(GetWindowHandle());

    _cover = LoadImage("cover_example.jpg");
    ImageResize(&_cover, 450, 450);
    _mask = LoadImage("cover_mask.png");
    ImageAlphaMask(&_cover, _mask);
    _cover_texture = LoadTextureFromImage(_cover);

    _cover_blurred = ImageCopy(_cover);
    ImageBlurGaussian(&_cover_blurred, 5);
    _cover_blurred_texture = LoadTextureFromImage(_cover_blurred);

    _circle_clip = LoadShaderFromMemory(NULL, circle);
}

void Fluimp::update() noexcept {
    _mouse_pos = GetMousePosition();
}

void Fluimp::render() noexcept {
    ClearBackground(Color{ 0, 0, 0, 0 });

    _textures.file("main_border.png", 1, 1, 498, 498, WHITE, false);

    _textures.file("cover_shadow.png", 0, 0, 500, 500, WHITE, false);

    DrawTexture(_cover_texture, 25, 25, WHITE);

    _textures.file("cover_border.png", 24, 24, 452, 452, WHITE, false);

    // pause button
    {
        const Vector2 center = { 250.0f, 400.0f };
        const float baseRadius = 50.0f;
        const float baseSize = 100.0f;

        bool hovered = CheckCollisionPointCircle(_mouse_pos, center, baseRadius);

        float hoverTarget = hovered ? 1.0f : 0.0f;
        _hover_anim += (hoverTarget - _hover_anim) * 12.0f * GetFrameTime();
        _hover_anim = std::clamp(_hover_anim, 0.0f, 1.0f);

        float h = _hover_anim * _hover_anim * (3.0f - 2.0f * _hover_anim);
        float scale = 1.0f + h * 0.15f;

        float size = baseSize * scale;
        float x = center.x - size * 0.5f;
        float y = center.y - size * 0.5f;

        float blurRadius = baseRadius * scale;

        float texSize[2] = { (float)_cover_blurred_texture.width, (float)_cover_blurred_texture.height };
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "texSize"), texSize, SHADER_UNIFORM_VEC2);
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "radius"), &blurRadius, SHADER_UNIFORM_FLOAT);

        float centervec[2] = { center.x - 25, center.y - 25 };
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "center"), centervec, SHADER_UNIFORM_VEC2);

        _textures.file("pause_shadow.png", x-20.0f, y-20.0f, size + 40.0f, size + 40.0f, WHITE, true);

        BeginShaderMode(_circle_clip);
        {
            DrawTexture(_cover_blurred_texture, 25, 25, WHITE);
        }
        EndShaderMode();

        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            _paused = !_paused;
        }

        float target = _paused ? 1.0f : 0.0f;
        _pause_fade += (target - _pause_fade) * 20.0f * GetFrameTime();
        _pause_fade = std::clamp(_pause_fade, 0.0f, 1.0f);

        float t = _pause_fade * _pause_fade * (3.0f - 2.0f * _pause_fade);

        Color pauseCol = WHITE;
        pauseCol.a = (unsigned char)(t * 255);

        Color playCol = WHITE;
        playCol.a = (unsigned char)((1.0f - t) * 255);

        _textures.file("pause.png", x, y, size, size, pauseCol, true);
        _textures.file("not_paused.png", x, y, size, size, playCol, true);

    }

    DrawFPS(10, 10);
}

void Fluimp::cleanup() noexcept {
    UnloadTexture(_cover_texture);
    UnloadTexture(_cover_blurred_texture);

    UnloadImage(_cover);
    UnloadImage(_mask);
    UnloadImage(_cover_blurred);

    UnloadShader(_circle_clip);

    CloseWindow();
}
