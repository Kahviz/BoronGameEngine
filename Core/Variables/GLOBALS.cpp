// Globals.cpp
#include "Globals.h"
#include <cstdlib>
#include <string>
#include <iostream>

int screen_width = 800;
int screen_height = 400;

extern int viewport_width = 400;
extern int viewport_height = 200;

bool InEditor = true;
bool vSync = true;
bool Running = false;
float FOV = 100.0f;

int Index = 0;

std::string GetAppDataPath() {
    char* buffer = nullptr;
    size_t size = 0;

    if (_dupenv_s(&buffer, &size, "APPDATA") == 0 && buffer != nullptr) {
        std::string path(buffer);
        free(buffer);
        return path;
    }
    return "";
}

std::string ProjectName = "ProjectTest1";
std::string appData = GetAppDataPath();
std::string assets = appData + "\\UntilitedGameEngine\\Assets";
std::string fonts = appData + "\\UntilitedGameEngine\\Fonts";
