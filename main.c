#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "version.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "framework.h"
#include "resource.h"
#include "globals.h"
#include "utils.h"
#include "main.h"
#include "settings.h"
#include "about_dialog.h"
#include "settings_dialog.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize extended common controls
    if (!InitExtendedControls())
    {
        MessageBox(NULL, L"Unable to initialize Common Controls", NULL, MB_ICONERROR);
        return FALSE;
    }

    // Initalize global variables
    g_hInstance = hInstance;
    g_hAboutDialog = NULL;
    g_hSettingsDialog = NULL;

    // Load settings
    g_asAppSettings.SettingsLocation = SETTINGS_IN_REGISTRY;
    LoadSettings(&g_asAppSettings);

    // Initialize module level variables
    hWndAbout = NULL;
    HMENU hNotifyIconBaseMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_NOTIFYICON_MENU));
    m_hNotifyIconPopupMenu = GetSubMenu(hNotifyIconBaseMenu, 0);

    // Initialize local variables
    HWND hWndMainWindow = 0;
    ATOM hAtomMainClassId = 0;

    // Initialize and create window
    if (!(hAtomMainClassId = RegisterClassMain(hInstance)))
        return FALSE;
    if (!(hWndMainWindow = InitWindowMain(hInstance, nCmdShow)))
        return FALSE;
    if (!AddTaskbarIcon(hWndMainWindow))
        return FALSE;

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    DestroyMenu(hNotifyIconBaseMenu);

    return (int)msg.wParam;
}

ATOM RegisterClassMain(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = MAIN_WINDOW_CLASS;
    // According to following reference, Windows should automatically find and load appropiate small size icon
    // Reference: https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexw
    wcex.hIconSm = NULL;

    return RegisterClassEx(&wcex);
}

HWND InitWindowMain(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd = CreateWindow(
        MAIN_WINDOW_CLASS, 
        MAIN_WINDOW_NAME, 
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 
        0, 
        CW_USEDEFAULT, 
        0, 
        NULL, 
        NULL, 
        hInstance, 
        NULL
    );

    if (!hWnd)
        return hWnd;

    // Main window will be hidden as we don't need it other than message-loop
    //ShowWindow(hWnd, nCmdShow);
    //UpdateWindow(hWnd);

    return hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_NOTIFYICON_SETTINGS:
            if (!IsDialogBoxAlreadyCreated(g_hSettingsDialog))
            {
                if (DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_SETTINGS_DIALOG), hWnd, SettingsDialogBox) == IDOK)
                    SaveSettings(g_asAppSettings);
                g_hSettingsDialog = NULL;
            }
            break;
        case IDM_NOTIFYICON_ABOUT:
            if (!IsDialogBoxAlreadyCreated(g_hAboutDialog))
            {
                DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT_DIALOG), hWnd, AboutDialogBox);
                g_hAboutDialog = NULL;
            }
            break;
        case IDM_NOTIFYICON_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        ;  // If I don't put an empty statement here then compiler give weird error: E1072 a declaration cannot have a label
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case APP_NOTIFYICON_CALLBACK_MSG:
        switch (lParam)
        {
        case WM_RBUTTONDOWN:
            DisplayNotifyIconPopupMenu(hWnd);
            break;
        case WM_LBUTTONDOWN:
            MessageBox(hWnd, L"TODO: Fix the TouchPad issue", L"TODO", 0);
            break;
        }
        break;
    case WM_DESTROY:
        RemoveTaskbarIcon(hWnd);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

BOOL AddTaskbarIcon(HWND hWnd)
{
    NOTIFYICONDATA nid;

    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = APP_NOTIFYICON_ID;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_SHOWTIP | NIF_MESSAGE; // | NIF_GUID;
    nid.uCallbackMessage = APP_NOTIFYICON_CALLBACK_MSG;
    nid.hIcon = (HICON)GetClassLongPtr(hWnd, GCLP_HICON);
    nid.dwState = NIS_SHAREDICON;
    //nid.guidItem = APP_NOTIFYICON_GUID;
    // Using StringCchCopy instead of wcscpy_s (which is part of C Runtime (CRT)).
    StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), MAIN_WINDOW_NAME);

    return Shell_NotifyIcon(NIM_ADD, &nid);

    // NOTIFYICON_VERSION_4 is prefered
    //nid.uVersion = NOTIFYICON_VERSION_4;
    //return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

BOOL RemoveTaskbarIcon(HWND hWnd)
{
    NOTIFYICONDATA nid;

    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = APP_NOTIFYICON_ID;
    //nid.guidItem = APP_NOTIFYICON_GUID;

    return Shell_NotifyIcon(NIM_DELETE, &nid);
}

BOOL DisplayNotifyIconPopupMenu(HWND hWnd)
{
    POINT ptCursorPosition;

    GetCursorPos(&ptCursorPosition);
    return TrackPopupMenu(
        m_hNotifyIconPopupMenu,
        0,
        ptCursorPosition.x,
        ptCursorPosition.y,
        0,
        hWnd,
        NULL
    );
}

BOOL InitExtendedControls()
{
    INITCOMMONCONTROLSEX icce;

    ZeroMemory(&icce, sizeof(INITCOMMONCONTROLSEX));
    icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icce.dwICC = ICC_LINK_CLASS | ICC_TAB_CLASSES | ICC_UPDOWN_CLASS;
    
    return InitCommonControlsEx(&icce);
}

BOOL IsDialogBoxAlreadyCreated(HWND hDlg)
{
    if (hDlg == NULL)
        return FALSE;

    CenterWindowToScreen(hDlg);
    BringWindowToTop(hDlg);
    SetActiveWindow(hDlg);
    SetForegroundWindow(hDlg);

    return TRUE;
}