#include "fluimp.hh"

#include "win32.hh"
#include "shader_code.hh"

#include <raylib.h>
#include <raymath.h>
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

        float hover_target = hovered ? 1.0f : 0.0f;
        _pause_hover_anim += (hover_target - _pause_hover_anim) * 12.0f * GetFrameTime();
        if (fabsf(_pause_hover_anim - hover_target) < 0.035f) {
            _pause_hover_anim = hover_target;
        }

        float scale = 1.0f + _pause_hover_anim * 0.2f;

        float size = baseSize * scale;
        float x = center.x - size * 0.5f;
        float y = center.y - size * 0.5f;

        float blurRadius = baseRadius * scale;

        float shadowSize = (baseSize + 40.0f) * scale;
        float shadowX = center.x - shadowSize * 0.5f;
        float shadowY = center.y - shadowSize * 0.5f;

        _textures.file(
            "assets/pause_shadow.png",
            shadowX,
            shadowY,
            shadowSize,
            shadowSize,
            WHITE,
            true
        );

        float texSize[2] = { (float)_cover_blurred_texture.width, (float)_cover_blurred_texture.height };
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "texSize"), texSize, SHADER_UNIFORM_VEC2);
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "radius"), &blurRadius, SHADER_UNIFORM_FLOAT);

        float centervec[2] = { center.x - 25.0f, center.y - 25.0f };
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "center"), centervec, SHADER_UNIFORM_VEC2);

        BeginShaderMode(_circle_clip);
        DrawTexture(_cover_blurred_texture, 25, 25, WHITE);
        EndShaderMode();

        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            _paused = !_paused;
        }

        float target = _paused ? 1.0f : 0.0f;
        _pause_fade_anim += (target - _pause_fade_anim) * 20.0f * GetFrameTime();

        float t = _pause_fade_anim * _pause_fade_anim * (3.0f - 2.0f * _pause_fade_anim);

        Color pauseCol = WHITE;
        pauseCol.a = (unsigned char)(t * 255);

        Color playCol = WHITE;
        playCol.a = (unsigned char)((1.0f - t) * 255);

        float iconSize = size;
        float iconX = center.x - iconSize * 0.5f;
        float iconY = center.y - iconSize * 0.5f;

        _textures.file("assets/pause.png", iconX, iconY, iconSize, iconSize, pauseCol, true);
        _textures.file("assets/not_paused.png", iconX, iconY, iconSize, iconSize, playCol, true);
    }


    // forward button
    {
        constexpr float btnSize = 75.0f;
        constexpr Vector2 btnPos = { 320.0f, 365.0f };

        static float hover_anim = 0.0f;

        float baseRadius = btnSize * 0.5f;
        Vector2 btnCenter = {
            btnPos.x + baseRadius,
            btnPos.y + baseRadius
        };

        bool hovered = CheckCollisionPointCircle(_mouse_pos, btnCenter, baseRadius);

        float hover_target = hovered ? 1.0f : 0.0f;
        hover_anim += (hover_target - hover_anim) * 12.0f * GetFrameTime();
        if (fabsf(hover_anim - hover_target) < 0.035f) {
            hover_anim = hover_target;
        }

        float scale = 1.0f + hover_anim * 0.2f;

        float radius = baseRadius * scale;

        float shadowSize = (btnSize + 40.0f) * scale;
        Vector2 shadowPos = {
            btnCenter.x - shadowSize * 0.5f,
            btnCenter.y - shadowSize * 0.5f
        };

        _textures.file(
            "assets/forward_shadow.png",
            shadowPos.x,
            shadowPos.y,
            shadowSize,
            shadowSize,
            WHITE,
            true
        );

        float texSize[2] = {
            (float)_cover_blurred_texture.width,
            (float)_cover_blurred_texture.height
        };
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "texSize"), texSize, SHADER_UNIFORM_VEC2);
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "radius"), &radius, SHADER_UNIFORM_FLOAT);

        float center[2] = {
            btnCenter.x - 25.0f,
            btnCenter.y - 25.0f
        };
        SetShaderValue(_circle_clip, GetShaderLocation(_circle_clip, "center"), center, SHADER_UNIFORM_VEC2);

        BeginShaderMode(_circle_clip);
        DrawTexture(_cover_blurred_texture, 25, 25, WHITE);
        EndShaderMode();

        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            TraceLog(LOG_DEBUG, "CLICKED: SKIP");
        }

        float iconSize = btnSize * scale;
        Vector2 iconPos = {
            btnCenter.x - iconSize * 0.5f,
            btnCenter.y - iconSize * 0.5f
        };

        _textures.file(
            "assets/forward.png",
            iconPos.x,
            iconPos.y,
            iconSize,
            iconSize,
            WHITE,
            false
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
