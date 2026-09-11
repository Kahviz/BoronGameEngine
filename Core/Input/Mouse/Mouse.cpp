#include "Mouse.h"

bool Mouse::m_firstmouse = true;
BML::Vec2 Mouse::m_lastMousePos{};
BML::Vec2 Mouse::m_mousePos{};
bool Mouse::m_leftClicked = false;
bool Mouse::m_rightClicked = false;

void Mouse::updateMouse(Window* window) {
    double mouseX;
    double mouseY;

    m_leftClicked = false;
    m_rightClicked = false;

    glfwGetCursorPos(window->GetWindow(), &mouseX, &mouseY);
    if (glfwGetMouseButton(window->GetWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        m_leftClicked = true;
    }

    if (glfwGetMouseButton(window->GetWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        m_rightClicked = true;
    }

    if (m_firstmouse)
    {
        m_lastMousePos.setX(static_cast<float>(mouseX));
        m_lastMousePos.setY(static_cast<float>(mouseY));

        m_mousePos = m_lastMousePos;
        m_firstmouse = false;
    }


    m_lastMousePos = m_mousePos;
    m_mousePos.setX(static_cast<float>(mouseX));
    m_mousePos.setY(static_cast<float>(mouseY));
}

BML::Vec2 Mouse::getMousePos() {
    return m_mousePos;
}

float Mouse::getMousePosX() {
    return m_mousePos.x();
}

float Mouse::getMousePosY() {
    return m_mousePos.y();
}

BML::Vec2 Mouse::getDelta() {
    return BML::Vec2(m_mousePos - m_lastMousePos);
}

float Mouse::getDeltaX()
{
    return m_mousePos.x() - m_lastMousePos.x();
}

float Mouse::getDeltaY()
{
    return m_mousePos.y() - m_lastMousePos.y();
}

bool Mouse::isLeftClicked() {
    return m_leftClicked;
}

bool Mouse::isRightClicked() {
    return m_rightClicked;
}
