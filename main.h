#pragma once

// Maximum acceptable string size for Class Name is 256 character
// Reference: https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexa
#define MAX_WINDOW_CLASS_NAME 256

// Global Variables:
HINSTANCE g_hInstance;
WCHAR szWindowClass[MAX_WINDOW_CLASS_NAME];
WCHAR szWindowName[MAX_WINDOW_CLASS_NAME];

// Forward declarations of functions
ATOM RegisterClassMain(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);