#pragma once
#include "GLOBALS.h"
#include "Enums.h"
#include <unordered_map>

class Keyboard
{
public:
    static void Init(GLFWwindow* window);
    static bool isHeld(GLFWwindow* window, Boron::Keys key);
    static Boron::Keys getLastPressedKey(GLFWwindow* window);
    inline static Boron::Keys lastKey = static_cast<Boron::Keys>(0);
private:
    inline static bool inited = false;
};