#include "win32.hh"

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

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

LRESULT CALLBACK CustomWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static bool dragging = false;
    static POINT dragOffset = {};

    switch (msg)
    {
        case WM_LBUTTONDOWN:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            if (pt.y >= 0 && pt.y < 25) // draggable area (like title bar)
            {
                dragging = true;
                SetCapture(hwnd); // capture mouse
                dragOffset.x = pt.x;
                dragOffset.y = pt.y;
            }
            break;
        }

        case WM_MOUSEMOVE:
        {
            if (dragging)
            {
                POINT pt;
                GetCursorPos(&pt); // screen coordinates
                SetWindowPos(hwnd, nullptr,
                    pt.x - dragOffset.x,
                    pt.y - dragOffset.y,
                    0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }
            break;
        }

        case WM_LBUTTONUP:
        {
            if (dragging)
            {
                dragging = false;
                ReleaseCapture();
            }
            break;
        }

        case WM_NCHITTEST:
        {
            // let us still resize from borders if needed
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);
            if (pt.y >= 0 && pt.y < 25) return HTCLIENT; // draggable handled manually
            return HTCLIENT;
        }
    }

    return CallWindowProc(originalProc, hwnd, msg, wParam, lParam);
}


void init_win32(void* yehwnd) noexcept {

    HWND hwnd = (HWND)yehwnd;

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
}