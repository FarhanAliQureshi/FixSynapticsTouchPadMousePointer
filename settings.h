#pragma once

// Constants to use in APP_SETTINGS.SettingsLocation
#define SETTINGS_IN_REGISTRY 1              // Currently implemented, rest are not implemented yet
#define SETTINGS_IN_USER_PROFILE 2          // Not yet implemented, probably INI file
#define SETTINGS_IN_ALL_USERS_PROFILE 3     // Not yet implemented, probably INI file
#define SETTINGS_IN_USER_DOCUMENTS 4        // Not yet implemented, probably INI file

#define MAX_SETTINGS_VALUE_SIZE MAX_PATH

typedef struct
{
    WCHAR PathToSynapticsApp[MAX_SETTINGS_VALUE_SIZE];
    WCHAR AppToRelaunch[MAX_SETTINGS_VALUE_SIZE];
    UINT RelaunchDelay;
    UINT SettingsLocation;
} APP_SETTINGS, *PAPP_SETTINGS;

// Forward declarations of functions
BOOL LoadSettings(PAPP_SETTINGS);
BOOL SaveSettings(APP_SETTINGS);
void LoadDefaultSettings(PAPP_SETTINGS);