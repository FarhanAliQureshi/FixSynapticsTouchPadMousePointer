#pragma once

BOOL CenterWindowToScreen(HWND);
void AutoResizeControl(HWND, DWORD);
BOOL KillTask(LPCWSTR, DWORD);
void GetShellExecuteErrorMessage(LPWSTR, DWORD, DWORD);
void GetLastErrorMessage(LPWSTR, DWORD, DWORD);