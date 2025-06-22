/*
    TODO: Make the IDD_PROPPAGE_SETTINGS_GENERAL (inside IDC_TAB1) to have
    transparent background.

    TODO: Resize IDD_PROPPAGE_SETTINGS_GENERAL to have (maximum) sized exactly to 
    the client-area of IDC_TAB1. 

    TODO: Tab-key (Tab-stop) works with controls in child IDD_PROPPAGE_SETTINGS_GENERAL
    with parent dialog window IDD_SETTINGS_DIALOG.
*/

#include "framework.h"
#include "globals.h"
#include "resource.h"
#include "settings_dialog.h"

// Module-level variables (shouldn't be accessable from outside)
typedef struct TABCONTROLDATA
{
    UINT nTotalPages;   // Set in function InitTabControl()
    HWND hPage[1];      // We currently have only one page in our tab control
    HWND hTabControl;
} TABCONTROLDATA;

TABCONTROLDATA m_tabControlData;

// Message handler for about dialog-box window.
INT_PTR CALLBACK SettingsDialogBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        InitTabControl(hDlg);
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
        case TCN_SELCHANGE:
            OnTabSelectionChange();
            break;
        }
        break;

    case WM_DESTROY:
        DestroyTabControlDialogs();
        break;
    }

    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK TabPages(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
        break;
    }

    return (INT_PTR)FALSE;
}

INT_PTR InitTabControl(HWND hParent)
{
    // Store HWND of tab control
    m_tabControlData.hTabControl = GetDlgItem(hParent, IDC_TAB1);

    // Create tabs in tab control
    TCITEM tci;
    ZeroMemory(&tci, sizeof(TCITEM));
    tci.mask = TCIF_TEXT;
    tci.pszText = L"General";
    TabCtrl_InsertItem(m_tabControlData.hTabControl, 0, &tci);

    // Store total numbers of tabs which we just created
    m_tabControlData.nTotalPages = TabCtrl_GetItemCount(m_tabControlData.hTabControl);

    // Create dialogs for each tab page
    m_tabControlData.hPage[0] = CreateDialog(
        g_hInstance,
        MAKEINTRESOURCE(IDD_PROPPAGE_SETTINGS_GENERAL),
        m_tabControlData.hTabControl,
        TabPages
    );

    // Move all pages to appear inside tab control
    for (UINT i = 0; i < m_tabControlData.nTotalPages; i++)
    {
        RECT tabDisplay;
        GetWindowRect(m_tabControlData.hPage[i], &tabDisplay);
        TabCtrl_AdjustRect(m_tabControlData.hTabControl, FALSE, &tabDisplay);
        POINT pt;
        pt.x = tabDisplay.left; pt.y = tabDisplay.top;
        ScreenToClient(m_tabControlData.hTabControl, &pt);
        SetWindowPos(
            m_tabControlData.hPage[i], HWND_TOP,
            pt.x, pt.y, 
            tabDisplay.right - tabDisplay.left, tabDisplay.bottom - tabDisplay.top, 
            SWP_HIDEWINDOW
        );
    }

    // Show first page
    TabCtrl_SetCurSel(m_tabControlData.hTabControl, 0);
    OnTabSelectionChange();

    return (INT_PTR)TRUE;
}

void OnTabSelectionChange()
{
    UINT selected = TabCtrl_GetCurSel(m_tabControlData.hTabControl);
    for (UINT i = 0; i < m_tabControlData.nTotalPages; i++)
    {
        ShowWindow(
            m_tabControlData.hPage[i],
            (selected == i) ? SW_SHOW : SW_HIDE
        );
    }
}

void DestroyTabControlDialogs()
{
    for (UINT i = 0; i < m_tabControlData.nTotalPages; i++)
    {
        DestroyWindow(m_tabControlData.hPage[i]);
        m_tabControlData.hPage[i] = NULL;
    }
}