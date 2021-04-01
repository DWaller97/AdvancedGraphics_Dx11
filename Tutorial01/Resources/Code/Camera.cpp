#include "Camera.h"
#include <d3d11.h>
#include "Utilities/structures.h"

using namespace DirectX;

float* Camera::moveSpeed = nullptr;

Camera::Camera(DirectX::XMFLOAT4& _eye, DirectX::XMFLOAT4& _at, DirectX::XMFLOAT4& _up, const int _width, const int _height)
{
    moveSpeed = new float();
    *moveSpeed = 250;
    eye = _eye;
    at = _at;
    up = _up;
    forward = XMFLOAT4(0, 0, 1, 0);
    eye.x = at.x;
    eye.y = at.y + 1;
    eye.z = at.z - 3;
    DirectX::XMVECTOR Eye = DirectX::XMLoadFloat4(&eye);
    DirectX::XMVECTOR At = DirectX::XMLoadFloat4(&at);
    DirectX::XMVECTOR Up = DirectX::XMLoadFloat4(&up);

    DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&view,  DirectX::XMMatrixLookAtLH(Eye, At, Up));
    DirectX::XMStoreFloat4x4(&proj,  DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, _width / (float)_height, 0.01f, 10000.0f));
    
    LookAt();

}

Camera::~Camera()
{
}

void Camera::LookAt()
{
    XMStoreFloat4x4(&view, XMMatrixLookAtLH(DirectX::XMLoadFloat4(&eye), DirectX::XMLoadFloat4(&at), DirectX::XMLoadFloat4(&up)));
}


void Camera::Strafe(float dist)
{
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(dist * strafeSpeed);
    DirectX::XMVECTOR r = DirectX::XMLoadFloat4(&right);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat4(&at);

    DirectX::XMStoreFloat4(&at, DirectX::XMVectorMultiplyAdd(s, r, p));
}

void Camera::Move(float dist)
{
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(dist * *moveSpeed);
    DirectX::XMVECTOR l = DirectX::XMLoadFloat4(&eye);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat4(&at);

    DirectX::XMStoreFloat4(&at, DirectX::XMVectorMultiplyAdd(s, l, p));


}

void Camera::MoveUp(float dist)
{
    at.y += (dist * *moveSpeed);
}

void Camera::Pitch(float angle)
{
    //TODO: Fix
    DirectX::XMMATRIX r = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat4(&right), angle * pitchSpeed);
    //DirectX::XMStoreFloat4(&at, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat4(&eye), r));
    //DirectX::XMStoreFloat4(&at, DirectX::XMVectorMultiplyAdd(r, DirectX::XMLoadFloat4(&eye), DirectX::XMLoadFloat4(&at)));


}

void Camera::RotateY(float angle)
{
    //DirectX::XMMATRIX r = DirectX::XMMatrixRotationY(angle * rotateYSpeed);
    //DirectX::XMStoreFloat4(&right, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat4(&right), r));

    //DirectX::XMStoreFloat4(&up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat4(&up), r));

    //DirectX::XMStoreFloat4(&eye, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat4(&eye), r));

    if (angle == 0)
        return;
    if (angle > 0) {
        if (rotation.y >= XM_PI * 2)
            rotation.y = 0;
    }
    else if (angle < 0) {
        if (rotation.y <= -(XM_PI * 2))
            rotation.y = 0;
    }

    MoveRotation(0, angle, 0);
    float rot = angle;
    SetForward((sin(rot)), 0, cos(rot));

}

void Camera::UpdateViewMatrix()
{
    DirectX::XMVECTOR r = DirectX::XMLoadFloat4(&right);
    DirectX::XMVECTOR u = DirectX::XMLoadFloat4(&up);
    DirectX::XMVECTOR e = DirectX::XMLoadFloat4(&eye);
    DirectX::XMVECTOR a = DirectX::XMLoadFloat4(&at);
    e = DirectX::XMVector3Normalize(e);
    u = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e, r));
    r = DirectX::XMVector3Cross(u, e);
    float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(a, r));
    float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(a, u));
    float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(a, e));

    DirectX::XMStoreFloat4(&right, r);
    DirectX::XMStoreFloat4(&up, u);
    DirectX::XMStoreFloat4(&eye, e);


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


}

void Camera::Update()
{
    DirectX::XMStoreFloat4x4(&world, XMMatrixTranspose(XMMatrixRotationX(rotation.x) * XMMatrixRotationY(rotation.y) * XMMatrixRotationZ(rotation.z) * XMMatrixTranslation(at.x, at.y, at.z)));
}

void Camera::SetPosition(float x, float y, float z)
{
    at.x = x;
    at.y = y;
    at.z = z;
    eye.x = x;
    eye.y = at.y + 1;
    eye.z = at.z - 3;
    LookAt();
}

void Camera::SetPosition(const DirectX::XMFLOAT4& v)
{
    DirectX::XMStoreFloat4(&at, DirectX::XMLoadFloat4(&v));
}

void Camera::SetRotation(float x, float y, float z)
{
    rotation.x = x;
    rotation.y = y;
    rotation.z = z;
}

void Camera::SetForward(float x, float y, float z)
{
    forward = { x, y, z, 1 };
}

void Camera::MovePosition(float x, float y, float z)
{
    at.x += (x * *moveSpeed);
    at.y += y * *moveSpeed;
    at.z += z * *moveSpeed;
    eye.x = at.x;
    eye.y = at.y + 1;
    eye.z = at.z - 3;
    LookAt();
}

void Camera::MoveRotation(float x, float y, float z)
{
    rotation.x = 10;
    rotation.y = 10;
    rotation.z = 10;
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