#include "framework.h"
#include "globals.h"
#include "about_dialog.h"

// Message handler for about dialog-box window.
INT_PTR CALLBACK AboutDialogBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        g_hAboutDialog = hDlg;
        return (INT_PTR)TRUE;
        break;

    case WM_COMMAND:
        switch LOWORD(wParam)
        {
        case IDOK:
        case IDCANCEL:
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
    HINSTANCE hFakeInstance = ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);

    INT_PTR nReturnValue = (INT_PTR)hFakeInstance;
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