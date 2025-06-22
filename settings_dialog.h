#pragma once

// Forward declarations of functions
INT_PTR CALLBACK SettingsDialogBox(HWND, UINT, WPARAM, LPARAM);
INT_PTR InitTabControl(HWND);
INT_PTR CALLBACK TabPages(HWND, UINT, WPARAM, LPARAM);
void OnTabSelectionChange();
void DestroyTabControlDialogs();
