#include "framework.h"
#include "globals.h"
#include "settings.h"

// Key Names
#define VALUE_NAME_PATH_TO_SYNAPTICS_APP L"PathToSynapticsApp"
#define VALUE_NAME_RELAUNCH_DELAY L"RelaunchDelay"

// Default Values
#define DEFAULT_VALUE_PATH_TO_SYNAPTICS_APP L"C:\\Program Files\\Synaptics\\SynTP\\"
#define DEFAULT_VALUE_APP_TO_RELAUNCH L"SynTPEnh.exe"
#define DEFAULT_VALUE_RELAUNCH_DELAY 1      // In seconds

// Registry Key structure
// NOTE: 
// If dwDataType is REG_SZ then szValue and szDefaultValue will be used.
// If dwDataType is REG_DWORD then dwValue and dwDefaultValue will be used.
typedef struct
{
    HKEY hKey;
    WCHAR szName[MAX_SETTINGS_VALUE_SIZE];
    DWORD dwDataType;
    WCHAR szValue[MAX_SETTINGS_VALUE_SIZE];
    WCHAR szDefaultValue[MAX_SETTINGS_VALUE_SIZE];
    DWORD dwValue;
    DWORD dwDefaultValue;
} REGISTRY_NAMED_VALUE, *PREGISTRY_NAMED_VALUE;

// Forward declarations of functions (keeping them private to this module)
BOOL LoadSettingsFromRegistry(PAPP_SETTINGS);
BOOL SaveSettingsToRegistry(APP_SETTINGS);
void GetRegistryKeyPath(STRSAFE_LPWSTR, size_t);
BOOL OpenRegistryKeyPath(PHKEY, LPCWSTR, BOOL);
BOOL WriteRegistryNamedValue(REGISTRY_NAMED_VALUE, BOOL);
LSTATUS ReadRegistryNamedValue(PREGISTRY_NAMED_VALUE, BOOL);
BOOL ReadRegistryNamedValueOrWriteDefault(PREGISTRY_NAMED_VALUE, BOOL);

// ===============================================================
BOOL LoadSettings(PAPP_SETTINGS pAppSettings)
{
    LoadDefaultSettings(pAppSettings);

    switch (pAppSettings->SettingsLocation)
    {
    case SETTINGS_IN_REGISTRY:
        return LoadSettingsFromRegistry(pAppSettings);
        break;
    default:
        // Unknown location
        return FALSE;
        break;
    }

    return TRUE;
}

BOOL SaveSettings(APP_SETTINGS appSettings)
{
    switch (appSettings.SettingsLocation)
    {
    case SETTINGS_IN_REGISTRY:
        return SaveSettingsToRegistry(appSettings);
        break;
    default:
        // Unknown location
        return FALSE;
        break;
    }

    return TRUE;
}

BOOL LoadSettingsFromRegistry(PAPP_SETTINGS pAppSettings)
{
    WCHAR keyPath[MAX_SETTINGS_VALUE_SIZE];
    REGISTRY_NAMED_VALUE namedValue;
    APP_SETTINGS defaultSettings;

    // Get default settings
    LoadDefaultSettings(&defaultSettings);

    // Prepare path
    GetRegistryKeyPath(keyPath, ARRAYSIZE(keyPath));

    // Open registry
    ZeroMemory(&namedValue, sizeof(REGISTRY_NAMED_VALUE));
    if (!OpenRegistryKeyPath(&namedValue.hKey, keyPath, FALSE))
        return FALSE;

    // Path to Synaptics App
    namedValue.dwDataType = REG_SZ;
    StringCchCopy(namedValue.szName, ARRAYSIZE(namedValue.szName), VALUE_NAME_PATH_TO_SYNAPTICS_APP);
    StringCchCopy(namedValue.szDefaultValue, ARRAYSIZE(namedValue.szDefaultValue), defaultSettings.PathToSynapticsApp);
    if (!ReadRegistryNamedValueOrWriteDefault(&namedValue, FALSE))
        return FALSE;
    StringCchCopy(pAppSettings->PathToSynapticsApp, ARRAYSIZE(pAppSettings->PathToSynapticsApp), namedValue.szValue);

    // Relaunch delay
    namedValue.dwDataType = REG_DWORD;
    StringCchCopy(namedValue.szName, ARRAYSIZE(namedValue.szName), VALUE_NAME_RELAUNCH_DELAY);
    namedValue.dwDefaultValue = defaultSettings.RelaunchDelay;
    if (!ReadRegistryNamedValueOrWriteDefault(&namedValue, FALSE))
        return FALSE;
    pAppSettings->RelaunchDelay = namedValue.dwValue;

    // Free-up system resources
    RegCloseKey(namedValue.hKey);

    return TRUE;
}

