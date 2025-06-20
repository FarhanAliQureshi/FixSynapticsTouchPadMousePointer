#include "framework.h"
#include "resource.h"
#include "main.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize module level variables
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

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

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
            MessageBox(hWnd, L"TODO: Settings", L"TODO", 0);
            break;
        case IDM_NOTIFYICON_ABOUT:
            MessageBox(hWnd, L"TODO: About", L"TODO", 0);
            break;
        case IDM_NOTIFYICON_EXIT:
            PostMessage(hWnd, WM_DESTROY, 0, 0);
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
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_SHOWTIP | NIF_MESSAGE;         // Should I use NIF_GUID too?
    nid.uCallbackMessage = APP_NOTIFYICON_CALLBACK_MSG;
    nid.hIcon = (HICON)GetClassLongPtr(hWnd, GCLP_HICON);
    nid.dwState = NIS_SHAREDICON;
    wcscpy_s(nid.szTip, ARRAYSIZE(nid.szTip), MAIN_WINDOW_NAME);

    return Shell_NotifyIcon(NIM_ADD, &nid);
}

BOOL RemoveTaskbarIcon(HWND hWnd)
{
    NOTIFYICONDATA nid;

    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = APP_NOTIFYICON_ID;

    return Shell_NotifyIcon(NIM_DELETE, &nid);
}

void DisplayNotifyIconPopupMenu(HWND hWnd)
{
    POINT ptCursorPosition;

    GetCursorPos(&ptCursorPosition);
    TrackPopupMenu(
        m_hNotifyIconPopupMenu,
        0,
        ptCursorPosition.x,
        ptCursorPosition.y,
        0,
        hWnd,
        NULL
    );
}