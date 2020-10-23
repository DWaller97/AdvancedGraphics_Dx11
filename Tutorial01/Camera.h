#pragma once
#include <directxmath.h>
#include <d3d11.h>

class Camera
{
public:
	Camera(DirectX::XMFLOAT3& _eye, DirectX::XMFLOAT3& _at, DirectX::XMFLOAT3& _up, const int _width, const int _height);
	~Camera();

	DirectX::XMVECTOR GetPositionVec() { return DirectX::XMLoadFloat3(&at); };
	DirectX::XMFLOAT3 GetPosition() { return at; };

	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3 &v);

	void SetFrustum(float fovY, float aspect, float nearZ, float farZ);

	DirectX::XMVECTOR GetRightVec() { return DirectX::XMLoadFloat3(&right); };
	DirectX::XMFLOAT3 GetRight() { return right; };
	DirectX::XMVECTOR GetUpVec() { return DirectX::XMLoadFloat3(&up); };
	DirectX::XMFLOAT3 GetUp() { return up; };
	DirectX::XMVECTOR GetLookVec() { return DirectX::XMLoadFloat3(&eye); };
	DirectX::XMFLOAT3 GetLook() { return eye; };

	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 worldUp);

	DirectX::XMFLOAT4X4* GetViewMat() { return &view; };
	DirectX::XMFLOAT4X4* GetProjMat() { return &proj; };
	DirectX::XMFLOAT4X4* GetWorldMat() { return &world; };

	void Strafe(float dist);
	void Move(float dist);
	void MoveUp(float dist);
	void Pitch(float angle);

	void RotateY(float angle);

	void UpdateViewMatrix();

private:
	DirectX::XMFLOAT3 at = DirectX::XMFLOAT3(0, 1, 0), right = DirectX::XMFLOAT3(1, 0, 0), up = DirectX::XMFLOAT3(0, 1, 0), eye = DirectX::XMFLOAT3(0, 0, -3);
	DirectX::XMFLOAT4X4 view, proj, world;
	float fovY, aspect, nearZ, farZ;
	float strafeSpeed = 0.2f, moveSpeed = 0.2f, pitchSpeed = 0.2f, rotateYSpeed = 0.2f;
};

