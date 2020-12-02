
#include <Windows.h>

#include <svr/vulkan_simple_context.hpp>

#include <cstdint>
#include <memory>

constexpr char window_title[] = "Simple Context App";
constexpr char class_name[]   = "Simple Context App Window Class";

bool is_running   = true;
bool is_minimized = false;

int32_t client_area_width  = 1280;
int32_t client_area_height = 720;

std::unique_ptr<svr::vulkan_simple_context> context;

LRESULT CALLBACK
window_proc(_In_ HWND hwnd, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam);

int WINAPI WinMain(
    _In_ HINSTANCE            hinstance,
    [[maybe_unused]] _In_opt_ HINSTANCE,
    [[maybe_unused]] _In_     LPSTR,
    _In_ int                  show_cmd)
{
    DWORD window_style          = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
    DWORD extended_window_style = WS_EX_OVERLAPPEDWINDOW;

    MSG msg;

    RECT window_size = {0, 0, client_area_width, client_area_height};

    WNDCLASSEXA wcex   = {};
    wcex.cbSize        = sizeof(wcex);
    wcex.hIcon         = LoadIcon(hinstance, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpfnWndProc   = window_proc;
    wcex.hInstance     = hinstance;
    wcex.lpszClassName = class_name;
    wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    if (!RegisterClassEx(&wcex)) {
        return -1;
    }

    AdjustWindowRectEx(&window_size, window_style, false, extended_window_style);

    HWND hwnd = CreateWindowExA(
        extended_window_style,
        class_name,
        window_title,
        window_style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        window_size.right - window_size.left,
        window_size.bottom - window_size.top,
        NULL,
        NULL,
        hinstance,
        NULL);

    if (!hwnd) {
        return -1;
    }

    ShowWindow(hwnd, show_cmd);

    // application specific setup
    context = std::make_unique<svr::vulkan_simple_context>();

    while (is_running) {
        // update game/simulation

        if (!is_minimized) {
            // draw frame
        }

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message) {
                is_running = false;
            }
            else {

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    return 0;
}

LRESULT CALLBACK
window_proc(_In_ HWND hwnd, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam)
{
    LRESULT result = 0;

    switch (msg) {
        case WM_CLOSE: {
            DestroyWindow(hwnd);
        } break;
        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;
        case WM_SIZE: {
            UINT width  = LOWORD(lparam);
            UINT height = HIWORD(lparam);

            if (width == 0 || height == 0) {
                is_minimized = true;
            }
            else {
                is_minimized = false;
            }
        } break;
        case WM_KEYDOWN: {
            switch (wparam) {
                case VK_ESCAPE: {
                    PostQuitMessage(0);
                } break;
            }
        } break;
        default: {
            result = DefWindowProc(hwnd, msg, wparam, lparam);
        } break;
    }

    return result;
}
