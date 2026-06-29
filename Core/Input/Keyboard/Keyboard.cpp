#include "Keyboard.h"

bool Keyboard::isHeld(GLFWwindow* window, Boron::Keys key)
{
    return glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS;
}