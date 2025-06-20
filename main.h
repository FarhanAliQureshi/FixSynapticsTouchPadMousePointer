#pragma once

#define MAIN_WINDOW_CLASS L"FIX_SYNAPTICS_TOUCH_PAD_MOUSE_POINTER"
#define MAIN_WINDOW_NAME L"Fix Mouse Pointer Issue of Synaptics Touch Pad"
#define APP_NOTIFYICON_ID WM_USER+1
#define APP_NOTIFYICON_CALLBACK_MSG WM_USER+2

// Module level variables
HMENU m_hNotifyIconPopupMenu;

// Forward declarations of functions
ATOM RegisterClassMain(HINSTANCE);
HWND InitWindowMain(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
BOOL AddTaskbarIcon(HWND);
BOOL RemoveTaskbarIcon(HWND);
void DisplayNotifyIconPopupMenu(HWND);