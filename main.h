#pragma once

// Maximum acceptable string size for Class Name is 256 character
// Reference: https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexa
#define MAX_WINDOW_CLASS_NAME 256       
#define MAX_WINDOW_NAME 128       // Reducing size to 128 due to NOTIFYICONDATA.szTip 
#define APP_NOTIFYICON_ID 1001
#define APP_NOTIFYICON_CALLBACK_MSG 2001

// Global Variables:
HINSTANCE g_hInstance;
WCHAR g_szWindowClass[MAX_WINDOW_CLASS_NAME];
WCHAR g_szWindowName[MAX_WINDOW_NAME];
HWND g_hWndMainWindow;

// Forward declarations of functions
ATOM RegisterClassMain(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
BOOL AddTaskbarIcon();
BOOL RemoveTaskbarIcon();