#pragma once

#define MAIN_WINDOW_CLASS L"FIX_SYNAPTICS_TOUCHPAD_MOUSE_POINTER"
#define MAIN_WINDOW_NAME L"Fix Mouse Pointer Issue of Synaptics TouchPad"
#define APP_NOTIFYICON_ID WM_USER+1
#define APP_NOTIFYICON_CALLBACK_MSG WM_USER+2
// {9ED847B6-5709-4DA1-AE42-57CB4FFA3F0F}
//DEFINE_GUID(APP_NOTIFYICON_GUID, 0x9ed847b6, 0x5709, 0x4da1, 0xae, 0x42, 0x57, 0xcb, 0x4f, 0xfa, 0x3f, 0xf);

// Module level variables
HMENU m_hNotifyIconPopupMenu;
HWND hWndAbout;

// Forward declarations of functions
ATOM RegisterClassMain(HINSTANCE);
HWND InitWindowMain(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL AddTaskbarIcon(HWND);
BOOL RemoveTaskbarIcon(HWND);
BOOL DisplayNotifyIconPopupMenu(HWND);
BOOL InitExtendedControls();
BOOL IsDialogBoxAlreadyCreated(HWND);