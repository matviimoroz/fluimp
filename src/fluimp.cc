#include "fluimp.hh"

#include "win32.hh"
#include "shader_code.hh"

#include <algorithm>
#include <raylib.h>
#include <format>
#include <rlgl.h>
#include <random>

inline static int rand_int(int min, int max)
{
    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

void Fluimp::init() noexcept {
    SetTraceLogLevel(LOG_ALL);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_TRANSPARENT);

    InitWindow(500, 500, "fluimp");

    init_win32(GetWindowHandle());

    _cover = LoadImage("assets/cover_example.jpg");
    ImageResize(&_cover, 450, 450);
    _mask = LoadImage("assets/cover_mask.png");
    ImageAlphaMask(&_cover, _mask);
    _cover_texture = LoadTextureFromImage(_cover);

    _cover_blurred = ImageCopy(_cover);
    ImageBlurGaussian(&_cover_blurred, 5);
    _cover_blurred_texture = LoadTextureFromImage(_cover_blurred);

    _circle_clip = LoadShaderFromMemory(NULL, circle);

    rlSetBlendFactorsSeparate(RL_SRC_ALPHA, RL_ONE_MINUS_SRC_ALPHA, RL_ONE, RL_ONE, RL_FUNC_ADD, RL_MAX);

    InitAudioDevice();

    // songs
    _songs = LoadDirectoryFiles("songs");
}

void Fluimp::update() noexcept {
    _mouse_pos = GetMousePosition();

    if (_skip) {
        _current_song_index = rand_int(0, _songs.count - 1);
        _current_song_path = _songs.paths[_current_song_index];
        _song = LoadMusicStream(_current_song_path);
        PlayMusicStream(_song);
        _skip = !_skip;
    } 

    if (_paused) {
        PauseMusicStream(_song);
    } else {
        ResumeMusicStream(_song);
    }

    UpdateMusicStream(_song);

    if (IsKeyDown(KEY_DOWN)) {
        SetMasterVolume(GetMasterVolume() - 0.005f);
        TraceLog(LOG_INFO, "VOLUME: %f", GetMasterVolume());
    } else if (IsKeyDown(KEY_UP)) {
        if (GetMasterVolume() != 1.0f) {
            SetMasterVolume(GetMasterVolume() + 0.005f);
            TraceLog(LOG_INFO, "VOLUME: %f", GetMasterVolume());
        }
    }
}

void Fluimp::render() noexcept {
    BeginBlendMode(RL_BLEND_CUSTOM_SEPARATE);
    ClearBackground(Color{ 0, 0, 0, 0 });

    _textures.file("assets/main_border.png", 1, 1, 498, 498, WHITE, false);

    _textures.file("assets/cover_shadow.png", 0, 0, 500, 500, WHITE, false);

    DrawTexture(_cover_texture, 25, 25, WHITE);

    _textures.file("assets/cover_border.png", 24, 24, 452, 452, WHITE, false);

    // pause button
    {
        const Vector2 center = { 250.0f, 400.0f };
        const float baseRadius = 50.0f;
        const float baseSize = 100.0f;

        bool hovered = CheckCollisionPointCircle(_mouse_pos, center, baseRadius);

        float hoverTarget = hovered ? 1.0f : 0.0f;
        _pause_hover_anim += (hoverTarget - _pause_hover_anim) * 12.0f * GetFrameTime();
        _pause_hover_anim = std::clamp(_pause_hover_anim, 0.0f, 1.0f);

        float h = _pause_hover_anim * _pause_hover_anim * (3.0f - 2.0f * _pause_hover_anim);
        float scale = 1.0f + h * 0.15f;

        float size = baseSize * scale;
        float x = center.x - size * 0.5f;
        float y = center.y - size * 0.5f;

        float blurRadius = baseRadius * scale;
        
        _textures.file("assets/pause_shadow.png", x - 20.0f, y - 20.0f, size + 40.0f, size + 40.0f, WHITE, true);

        float texSize[2] = { (float)_cover_blurred_texture.width, (float)_cover_blurred_texture.height };
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "texSize"), texSize, SHADER_UNIFORM_VEC2);
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "radius"), &blurRadius, SHADER_UNIFORM_FLOAT);

        float centervec[2] = { center.x - 25, center.y - 25 };
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "center"), centervec, SHADER_UNIFORM_VEC2);

        BeginShaderMode(_circle_clip);
        {
            DrawTexture(_cover_blurred_texture, 25, 25, WHITE);
        }
        EndShaderMode();

        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            _paused = !_paused;
        }

        float target = _paused ? 1.0f : 0.0f;
        _pause_fade_anim += (target - _pause_fade_anim) * 20.0f * GetFrameTime();
        _pause_fade_anim = std::clamp(_pause_fade_anim, 0.0f, 1.0f);

        float t = _pause_fade_anim * _pause_fade_anim * (3.0f - 2.0f * _pause_fade_anim);

        Color pauseCol = WHITE;
        pauseCol.a = (unsigned char)(t * 255);

        Color playCol = WHITE;
        playCol.a = (unsigned char)((1.0f - t) * 255);

        _textures.file("assets/pause.png", x, y, size, size, pauseCol, true);
        _textures.file("assets/not_paused.png", x, y, size, size, playCol, true);
    }

    // forward button
    {
        const Vector2 center = { 320.0f + 72.0f / 2.0f, 365.0f + 72.0f / 2.0f };
        const float radius = 75.0f / 2.0f;
        const float diameter = 75.0f;

        bool hovered = CheckCollisionPointCircle(_mouse_pos, center, diameter);

        float hoverTarget = hovered ? 1.0f : 0.0f;
        float prev_anim = _skip_hover_anim;

        _skip_hover_anim += (hoverTarget - _skip_hover_anim) * 12.0f * GetFrameTime();

        float delta_anim = _skip_hover_anim - prev_anim;

        float h = _skip_hover_anim * _skip_hover_anim * (3.0f - 2.0f * _skip_hover_anim);
        float scale = 1.0f + h * 0.15f;

        float size = baseSize * scale;
        float x = center.x - size * 0.5f;
        float y = center.y - size * 0.5f;

        float blurRadius = baseRadius * scale;

        _textures.file(
            "assets/forward_shadow.png",
            x - 20.0f, y - 20.0f,
            size + 40.0f, size + 40.0f,
            WHITE, true
        );

        float texSize[2] = {
            (float)_cover_blurred_texture.width,
            (float)_cover_blurred_texture.height
        };

        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "texSize"), texSize, SHADER_UNIFORM_VEC2);
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "radius"), &blurRadius, SHADER_UNIFORM_FLOAT);

        float centervec[2] = { center.x - 25.0f, center.y - 25.0f };
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "center"), centervec, SHADER_UNIFORM_VEC2);

        BeginShaderMode(_circle_clip);
        DrawTexture(_cover_blurred_texture, 25, 25, WHITE);
        EndShaderMode();

        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // onClick();
        }

        _textures.file(
            "assets/forward.png",
            x, y, size, size,
            WHITE, false 
        );
    }

    EndBlendMode();
}

void Fluimp::cleanup() noexcept {
    UnloadTexture(_cover_texture);
    UnloadTexture(_cover_blurred_texture);

    UnloadImage(_cover);
    UnloadImage(_mask);
    UnloadImage(_cover_blurred);

    UnloadShader(_circle_clip);

    UnloadDirectoryFiles(_songs);

    CloseWindow();
}
