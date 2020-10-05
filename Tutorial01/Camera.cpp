#include "Camera.h"

Camera::Camera(DirectX::XMFLOAT3& _eye, DirectX::XMFLOAT3& _at, DirectX::XMFLOAT3& _up, const int _width, const int _height)
{
    world = DirectX::XMMatrixIdentity();
    eye = _eye;
    at = _at;
    up = _up;

    DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
    DirectX::XMVECTOR At = DirectX::XMLoadFloat3(&at);
    DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
    view = DirectX::XMMatrixLookAtLH(Eye, At, Up);
    proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, _width / (float)_height, 0.01f, 100.0f);
}

Camera::~Camera()
{
}

void Camera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp)
{
}

void Camera::LookAt(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 worldUp)
{
}


void Camera::Strafe(float dist)
{
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(dist);
    DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&right);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&at);

    DirectX::XMStoreFloat3(&at, DirectX::XMVectorMultiplyAdd(s, r, p));
}

void Camera::Move(float dist)
{
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(dist);
    DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&eye);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&at);

    DirectX::XMStoreFloat3(&at, DirectX::XMVectorMultiplyAdd(s, l, p));

}

void Camera::Pitch(float angle)
{
    DirectX::XMMATRIX r = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&right), angle);
    DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up), r));
    DirectX::XMStoreFloat3(&eye, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&eye), r));
}

void Camera::RotateY(float angle)
{
    DirectX::XMMATRIX r = DirectX::XMMatrixRotationY(angle);
    DirectX::XMStoreFloat3(&right, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&right), r));

    DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up), r));

    DirectX::XMStoreFloat3(&eye, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&eye), r));


}

void Camera::UpdateViewMatrix()
{
    DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&right);
    DirectX::XMVECTOR u = DirectX::XMLoadFloat3(&up);
    DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&eye);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&at);
    l = DirectX::XMVector3Normalize(l);
    u = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(l, r));
    r = DirectX::XMVector3Cross(u, l);
    float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(p, r));
    float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(p, u));
    float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(p, l));

    DirectX::XMStoreFloat3(&right, r);
    DirectX::XMStoreFloat3(&up, u);
    DirectX::XMStoreFloat3(&eye, l);

    DirectX::XMFLOAT4X4 _view;
    DirectX::XMStoreFloat4x4(&_view, view);

    _view(0, 0) = right.x;
    _view(1, 0) = right.y;
    _view(2, 0) = right.z;
    _view(3, 0) = x;

    _view(0, 1) = up.x;
    _view(1, 1) = up.y;
    _view(2, 1) = up.z;
    _view(3, 1) = y;

    _view(0, 2) = eye.x;
    _view(1, 2) = eye.y;
    _view(2, 2) = eye.z;
    _view(3, 2) = z;

    _view(0, 3) = 0.0f;
    _view(1, 3) = 0.0f;
    _view(2, 3) = 0.0f;
    _view(3, 3) = 1.0f;

    view = DirectX::XMLoadFloat4x4(&_view);
}

void Camera::SetPosition(float x, float y, float z)
{
    at.x = x;
    at.y = y;
    at.z = z;
}

void Camera::SetPosition(const DirectX::XMFLOAT3& v)
{
    DirectX::XMStoreFloat3(&at, DirectX::XMLoadFloat3(&v));
}
