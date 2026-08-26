#include "Keyboard.h"
#include "ErrorHandling/ErrorMessage.h"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        Keyboard::lastKey = static_cast<Boron::Keys>(key);
    }
}
void Keyboard::Init(GLFWwindow* window) {
    if (!inited) {
        inited = true;
    }
}
bool Keyboard::isHeld(GLFWwindow* window, Boron::Keys key)
{
    if (!inited) {
        CreateError("Keyboard class not initiliazed!");
        return false; 
    }
    return glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS;
}

Boron::Keys Keyboard::getLastPressedKey(GLFWwindow* window)
{
    if (!inited) {
        CreateError("Keyboard class not initiliazed!");
        return {};
    }
    return lastKey;
}