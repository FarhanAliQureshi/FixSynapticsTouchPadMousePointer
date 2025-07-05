#include "framework.h"
#include "globals.h"
#include "resource.h"
#include "about_dialog.h"

// Module-level variables (shouldn't be accessable from outside)
HFONT m_hTitleFont;

// Forward declarations of functions (keeping them private to this module)
void InitControls(HWND);
void Cleanup();
void AutoResizeControl(HWND, DWORD);

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
    // Title of App (Static Lable Control).
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

void Cleanup()
{
    DeleteObject(m_hTitleFont);
}

void AutoResizeControl(HWND hDlg, DWORD nIDDlgItem)
{
    HWND hControl = GetDlgItem(hDlg, nIDDlgItem);
    DWORD dwTextLength = GetWindowTextLength(hControl);

    if (dwTextLength == 0)
    {
        // There is no text in the given control. Resize to zero.
        SetWindowPos(hControl, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER);
        return;
    }

    // Alocate buffer on the heap.
    SIZE_T dwBufferSize = (dwTextLength + 1) * sizeof(WCHAR);   // Adding 1 for null character
    LPWSTR pszBuffer = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, dwBufferSize);
    // Did we run out of memory? If so then we have bigger problems to deal with.
    if (pszBuffer == NULL)
        return;

    // Copy text in control to buffer
    GetDlgItemText(hDlg, nIDDlgItem, pszBuffer, dwTextLength);

    // Measure the size which is needed to display that text in given control and resize the control
    SIZE newSize;
    HDC hDC = GetDC(hControl);
    GetTextExtentPoint32(hDC, pszBuffer, dwTextLength, &newSize);
    ReleaseDC(hControl, hDC);
    SetWindowPos(hControl, 0, 0, 0, newSize.cx, newSize.cy, SWP_NOMOVE | SWP_NOZORDER);

    HeapFree(GetProcessHeap(), 0, pszBuffer);
}