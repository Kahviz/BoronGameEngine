#pragma once

#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include "ErrorHandling/ErrorMessage.h"
#include "BoronMathLibrary.h"

class Graphics;

class Window
{
public:
    Window(int Height, int Width, std::string Name);
    void SetWindowIcon(GLFWwindow* window);
    ~Window();
    BML::Vector2 GetSize() const noexcept;
    GLFWwindow* GetWindow() const noexcept;
    Graphics& GetGraphics();
private:
    bool Inited = false;
    GLFWwindow* m_window = nullptr;
    std::unique_ptr<Graphics> pGfx;
};
