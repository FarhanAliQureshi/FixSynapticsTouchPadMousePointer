#include "framework.h"
#include "utils.h"

BOOL CenterWindowToScreen(HWND hWnd) 
{
    RECT rectWindow;
    DWORD nWindowWidth, nWindowHeight, nX, nY, nScreenWidth, nScreenHeight;

    // Get window dimensions
    GetWindowRect(hWnd, &rectWindow);
    nWindowWidth = rectWindow.right - rectWindow.left;
    nWindowHeight = rectWindow.bottom - rectWindow.top;

    // Get screen dimensions
    nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate center coordinates
    nX = (nScreenWidth - nWindowWidth) / 2;
    nY = (nScreenHeight - nWindowHeight) / 2;

    // Move window to center
    return MoveWindow(hWnd, nX, nY, nWindowWidth, nWindowHeight, TRUE);
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