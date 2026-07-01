#include "Mouse.h"

bool Mouse::s_firstmouse = true;
BML::Vec2 Mouse::s_lastMousePos{};
BML::Vec2 Mouse::s_mousePos{};

void Mouse::updateMouse(Window* window) {
    double mouseX;
    double mouseY;

    glfwGetCursorPos(window->GetWindow(), &mouseX, &mouseY);

    if (s_firstmouse)
    {
        s_lastMousePos.setX(static_cast<float>(mouseX));
        s_lastMousePos.setY(static_cast<float>(mouseY));

        s_mousePos = s_lastMousePos;
        s_firstmouse = false;
    }

    s_lastMousePos = s_mousePos;
    s_mousePos.setX(static_cast<float>(mouseX));
    s_mousePos.setY(static_cast<float>(mouseY));
}

BML::Vec2 Mouse::getMousePos() {
    return s_mousePos;
}

BML::Vec2 Mouse::GetDelta() {
    return BML::Vec2(s_mousePos - s_lastMousePos);
}

float Mouse::GetDeltaX()
{
    return s_mousePos.x() - s_lastMousePos.x();
}

float Mouse::GetDeltaY()
{
    return s_mousePos.y() - s_lastMousePos.y();
}
