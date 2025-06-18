#include "framework.h"
#include "main.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Default values
    wcscpy_s(g_szWindowClass, ARRAYSIZE(g_szWindowClass),
        L"FIX SYNAPTICS TOUCH PAD MOUSE POINTER");
    wcscpy_s(g_szWindowName, ARRAYSIZE(g_szWindowName),
        L"Fix Mouse Pointer Issue of Synaptics Touch Pad");

    // Initialize and create window
    RegisterClassMain(hInstance);
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;
    AddTaskbarIcon();

    // Main message loop:
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

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = g_szWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInstance = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindow(g_szWindowClass, g_szWindowName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
        return FALSE;

    g_hWndMainWindow = hWnd;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        RemoveTaskbarIcon();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

BOOL AddTaskbarIcon()
{
    NOTIFYICONDATA nid;

    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = g_hWndMainWindow;
    nid.uID = APP_NOTIFYICON_ID;
    nid.uFlags = NIF_TIP | NIF_SHOWTIP;         // Should I use NIF_GUID too?
    nid.uCallbackMessage = APP_NOTIFYICON_CALLBACK_MSG;
    nid.hIcon = (HICON)GetClassLongPtr(g_hWndMainWindow, GCLP_HICONSM);
    wcscpy_s(nid.szTip, ARRAYSIZE(nid.szTip), g_szWindowName);

    Shell_NotifyIcon(NIM_ADD, &nid);

    return FALSE;
}

BOOL RemoveTaskbarIcon()
{
    NOTIFYICONDATA nid;

    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = g_hWndMainWindow;
    nid.uID = APP_NOTIFYICON_ID;

    Shell_NotifyIcon(NIM_DELETE, &nid);

    return FALSE;
}