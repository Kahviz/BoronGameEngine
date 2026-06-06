#pragma once
#include "BoronMathLibrary.h"

class Camera
{
public:
    Camera();
    void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

    const BML::Matrix4x4& GetViewMatrix() const;
    const BML::Matrix4x4& GetProjectionMatrix() const;

    const BML::Vector4& GetPositionVector() const;
    const BML::Vector3& GetPositionVector3() const;
    const BML::Vector4& GetRotationVector() const;
    const BML::Vector3& GetRotationVector3() const;

    void SetPosition(const BML::Vector4& pos);
    void SetPosition(float x, float y, float z);
    void AdjustPosition(const BML::Vector4& pos);
    void AdjustPosition(float x, float y, float z);
    void SetRotation(const BML::Vector4& rot);
    void SetRotation(float x, float y, float z);
    void AdjustRotation(const BML::Vector4& rot);
    void AdjustRotation(float x, float y, float z);

    BML::Vector3 GetForward() const;
    BML::Vector3 GetRight() const;
    BML::Vector3 GetUp() const;
    void UpdateViewMatrix();

private:
    BML::Vector4 posVector;
    BML::Vector4 rotVector;
    BML::Vector3 pos;
    BML::Vector3 rot;
    BML::Matrix4x4 viewMatrix;
    BML::Matrix4x4 projectionMatrix;

    const BML::Vector3 DEFAULT_RIGHT_VECTOR = { 1.0f, 0.0f, 0.0f };
    const BML::Vector3 DEFAULT_FORWARD_VECTOR = { 0.0f, 0.0f, 1.0f };
    const BML::Vector3 DEFAULT_UP_VECTOR = { 0.0f, 1.0f, 0.0f };
};