BOOL SaveSettingsToRegistry(APP_SETTINGS appSettings)
{
    WCHAR keyPath[MAX_SETTINGS_VALUE_SIZE];
    REGISTRY_NAMED_VALUE namedValue;

    // Prepare path
    GetRegistryKeyPath(keyPath, ARRAYSIZE(keyPath));

    // Open registry
    ZeroMemory(&namedValue, sizeof(REGISTRY_NAMED_VALUE));
    if (!OpenRegistryKeyPath(&namedValue.hKey, keyPath, FALSE))
        return FALSE;

    // Path to Synaptics App
    namedValue.dwDataType = REG_SZ;
    StringCchCopy(namedValue.szName, ARRAYSIZE(namedValue.szName), VALUE_NAME_PATH_TO_SYNAPTICS_APP);
    StringCchCopy(namedValue.szValue, ARRAYSIZE(namedValue.szValue), appSettings.PathToSynapticsApp);
    if (!WriteRegistryNamedValue(namedValue, FALSE))
        return FALSE;

    // Relaunch delay
    namedValue.dwDataType = REG_DWORD;
    StringCchCopy(namedValue.szName, ARRAYSIZE(namedValue.szName), VALUE_NAME_RELAUNCH_DELAY);
    namedValue.dwValue = appSettings.RelaunchDelay;
    if (!WriteRegistryNamedValue(namedValue, FALSE))
        return FALSE;

    // Free-up system resources
    RegCloseKey(namedValue.hKey);

    return TRUE;
}

void LoadDefaultSettings(PAPP_SETTINGS pAppSettings)
{
    StringCchCopy(
        pAppSettings->PathToSynapticsApp,
        ARRAYSIZE(pAppSettings->PathToSynapticsApp),
        DEFAULT_VALUE_PATH_TO_SYNAPTICS_APP
    );

    StringCchCopy(
        pAppSettings->AppToRelaunch,
        ARRAYSIZE(pAppSettings->AppToRelaunch),
        DEFAULT_VALUE_APP_TO_RELAUNCH
    );

    pAppSettings->RelaunchDelay = DEFAULT_VALUE_RELAUNCH_DELAY;
}

void GetRegistryKeyPath(STRSAFE_LPWSTR outBuffer, size_t bufferSize)
{
    StringCchCopy(outBuffer, bufferSize, L"SOFTWARE\\");
    StringCchCat(outBuffer, bufferSize, AUTHOR_ID);
    StringCchCat(outBuffer, bufferSize, L"\\");
    StringCchCat(outBuffer, bufferSize, APP_ID);
    StringCchCat(outBuffer, bufferSize, L"\\General");
}

BOOL OpenRegistryKeyPath(PHKEY hKey, LPCWSTR lpKeyPath, BOOL bSilent)
{
    LSTATUS lReturn = RegCreateKeyEx(
        HKEY_CURRENT_USER,
        lpKeyPath,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,
        hKey,
        NULL
    );

    if (lReturn != ERROR_SUCCESS)
    {
        if (!bSilent)
            MessageBox(0, L"Error opening Windows Registry handle", NULL, 0);
        return FALSE;
    }

    return TRUE;
}

