#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <raylib.h>
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#undef LoadImage
#undef CloseWindow

static WNDPROC originalProc;

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    int AccentFlags;
    int GradientColor;
    int AnimationId;
};

enum WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attrib;
    void* pvData;
    SIZE_T cbData;
};

typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

LRESULT CALLBACK CustomWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCHITTEST) {
        POINT pt = {
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam)
        };
        ScreenToClient(hwnd, &pt);
        return HTCAPTION;
    }
    return CallWindowProc(originalProc, hwnd, msg, wParam, lParam);
}

int main() {
    SetTraceLogLevel(LOG_ALL);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT | FLAG_WINDOW_TRANSPARENT);

    InitWindow(500, 500, "fluimp");
    HWND hwnd = (HWND)GetWindowHandle();

    originalProc = (WNDPROC)SetWindowLongPtr(
        hwnd,
        GWLP_WNDPROC,
        (LONG_PTR)CustomWndProc
    );

    DWORD corner = DWMWCP_ROUND;
    DwmSetWindowAttribute(
        hwnd,
        DWMWA_WINDOW_CORNER_PREFERENCE,
        &corner,
        sizeof(corner)
    );

    ACCENT_POLICY accent = {
        ACCENT_ENABLE_ACRYLICBLURBEHIND,
        0,
        0x01444444,
        0
    };

    WINDOWCOMPOSITIONATTRIBDATA data = {
        WCA_ACCENT_POLICY,
        &accent,
        sizeof(accent)
    };

    HMODULE hUser = GetModuleHandle(TEXT("user32.dll"));
    if (hUser) {
        pSetWindowCompositionAttribute SetWindowCompositionAttribute =
            (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute) {
            SetWindowCompositionAttribute(hwnd, &data);
        }
    }

    const constexpr char* roundCornersShaderCode = R"(
    #version 330

    in vec2 fragTexCoord;
    in vec4 fragColor;

    out vec4 finalColor;

    uniform sampler2D texture0;
    uniform float radius;
    uniform vec2 texSize;

    void main()
    {
        vec2 uv = fragTexCoord;
        vec2 pos = uv * texSize;

        // Calculate distance to each corner
        vec2 tl = pos;                         
        vec2 tr = vec2(texSize.x - pos.x, pos.y); 
        vec2 bl = vec2(pos.x, texSize.y - pos.y); 
        vec2 br = texSize - pos;             

        float r = radius;

        if (tl.x < r && tl.y < r && length(tl - vec2(r,r)) > r) discard;
        if (tr.x < r && tr.y < r && length(tr - vec2(r,r)) > r) discard;
        if (bl.x < r && bl.y < r && length(bl - vec2(r,r)) > r) discard;
        if (br.x < r && br.y < r && length(br - vec2(r,r)) > r) discard;

        finalColor = texture(texture0, fragTexCoord) * fragColor;
    }
    )";

    Image cover = LoadImage("1.jpg");
    ImageResize(&cover, 450, 450);
    Texture2D tex = LoadTextureFromImage(cover);
    UnloadImage(cover);

    Shader round_corners_shader = LoadShaderFromMemory(NULL, roundCornersShaderCode);

    float texSize[2] = { (float)tex.width, (float)tex.height };
    SetShaderValue(round_corners_shader, GetShaderLocation(round_corners_shader, "texSize"), texSize, SHADER_UNIFORM_VEC2);

    float radiusVal[1] = { 8.0f };
    SetShaderValue(round_corners_shader, GetShaderLocation(round_corners_shader, "radius"), radiusVal, SHADER_UNIFORM_FLOAT);

    while (!WindowShouldClose()) {


        BeginDrawing();
        {
            ClearBackground(Color{ 0, 0, 0, 0 });

            BeginShaderMode(round_corners_shader);
            {
                DrawTexture(tex, 25, 25, WHITE);
            }
            EndShaderMode();
        }
        EndDrawing();
    }

    UnloadTexture(tex);
}