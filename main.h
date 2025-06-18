#pragma once

#define APP_NOTIFYICON_ID 1001
#define APP_NOTIFYICON_CALLBACK_MSG 2001
#define MAIN_WINDOW_CLASS L"FIX_SYNAPTICS_TOUCH_PAD_MOUSE_POINTER"
#define MAIN_WINDOW_NAME L"Fix Mouse Pointer Issue of Synaptics Touch Pad"

// Forward declarations of functions
ATOM RegisterClassMain(HINSTANCE);
HWND InitWindowMain(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
BOOL AddTaskbarIcon(HWND);
BOOL RemoveTaskbarIcon(HWND);