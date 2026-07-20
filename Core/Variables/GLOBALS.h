#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

#define INEDITOR 1 //Use 1 For Editing 0 For The Release

#define DIRECTX11 0 //1 = True, 0 = False
#define VULKAN 1 //1 = True, 0 = False

#define VALIDATIONLAYERS 0; //If in debug and VALIDATIONLAYERS = 1 then true else false
#define BGE_VERSION 1.2; //BoronGameEngine Version
#define DEBUGFILEMAKING 0 //1 = True, 0 = False
#define PRINTDEBUGINFO 1  //1 = True, 0 = False

#if !defined(_DEBUG)
	#undef VALIDATIONLAYERS
#endif
#if VULKAN == 0
	#undef VALIDATIONLAYERS
#endif

#if INEDITOR == 1
	#define PROFILER
#endif

#if VULKAN == 1
	#ifndef GLFW_INCLUDE_VULKAN
	#define GLFW_INCLUDE_VULKAN
	#endif
#endif

#include <GLFW/glfw3.h>

#define AURA // Most Useful thing in this project

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

extern fs::path appDataTarget;

extern int Index;
extern bool vSync;
extern bool Running;
extern bool Typing;
extern float FOV;