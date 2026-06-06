#include "Camera.h"
#include "GLOBALS.h"

Camera::Camera()
{
    this->pos = BML::Vector3(0.0f, 0.0f, 0.0f);
    this->posVector = Vector3ToVector4(this->pos);
    this->rot = BML::Vector3(0.0f, 0.0f, 0.0f);
    this->rotVector = Vector3ToVector4(this->rot);
    this->UpdateViewMatrix();
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
    float fovRadians = DegreesToRadians(fovDegrees);
    this->projectionMatrix = Matrix4x4PerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const BML::Matrix4x4& Camera::GetViewMatrix() const {
    return this->viewMatrix;
}
const BML::Matrix4x4& Camera::GetProjectionMatrix() const { return this->projectionMatrix; }
const BML::Vector4& Camera::GetPositionVector() const { return this->posVector; }
const BML::Vector3& Camera::GetPositionVector3() const { return this->pos; }
const BML::Vector4& Camera::GetRotationVector() const { return this->rotVector; }
const BML::Vector3& Camera::GetRotationVector3() const { return this->rot; }

void Camera::SetPosition(const BML::Vector4& pos)
{
    StoreVector3(&this->pos, pos);
    this->posVector = pos;
    this->UpdateViewMatrix();
}

void Camera::SetPosition(float x, float y, float z)
{
    this->pos = BML::Vector3(x, y, z);
    this->posVector = Vector3ToVector4(this->pos);
    this->UpdateViewMatrix();
}

void Camera::AdjustPosition(const BML::Vector4& pos)
{
    this->posVector.x() += pos.x();
    this->posVector.y() += pos.y();
    this->posVector.z() += pos.z();
    StoreVector3(&this->pos, this->posVector);
    this->UpdateViewMatrix();
}

void Camera::AdjustPosition(float x, float y, float z)
{
    BML::Vector3 vec3 = { x,y,z };

    this->pos += vec3;

    this->posVector = Vector3ToVector4(this->pos);
    this->UpdateViewMatrix();
}

void Camera::SetRotation(const BML::Vector4& rot)
{
    this->rotVector = rot;
    StoreVector3(&this->rot, rot);
    this->UpdateViewMatrix();
}

void Camera::SetRotation(float x, float y, float z)
{
    this->rot = BML::Vector3(x, y, z);
    this->rotVector = Vector3ToVector4(this->rot);
    this->UpdateViewMatrix();
}

void Camera::AdjustRotation(const BML::Vector4& rot)
{
    this->rotVector.x() += rot.x();
    this->rotVector.y() += rot.y();
    this->rotVector.z() += rot.z();
    StoreVector3(&this->rot, this->rotVector);
    this->UpdateViewMatrix();
}

void Camera::AdjustRotation(float x, float y, float z)
{
    this->rot.x() += x;
    this->rot.y() += y;
    this->rot.z() += z;
    this->rotVector = Vector3ToVector4(this->rot);
    this->UpdateViewMatrix();
}

BML::Vector3 Camera::GetForward() const
{
    float pitch = rot.x();  // X
    float yaw = rot.y();    // Y

    BML::Vector3 forward;
    forward.x() = sin(yaw) * cos(pitch);
    forward.y() = sin(pitch);
    forward.z() = cos(yaw) * cos(pitch);

    forward.normalize();

    return forward;
}

BML::Vector3 Camera::GetRight() const
{
    float yaw = rot.y();

    BML::Vector3 right;
    right.x() = cos(yaw);
    right.y() = 0.0f;
    right.z() = -sin(yaw);

    right.normalize();

    return right;
}

BML::Vector3 Camera::GetUp() const
{
    return BML::Vector3(0.0f, 1.0f, 0.0f);
}

void Camera::UpdateViewMatrix()
{
    BML::Vector3 eye = pos;
    BML::Vector3 forwardDir = GetForward();
    BML::Vector3 target = eye + forwardDir;
    BML::Vector3 up = GetUp();

    BML::Vector3 zaxis = (target - eye);
    zaxis.normalize();

    BML::Vector3 xaxis = up.cross(zaxis);
    xaxis.normalize();

    BML::Vector3 yaxis = zaxis.cross(xaxis);

    BML::Matrix4x4 view;

    view(0, 0) = xaxis.x(); view(0, 1) = xaxis.y(); view(0, 2) = xaxis.z();
    view(0, 3) = -xaxis.dot(eye);

    view(1, 0) = yaxis.x(); view(1, 1) = yaxis.y(); view(1, 2) = yaxis.z();
    view(1, 3) = -yaxis.dot(eye);

    view(2, 0) = zaxis.x(); view(2, 1) = zaxis.y(); view(2, 2) = zaxis.z();
    view(2, 3) = -zaxis.dot(eye);

    view(3, 0) = 0.0f; view(3, 1) = 0.0f; view(3, 2) = 0.0f; view(3, 3) = 1.0f;

    this->viewMatrix = view;
}