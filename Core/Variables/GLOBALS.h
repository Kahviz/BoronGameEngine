#pragma once

#include <unordered_map>

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

#define INEDITOR 1

#define DIRECTX11 0
#define VULKAN 1

#define BGE_VERSION 1.2
#define DEBUGFILEMAKING 0
#define PRINTDEBUGINFO 1

#define VALIDATIONLAYERS 1

#if INEDITOR == 1
	#define PROFILER
#endif

#if VULKAN == 1
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#endif

#include <GLFW/glfw3.h>

#define AURA

extern int screen_width;
extern int screen_height;
extern float zFar;
extern float viewport_width;
extern float viewport_height;

extern std::string appData;
extern std::string fonts;
extern std::string assets;
extern std::string textures;
extern std::string savings;
extern std::string g_projectName;

extern fs::path g_appDataTarget;

extern int g_Index;
extern bool g_vSync;
extern bool g_Running;
extern bool g_Typing;
extern float g_FOV;