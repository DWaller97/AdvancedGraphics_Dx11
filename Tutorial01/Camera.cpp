#include "Camera.h"

void Camera::Strafe(float dist)
{
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(dist);
    DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&right);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&position);

    DirectX::XMStoreFloat3(&position, DirectX::XMVectorMultiplyAdd(s, r, p));
}

void Camera::Move(float dist)
{
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(dist);
    DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&look);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&position);

    DirectX::XMStoreFloat3(&position, DirectX::XMVectorMultiplyAdd(s, l, p));

}

void Camera::Pitch(float angle)
{
    DirectX::XMMATRIX r = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&right), angle);
    DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up), r));
    DirectX::XMStoreFloat3(&look, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&look), r));
}

void Camera::RotateY(float angle)
{
    DirectX::XMMATRIX r = DirectX::XMMatrixRotationY(angle);
    DirectX::XMStoreFloat3(&right, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&right), r));

    DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up), r));

    DirectX::XMStoreFloat3(&look, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&look), r));


}

void Camera::UpdateViewMatrix()
{
    DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&right);
    DirectX::XMVECTOR u = DirectX::XMLoadFloat3(&up);
    DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&look);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&position);
    l = DirectX::XMVector3Normalize(l);
    u = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(l, r));
    r = DirectX::XMVector3Cross(u, l);
    float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(p, r));
    float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(p, u));
    float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(p, l));

    DirectX::XMStoreFloat3(&right, r);
    DirectX::XMStoreFloat3(&up, u);
    DirectX::XMStoreFloat3(&look, l);

    view(0, 0) = right.x;
    view(1, 0) = right.y;
    view(2, 0) = right.z;
    view(3, 0) = x;

    view(0, 1) = up.x;
    view(1, 1) = up.y;
    view(2, 1) = up.z;
    view(3, 1) = y;

    view(0, 2) = look.x;
    view(1, 2) = look.y;
    view(2, 2) = look.z;
    view(3, 2) = z;

    view(0, 3) = 0.0f;
    view(1, 3) = 0.0f;
    view(2, 3) = 0.0f;
    view(3, 3) = 1.0f;
}

void Camera::SetPosition(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;
}

void Camera::SetPosition(const DirectX::XMFLOAT3& v)
{
    DirectX::XMStoreFloat3(&position, DirectX::XMLoadFloat3(&v));
}
