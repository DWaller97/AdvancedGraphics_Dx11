#include "Camera.h"
#include <d3d11.h>
#include "structures.h"

using namespace DirectX;

Camera::Camera(DirectX::XMFLOAT3& _eye, DirectX::XMFLOAT3& _at, DirectX::XMFLOAT3& _up, const int _width, const int _height)
{
    eye = _eye;
    at = _at;
    up = _up;

    DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
    DirectX::XMVECTOR At = DirectX::XMLoadFloat3(&at);
    DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);

    DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&view,  DirectX::XMMatrixLookAtLH(Eye, At, Up));
    DirectX::XMStoreFloat4x4(&proj,  DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, _width / (float)_height, 0.01f, 100.0f));
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
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(dist * strafeSpeed);
    DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&right);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&at);

    DirectX::XMStoreFloat3(&at, DirectX::XMVectorMultiplyAdd(s, r, p));
}

void Camera::Move(float dist)
{
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(dist * moveSpeed);
    DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&eye);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&at);

    DirectX::XMStoreFloat3(&at, DirectX::XMVectorMultiplyAdd(s, l, p));

}

void Camera::MoveUp(float dist)
{
    at.y += (dist * moveSpeed);

}

void Camera::Pitch(float angle)
{
    DirectX::XMMATRIX r = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&right), angle * pitchSpeed);
    DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up), r));
    DirectX::XMStoreFloat3(&eye, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&eye), r));
}

void Camera::RotateY(float angle)
{
    DirectX::XMMATRIX r = DirectX::XMMatrixRotationY(angle * rotateYSpeed);
    DirectX::XMStoreFloat3(&right, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&right), r));

    DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up), r));

    DirectX::XMStoreFloat3(&eye, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&eye), r));


}

void Camera::UpdateViewMatrix()
{
    DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&right);
    DirectX::XMVECTOR u = DirectX::XMLoadFloat3(&up);
    DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&eye);
    DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&at);
    e = DirectX::XMVector3Normalize(e);
    u = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e, r));
    r = DirectX::XMVector3Cross(u, e);
    float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(a, r));
    float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(a, u));
    float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(a, e));

    DirectX::XMStoreFloat3(&right, r);
    DirectX::XMStoreFloat3(&up, u);
    DirectX::XMStoreFloat3(&eye, e);


    view(0, 0) = right.x;
    view(1, 0) = right.y;
    view(2, 0) = right.z;
    view(3, 0) = x;

    view(0, 1) = up.x;
    view(1, 1) = up.y;
    view(2, 1) = up.z;
    view(3, 1) = y;

    view(0, 2) = eye.x;
    view(1, 2) = eye.y;
    view(2, 2) = eye.z;
    view(3, 2) = z;

    view(0, 3) = 0.0f;
    view(1, 3) = 0.0f;
    view(2, 3) = 0.0f;
    view(3, 3) = 1.0f;



    //DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
    //DirectX::XMVECTOR At = DirectX::XMLoadFloat3(&at);
    //DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
    //DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixLookAtLH(, Eye, Up));

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

void Camera::SetFrustum(float _fovY, float _aspect, float _nearZ, float _farZ)
{
    fovY = _fovY;
    aspect = _aspect;
    nearZ = _nearZ;
    farZ = _farZ;

    DirectX::XMMATRIX pers = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    DirectX::XMStoreFloat4x4(&proj, pers);
}