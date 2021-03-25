#pragma once
#include <directxmath.h>
#include <d3d11.h>
#include <list>
#include <stdio.h>

class Camera
{
public:
	Camera(DirectX::XMFLOAT4& _eye, DirectX::XMFLOAT4& _at, DirectX::XMFLOAT4& _up, const int _width, const int _height);
	~Camera();



	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT4 &v);
	void SetRotation(float x, float y, float z);
	void SetForward(float x, float y, float z);
	void SetFrustum(float fovY, float aspect, float nearZ, float farZ);

	void MovePosition(float x, float y, float z);
	void MoveRotation(float x, float y, float z);

	DirectX::XMVECTOR GetPositionVec() { return DirectX::XMLoadFloat4(&at); };
	DirectX::XMFLOAT4 GetPosition() { return at; };
	DirectX::XMVECTOR GetRightVec() { return DirectX::XMLoadFloat4(&right); };
	DirectX::XMFLOAT4 GetRight() { return right; };
	DirectX::XMVECTOR GetUpVec() { return DirectX::XMLoadFloat4(&up); };
	DirectX::XMFLOAT4 GetUp() { return up; };
	DirectX::XMVECTOR GetLookVec() { return DirectX::XMLoadFloat4(&eye); };
	DirectX::XMFLOAT4 GetLook() { return eye; };

	void LookAt();
	DirectX::XMFLOAT4X4* GetViewMat() { return &view; };
	DirectX::XMFLOAT4X4* GetProjMat() { return &proj; };
	DirectX::XMFLOAT4X4* GetWorldMat() { return &world; };

	void Strafe(float dist);
	void Move(float dist);
	void MoveUp(float dist);
	void Pitch(float angle);

	void RotateY(float angle);

	void UpdateViewMatrix();
	void Update();

private:
	DirectX::XMFLOAT4 at, right, up, eye, forward;
	DirectX::XMFLOAT4X4 view, proj, world;
	float fovY, aspect, nearZ, farZ;
	float strafeSpeed = 0.2f, moveSpeed = 20.0f, pitchSpeed = 0.2f, rotateYSpeed = 0.2f;
	DirectX::XMFLOAT3 rotation;
};

