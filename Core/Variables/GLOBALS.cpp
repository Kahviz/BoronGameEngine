#include "Globals.h"
#include <string>
#include <iostream>
#include "Logger/Logger.h"

int screen_width = 800;
int screen_height = 400;
float zFar = 1000.0f;
extern float viewport_width = 400.0f;
extern float viewport_height = 200.0f;

#include <cstdlib>

bool g_vSync = false; //Limit it to the monitors refresh rate, if true
bool g_Running = true;
bool g_Typing = false;
float g_FOV = 100.0f;
int g_Index = 0;

std::string GetAppDataPath() {
    char* buffer = nullptr;
    size_t size = 0;
    #ifdef _WIN32
        

        if (_dupenv_s(&buffer, &size, "APPDATA") == 0 && buffer != nullptr) {
            CreateSuccess("AppData Found!");
            std::string path(buffer);
            free(buffer);
            return path;
        }
        else {
            CreateError("AppData Not Found in GLOBALS.h with _WIN32");
            std::exit(10);
        }
    #else
        const char* homeDir = getenv("HOME");
        if (!homeDir) {
            MakeAError("Failed to get HOME directory");
            return "";
        }
        fs::path appDataTarget = fs::path(homeDir) / ".config" / "UntilitedGameEngine";

        std::cout << "HOME directory found: " << appDataTarget << "\n";

        return appDataTarget;
    #endif

    return "";
}

fs::path GetAppDataDir() {
    #ifdef _WIN32
        char* appDataPath = nullptr;
        size_t sz = 0;
        if (_dupenv_s(&appDataPath, &sz, "APPDATA") == 0 && appDataPath != nullptr) {
            fs::path appdatatarget = fs::path(appDataPath) / "BoronEngine";

            #ifdef _DEBUG
                std::cout << "APPDATA path found: " << appdatatarget << "\n";
            #endif // _DEBUG

            free(appDataPath);
            return appdatatarget;
        }
        else {
            CreateError("Failed to get APPDATA path");
            if (appDataPath) free(appDataPath);
            return fs::path();
        }
    #else
        const char* homeDir = getenv("HOME");
        if (homeDir) {
            fs::path appdatatarget = fs::path(homeDir) / ".config" / "BoronEngine";
            std::cout << "HOME directory found: " << appdatatarget << "\n";
            return appdatatarget;
        }
        else {
            MakeAError("Failed to get HOME directory");
            return fs::path();
        }
    #endif
}

std::string g_projectName = "ProjectTest1";
fs::path appData = GetAppDataPath();

fs::path BoronEnginePath = appData / "BoronEngine";
fs::path savings = BoronEnginePath / "Savings";
fs::path assets = BoronEnginePath / "Assets";
fs::path textures = BoronEnginePath / "Textures";
fs::path fonts = BoronEnginePath / "Fonts";
fs::path g_appDataTarget = GetAppDataDir();