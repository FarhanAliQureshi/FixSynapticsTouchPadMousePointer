#include "framework.h"
#include <tlhelp32.h>
#include <Shlwapi.h>
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

/// <summary>
/// Loop through all active processes and kill any process that matches with the given executable filename
/// </summary>
/// <param name="pszExeFilename">Filename without path but with extension (case insensitive)</param>
/// <param name="dwWaitToConfirm">(Optional) Wait in seconds to confirm process is terminated. Zero means don't wait to confirm</param>
/// <returns>TRUE if killed all processes matching the filename</returns>
BOOL KillTask(LPCWSTR pszExeFilename, DWORD dwWaitToConfirm)
{
    BOOL bSuccess = FALSE;

    HANDLE hSnapProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapProcess == INVALID_HANDLE_VALUE)
        return FALSE;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    BOOL bContinueLoop = Process32First(hSnapProcess, &pe32);

    while (bContinueLoop)
    {
        LPCWSTR pszProcessFilename = PathFindFileName(pe32.szExeFile);

        if (CompareStringEx(
            LOCALE_NAME_INVARIANT, 
            LINGUISTIC_IGNORECASE, 
            pszExeFilename, 
            -1, 
            pszProcessFilename, 
            -1, 
            NULL, 
            NULL, 
            0) == CSTR_EQUAL)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if (hProcess)
            {
                bSuccess = TerminateProcess(hProcess, 0);
                if (dwWaitToConfirm > 0)
                    WaitForSingleObject(hProcess, dwWaitToConfirm * 1000);
                CloseHandle(hProcess);
            }
        }

        bContinueLoop = Process32Next(hSnapProcess, &pe32);
    }

    CloseHandle(hSnapProcess);

    return bSuccess;
}

void GetShellExecuteErrorMessage(LPWSTR pszBuffer, DWORD dwBufferSize, DWORD dwErrorCode)
{
    if (dwErrorCode > 32)
    {
        // Not an error
        StringCchCopy(pszBuffer, dwBufferSize, L"");
        return;
    }

    switch (dwErrorCode)
    {
    case 0: StringCchCopy(pszBuffer, dwBufferSize, L"The operating system is out of memory or resources."); break;
    case ERROR_FILE_NOT_FOUND: StringCchCopy(pszBuffer, dwBufferSize, L"The specified file was not found."); break;
    case ERROR_PATH_NOT_FOUND: StringCchCopy(pszBuffer, dwBufferSize, L"The specified path was not found."); break;
    case ERROR_BAD_FORMAT: StringCchCopy(pszBuffer, dwBufferSize, L"The .exe file is invalid (non-Win32 .exe or error in .exe image)."); break;
    case SE_ERR_ACCESSDENIED: StringCchCopy(pszBuffer, dwBufferSize, L"The operating system denied access to the specified file."); break;
    case SE_ERR_ASSOCINCOMPLETE: StringCchCopy(pszBuffer, dwBufferSize, L"The file name association is incomplete or invalid."); break;
    case SE_ERR_DDEBUSY: StringCchCopy(pszBuffer, dwBufferSize, L"The DDE transaction could not be completed because other DDE transactions were being processed."); break;
    case SE_ERR_DDEFAIL: StringCchCopy(pszBuffer, dwBufferSize, L"The DDE transaction failed."); break;
    case SE_ERR_DDETIMEOUT: StringCchCopy(pszBuffer, dwBufferSize, L"The DDE transaction could not be completed because the request timed out."); break;
    case SE_ERR_DLLNOTFOUND: StringCchCopy(pszBuffer, dwBufferSize, L"The specified DLL was not found."); break;
    // SE_ERR_FNF is same as ERROR_FILE_NOT_FOUND
    //case SE_ERR_FNF: StringCchCopy(pszBuffer, dwBufferSize, L"The specified file was not found."); break;
    case SE_ERR_NOASSOC: StringCchCopy(pszBuffer, dwBufferSize, L"There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable."); break;
    case SE_ERR_OOM: StringCchCopy(pszBuffer, dwBufferSize, L"There was not enough memory to complete the operation."); break;
    // SE_ERR_PNF is same as ERROR_PATH_NOT_FOUND
    //case SE_ERR_PNF: StringCchCopy(pszBuffer, dwBufferSize, L"The specified path was not found."); break;
    case SE_ERR_SHARE: StringCchCopy(pszBuffer, dwBufferSize, L"A sharing violation occurred."); break;
    default: StringCchCopy(pszBuffer, dwBufferSize, L"Unknown error"); break;
    }
}

void GetLastErrorMessage(LPWSTR pszBuffer, DWORD dwBufferSize, DWORD dwErrorCode)
{
    LPVOID lpMessageBuffer = NULL;

    if (dwErrorCode == 0)
    {
        StringCchCopy(pszBuffer, dwBufferSize, L"No error");
        return;
    }

    DWORD nMessageBufferSize = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&lpMessageBuffer,
        0,
        NULL
    );

    if (nMessageBufferSize == 0 || lpMessageBuffer == NULL)
    {
        // FormatMessage failed for unknown reasons
        StringCchCopy(pszBuffer, dwBufferSize, L"Unknown error");
        if (lpMessageBuffer)
            LocalFree(lpMessageBuffer);
        return;
    }

    StringCchCopy(pszBuffer, dwBufferSize, (LPWSTR)lpMessageBuffer);

    LocalFree(lpMessageBuffer);
}