BOOL WriteRegistryNamedValue(REGISTRY_NAMED_VALUE regNamedValue, BOOL bSilent)
{
    PVOID pBufferPtr = NULL;
    DWORD bufferSize = 0;

    switch (regNamedValue.dwDataType)
    {
    case REG_SZ:
        pBufferPtr = &regNamedValue.szValue;
        bufferSize = ARRAYSIZE(regNamedValue.szValue);
        break;
    case REG_DWORD:
        pBufferPtr = &regNamedValue.dwValue;
        bufferSize = sizeof(regNamedValue.dwValue);
        break;
    }

    LSTATUS lReturn = RegSetValueEx(
        regNamedValue.hKey,
        regNamedValue.szName,
        0,
        regNamedValue.dwDataType,
        (LPBYTE)(pBufferPtr),
        bufferSize
    );

    if (lReturn != ERROR_SUCCESS)
    {
        if (!bSilent)
            MessageBox(0, L"Error writing default settings to Windows Registry", NULL, 0);
        return FALSE;
    }

    return TRUE;
}

LSTATUS ReadRegistryNamedValue(PREGISTRY_NAMED_VALUE pRegNamedValue, BOOL bSilent)
{
    WCHAR szBuffer[MAX_SETTINGS_VALUE_SIZE];
    DWORD receivedDataType, bufferSize, flags, dwBuffer;
    PVOID pBufferPtr;

    // Determine what kind of buffer we would need
    switch (pRegNamedValue->dwDataType)
    {
    case REG_SZ:
        flags = RRF_RT_REG_SZ;
        pBufferPtr = &szBuffer;
        bufferSize = ARRAYSIZE(szBuffer);
        break;
    case REG_DWORD:
        flags = RRF_RT_DWORD;
        pBufferPtr = &dwBuffer;
        bufferSize = sizeof(dwBuffer);
        break;
    default: 
        MessageBox(0, L"Invalid registry value type", NULL, 0);
        return FALSE;
    }

    // Read named-value from registry
    LSTATUS lReturn = RegGetValue(
        pRegNamedValue->hKey,
        NULL,
        pRegNamedValue->szName,
        flags,
        &receivedDataType,
        pBufferPtr,
        &bufferSize
    );

    // Named-Value doesn't exist
    if (lReturn == ERROR_FILE_NOT_FOUND)
        return lReturn;

    // Why we got error? Is the buffer too small or something else went wrong?
    if (lReturn != ERROR_SUCCESS)
    {
        if (!bSilent)
            MessageBox(0, L"Error reading settings from Windows Registry", NULL, 0);
        return lReturn;
    }

    // Return the result from the relevant buffer
    switch (pRegNamedValue->dwDataType)
    {
    case REG_SZ:
        StringCchCopy(pRegNamedValue->szValue, ARRAYSIZE(pRegNamedValue->szValue), szBuffer);
        break;
    case REG_DWORD:
        pRegNamedValue->dwValue = dwBuffer;
        break;
    }

    return lReturn;
}

BOOL ReadRegistryNamedValueOrWriteDefault(PREGISTRY_NAMED_VALUE pRegNamedValue, BOOL bSilent)
{
    LSTATUS lReturn = ReadRegistryNamedValue(pRegNamedValue, bSilent);

    // If Named-Value doesn't exist then create it.
    if (lReturn == ERROR_FILE_NOT_FOUND)
    {
        switch (pRegNamedValue->dwDataType)
        {
        case REG_DWORD:
            pRegNamedValue->dwValue = pRegNamedValue->dwDefaultValue;
            break;
        case REG_SZ:
            StringCchCopy(pRegNamedValue->szValue, ARRAYSIZE(pRegNamedValue->szValue), pRegNamedValue->szDefaultValue);
            break;
        }
        if (!WriteRegistryNamedValue(*pRegNamedValue, bSilent))
            return FALSE;
    }

    return TRUE;
}
