#include "framework.h"
#include "resource.h"
#include "globals.h"
#include "utils.h"
#include "about_dialog.h"

// Module-level variables (shouldn't be accessable from outside)
HFONT m_hTitleFont;

// Forward declarations of functions (keeping them private to this module)
void InitControls(HWND);
void InitHeaderControl(HWND);
void InitVersionInfoControl(HWND);
void Cleanup();

// Message handler for about dialog-box window.
INT_PTR CALLBACK AboutDialogBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        g_hAboutDialog = hDlg;
        InitControls(hDlg);
        return (INT_PTR)TRUE;
        break;

    case WM_COMMAND:
        switch LOWORD(wParam)
        {
        case IDOK:
        case IDCANCEL:
            Cleanup();
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case NM_CLICK:
        case NM_RETURN:
            OpenLink(hDlg, lParam);
            break;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

BOOL OpenLink(HWND hWnd, LPARAM lParam)
{
    PNMLINK pNMLink = (PNMLINK)lParam;
    LITEM item = pNMLink->item;
    HINSTANCE hShellInstance = ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);

    INT_PTR nReturnValue = (INT_PTR)hShellInstance;
    if (nReturnValue <= 32)
    {
        // We got some error
        WCHAR msg[MAX_PATH];
        StringCchCopy(msg, ARRAYSIZE(msg), L"Error opening link for ");
        // item.szID contains ID HTML tag, for example "website" or "repository"
        StringCchCat(msg, ARRAYSIZE(msg), item.szID);
        MessageBox(hWnd, msg, NULL, MB_OK);
        return FALSE;
    }

    return TRUE;
}

void InitControls(HWND hDlg)
{
    InitHeaderControl(hDlg);
    InitVersionInfoControl(hDlg);
}

void Cleanup()
{
    DeleteObject(m_hTitleFont);
}

void InitHeaderControl(HWND hDlg)
{
    // Header of About dialog (Static Lable Control).
    // Increase font size of Title control and make it bold text.
    HWND hTitleControl = GetDlgItem(hDlg, IDC_STATIC_TITLE);
    HFONT hOriginalFont = (HFONT)SendMessage(hTitleControl, WM_GETFONT, 0, 0);

    // Modify font.
    LOGFONT logFont;
    GetObject(hOriginalFont, sizeof(LOGFONT), &logFont);
    logFont.lfWeight = FW_BOLD;
    // Increase font's height.
    // Reference: https://learn.microsoft.com/en-us/windows/win32/api/shtypes/ns-shtypes-logfontw
    if (logFont.lfHeight < 0) logFont.lfHeight -= 2;
    if (logFont.lfHeight > 0) logFont.lfHeight += 2;

    // Create a new font based on the modified font options, and apply it.
    m_hTitleFont = CreateFontIndirect(&logFont);        // Must be deleted when destroying dialog
    SendMessage(hTitleControl, WM_SETFONT, (WPARAM)m_hTitleFont, (LPARAM)TRUE);
    AutoResizeControl(hDlg, IDC_STATIC_TITLE);
}

void InitVersionInfoControl(HWND hDlg)
{
    WCHAR szVersion[MAX_PATH];    
    StringCchCopy(szVersion, ARRAYSIZE(szVersion), L"Version");

    // First, get the name of the current module (executable file)
    WCHAR szModuleName[MAX_PATH];
    if (!GetModuleFileName(NULL, szModuleName, ARRAYSIZE(szModuleName)))
        return;

    // Determine the size of the buffer that we need to store the version information
    DWORD dwBufferSize = GetFileVersionInfoSize(szModuleName, NULL);
    if (dwBufferSize == 0)
        return;

    // Allocate buffer on heap for version information
    LPVOID lpBuffer = HeapAlloc(GetProcessHeap(), 0, dwBufferSize);
    if (lpBuffer == NULL)
        return;

    // Get version information in heap buffer
    if (!GetFileVersionInfo(szModuleName, 0, dwBufferSize, lpBuffer))
    {
        HeapFree(GetProcessHeap(), 0, lpBuffer);
        return;
    }

    // Extract version numbers from the heap buffer
    UINT nVersionLength;
    VS_FIXEDFILEINFO *lpVersionInfo;
    if (VerQueryValue(lpBuffer, L"\\", (LPVOID*)&lpVersionInfo, &nVersionLength))
    {
        WCHAR szProductVersion[MAX_PATH];
        ZeroMemory(szProductVersion, ARRAYSIZE(szProductVersion));
        StringCchPrintf(
            szProductVersion, 
            ARRAYSIZE(szProductVersion), 
            L" %u.%u.%u.%u", 
            HIWORD(lpVersionInfo->dwProductVersionMS),
            LOWORD(lpVersionInfo->dwProductVersionMS),
            HIWORD(lpVersionInfo->dwProductVersionLS),
            LOWORD(lpVersionInfo->dwProductVersionLS)
        );
        StringCchCat(szVersion, ARRAYSIZE(szVersion), szProductVersion);
    }

    // Free system resources since we don't need buffer anymore
    HeapFree(GetProcessHeap(), 0, lpBuffer);
    lpBuffer = NULL;

    // Get build Date from compiler
    WCHAR szBuildDate[MAX_PATH];
    StringCchCopy(szBuildDate, ARRAYSIZE(szBuildDate), L" (Build on ");
    // For __DATE__ Reference: https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
    StringCchCat(szBuildDate, ARRAYSIZE(szBuildDate), _CRT_WIDE(__DATE__));
    StringCchCat(szBuildDate, ARRAYSIZE(szBuildDate), L")");
    StringCchCat(szVersion, ARRAYSIZE(szVersion), szBuildDate);

    // Display Version information on About dialog
    SetDlgItemText(hDlg, IDC_STATIC_VERSION, szVersion);
    AutoResizeControl(hDlg, IDC_STATIC_VERSION);
}