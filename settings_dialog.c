/*
    TODO: Make the IDD_PROPPAGE_SETTINGS_GENERAL (inside IDC_TAB1) to have
    transparent background.
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

// Forward declarations of functions (keeping them private to this module)
void DisplaySettings();
void StoreSettings();
BOOL ValidateSettings(HWND);
void DisplayGeneralSettings(HWND);
void StoreGeneralSettings(HWND);
BOOL ValidateGeneralSettings(HWND, HWND);

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
            if (!ValidateSettings(hDlg)) 
                return (INT_PTR)FALSE;
            StoreSettings();
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
    // Store HWND of Tab Control
    m_tabControlData.hTabControl = GetDlgItem(hParent, IDC_TAB1);
    // Make Tab Control as a parent of other child windows
    LONG tabControlExStyles = GetWindowLong(m_tabControlData.hTabControl, GWL_EXSTYLE);
    tabControlExStyles |= WS_EX_CONTROLPARENT;
    SetWindowLong(m_tabControlData.hTabControl, GWL_EXSTYLE, tabControlExStyles);

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

    // Display settings on tab pages
    DisplaySettings();

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

void DisplaySettings()
{
    DisplayGeneralSettings(m_tabControlData.hPage[0]);
}

void StoreSettings()
{
    StoreGeneralSettings(m_tabControlData.hPage[0]);
}

BOOL ValidateSettings(HWND hDlg)
{
    if(!ValidateGeneralSettings(hDlg, m_tabControlData.hPage[0])) return FALSE;

    return TRUE;
}

void DisplayGeneralSettings(HWND hPropertyPage)
{
    SetDlgItemText(hPropertyPage, IDC_EDIT_FOLDER, g_asAppSettings.PathToSynapticsApp);
    SetDlgItemInt(hPropertyPage, IDC_EDIT_DELAY, g_asAppSettings.RelaunchDelay, FALSE);
}

void StoreGeneralSettings(HWND hPropertyPage)
{
    BOOL bSuccess;
    APP_SETTINGS defaultSettings;
    WCHAR buffer[MAX_SETTINGS_VALUE_SIZE];

    LoadDefaultSettings(&defaultSettings);

    // Path to Synaptics App
    UINT nResult = GetDlgItemText(hPropertyPage, IDC_EDIT_FOLDER, buffer, ARRAYSIZE(buffer));
    if (nResult)
    {
        size_t length;
        if (StringCchLength(buffer, ARRAYSIZE(buffer), &length) == S_OK)
        {
            // Don't let user to make an invalid setting
            if (length == 0)
                StringCchCopy(buffer, ARRAYSIZE(buffer), defaultSettings.PathToSynapticsApp);
            StringCchCopy(g_asAppSettings.PathToSynapticsApp, ARRAYSIZE(g_asAppSettings.PathToSynapticsApp), buffer);
        }
    }

    // Relaunch delay
    nResult = GetDlgItemInt(hPropertyPage, IDC_EDIT_DELAY, &bSuccess, FALSE);
    if (bSuccess)
    {
        // Don't let user to make an invalid setting
        if (nResult < 0)
            nResult = defaultSettings.RelaunchDelay;
        g_asAppSettings.RelaunchDelay = nResult;
    }
}

BOOL ValidateGeneralSettings(HWND hDlg, HWND hPropertyPage)
{
    WCHAR buffer[MAX_SETTINGS_VALUE_SIZE];
    BOOL bSuccess, bInvalid;

    // Path to Synaptics app
    bInvalid = FALSE;
    if (GetDlgItemText(hPropertyPage, IDC_EDIT_FOLDER, buffer, ARRAYSIZE(buffer)))
    {
        size_t length;
        if (StringCchLength(buffer, ARRAYSIZE(buffer), &length) == S_OK)
        {
            if (length == 0) 
                bInvalid = TRUE;
        }
        else
        {
            bInvalid = TRUE;
        }
    }
    else
    {
        bInvalid = TRUE;
    }

    if (bInvalid)
    {
        MessageBox(hDlg, L"Path to Synaptics App is invalid. Please enter a valid path.", NULL, MB_OK | MB_ICONERROR);
        SetFocus(GetDlgItem(hPropertyPage, IDC_EDIT_FOLDER));
        return FALSE;
    }

    // Relaunch delay
    bInvalid = FALSE;
    UINT nResult = GetDlgItemInt(hPropertyPage, IDC_EDIT_DELAY, &bSuccess, FALSE);
    if (bSuccess)
    {
        if (nResult < 0) 
            bInvalid = TRUE;
    }
    else
    {
        bInvalid = TRUE;
    }

    if (bInvalid)
    {
        MessageBox(hDlg, L"Value for Relaunch Delay is invalid. Please enter a valid value.", NULL, MB_OK | MB_ICONERROR);
        SetFocus(GetDlgItem(hPropertyPage, IDC_EDIT_DELAY));
        return FALSE;
    }

    return TRUE;
}