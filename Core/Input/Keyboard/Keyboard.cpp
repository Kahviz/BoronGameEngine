#include "Keyboard.h"

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

        glfwSetKeyCallback(window, keyCallback);
    }
}
bool Keyboard::isHeld(GLFWwindow* window, Boron::Keys key)
{
    return glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS;
}

Boron::Keys Keyboard::getLastPressedKey(GLFWwindow* window)
{
    return lastKey;
}