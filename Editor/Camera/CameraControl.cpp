#include "CameraControl.h"
#include <string>
#include "GLOBALS.h"

#include <GLFW/glfw3.h>

#include "Keyboard/Keyboard.h"
#include "Mouse/Mouse.h"

void CameraControl::MakeCameraControls(Window& wnd, float deltaTime)
{
    float speed = 5.0f * deltaTime;
    float sensitivity = 0.002f;

    Camera& Cam = wnd.GetGraphics().GetCamera();
    BML::Vector3 forward = Cam.GetForward();
    BML::Vector3 right = Cam.GetRight();

    GLFWwindow* glfwWND = wnd.GetWindow();

    if (Keyboard::isHeld(glfwWND, Boron::Keys::LeftShift))
    {
        speed = 100.0f * deltaTime;
    }

    if (Keyboard::isHeld(glfwWND,Boron::Keys::A))
        Cam.AdjustPosition(-right.x() * speed, -right.y() * speed, -right.z() * speed);
    
    if (Keyboard::isHeld(glfwWND, Boron::Keys::D))
        Cam.AdjustPosition(right.x() * speed, right.y() * speed, right.z() * speed);
    if (Keyboard::isHeld(glfwWND, Boron::Keys::S))
        Cam.AdjustPosition(-forward.x() * speed, -forward.y() * speed, -forward.z() * speed);

    if (Keyboard::isHeld(glfwWND, Boron::Keys::W))
        Cam.AdjustPosition(forward.x() * speed, forward.y() * speed, forward.z() * speed);

    if (Keyboard::isHeld(glfwWND, Boron::Keys::Q))
        Cam.AdjustPosition(0.0f, -speed, 0.0f);

    if (Keyboard::isHeld(glfwWND, Boron::Keys::E))
        Cam.AdjustPosition(0.0f, speed, 0.0f);

    if (glfwGetMouseButton(glfwWND, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        float x = -Mouse::GetDeltaY() * sensitivity;
        float y = Mouse::GetDeltaX() * sensitivity;

        Cam.SetRotationX(std::clamp(Cam.GetRotationVector().x() + x, DegreesToRadians(-89.9f), DegreesToRadians(89.9f)));
        Cam.AdjustRotation(0.0f, y, 0.0f);
    }

}

BML::Matrix4x4 CameraControl::GetViewMatrix(const Camera& cam)
{
    return cam.GetViewMatrix();
}

BML::Matrix4x4 CameraControl::GetProjectionMatrix(const Camera& cam, float aspectRatio)
{
    return cam.GetProjectionMatrix();
}