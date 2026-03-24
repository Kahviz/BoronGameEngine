#include "Camera.h"
#include "GLOBALS.h"

Camera::Camera()
{
    this->pos = Vector3(0.0f, 0.0f, 0.0f);
    this->posVector = Vector3ToVector4(this->pos);
    this->rot = Vector3(0.0f, 0.0f, 0.0f);
    this->rotVector = Vector3ToVector4(this->rot);
    this->UpdateViewMatrix();
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
    float fovRadians = DegreesToRadians(fovDegrees);
    this->projectionMatrix = Matrix4x4PerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const Matrix4x4& Camera::GetViewMatrix() const { 
    return this->viewMatrix;
}
const Matrix4x4& Camera::GetProjectionMatrix() const { return this->projectionMatrix; }
const Vector4& Camera::GetPositionVector() const { return this->posVector; }
const Vector3& Camera::GetPositionVector3() const { return this->pos; }
const Vector4& Camera::GetRotationVector() const { return this->rotVector; }
const Vector3& Camera::GetRotationVector3() const { return this->rot; }

void Camera::SetPosition(const Vector4& pos)
{
    StoreVector3(&this->pos, pos);
    this->posVector = pos;
    this->UpdateViewMatrix();
}

void Camera::SetPosition(float x, float y, float z)
{
    this->pos = Vector3(x, y, z);
    this->posVector = Vector3ToVector4(this->pos);
    this->UpdateViewMatrix();
}

void Camera::AdjustPosition(const Vector4& pos)
{
    this->posVector.x() += pos.x();
    this->posVector.y() += pos.y();
    this->posVector.z() += pos.z();
    StoreVector3(&this->pos, this->posVector);
    this->UpdateViewMatrix();
}

void Camera::AdjustPosition(float x, float y, float z)
{
    this->pos.x() += x;
    this->pos.y() += y;
    this->pos.z() += z;
    this->posVector = Vector3ToVector4(this->pos);
    this->UpdateViewMatrix();
}

void Camera::SetRotation(const Vector4& rot)
{
    this->rotVector = rot;
    StoreVector3(&this->rot, rot);
    this->UpdateViewMatrix();
}

void Camera::SetRotation(float x, float y, float z)
{
    this->rot = Vector3(x, y, z);
    this->rotVector = Vector3ToVector4(this->rot);
    this->UpdateViewMatrix();
}

void Camera::AdjustRotation(const Vector4& rot)
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

Vector3 Camera::GetForward() const
{
    Matrix4x4 rotMatrix;
    rotMatrix.Matrix4x4RotationRollPitchYaw(rot.x(), rot.y(), rot.z());

    Vector3 forward3 = { DEFAULT_FORWARD_VECTOR.x(), DEFAULT_FORWARD_VECTOR.y(), DEFAULT_FORWARD_VECTOR.z() };
    Vector3 transformed = Vector3Transform(forward3, rotMatrix);

    Vector3 f;
    f = transformed;
    return f;
}

Vector3 Camera::GetRight() const
{
    Matrix4x4 rotMatrix;
    rotMatrix.Matrix4x4RotationRollPitchYaw(rot.x(), rot.y(), rot.z());

    Vector3 right3 = { DEFAULT_RIGHT_VECTOR.x(), DEFAULT_RIGHT_VECTOR.y(), DEFAULT_RIGHT_VECTOR.z() };
    Vector3 transformed = Vector3Transform(right3, rotMatrix);

    Vector3 r;
    r = transformed;
    return r;
}

void Camera::UpdateViewMatrix()
{
    Vector3 eye = pos;
    Vector3 target = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);

    Vector3 zaxis = target - eye;
    zaxis.normalize();  

    Vector3 xaxis = up.cross(zaxis);
    xaxis.normalize();

    Vector3 yaxis = zaxis.cross(xaxis);

    Matrix4x4 view;

    view(0, 0) = xaxis.x(); view(0, 1) = xaxis.y(); view(0, 2) = xaxis.z();
    view(0, 3) = -xaxis.dot(eye);

    view(1, 0) = yaxis.x(); view(1, 1) = yaxis.y(); view(1, 2) = yaxis.z();
    view(1, 3) = -yaxis.dot(eye);

    view(2, 0) = zaxis.x(); view(2, 1) = zaxis.y(); view(2, 2) = zaxis.z();
    view(2, 3) = -zaxis.dot(eye);

    view(3, 0) = 0.0f; view(3, 1) = 0.0f; view(3, 2) = 0.0f; view(3, 3) = 1.0f;

    this->viewMatrix = view;

    std::cout << "View Matrix:\n" << this->viewMatrix << std::endl;
}