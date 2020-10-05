#pragma once

#include <directxmath.h>

class Camera
{
public:
	Camera(DirectX::XMMATRIX &worldMatrix, DirectX::XMFLOAT3 &_eye, DirectX::XMFLOAT3 &_at, DirectX::XMFLOAT3 &_up);
	~Camera();

	DirectX::XMVECTOR GetPositionVec() { return DirectX::XMLoadFloat3(&position); };
	DirectX::XMFLOAT3 GetPositionFloat() { return position; };

	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3 &v);

	DirectX::XMVECTOR GetRightVec();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMVECTOR GetUpVec();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMVECTOR GetLookVec();

	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 worldUp);

	DirectX::XMMATRIX GetViewMat();
	DirectX::XMMATRIX GetProjMat();
	DirectX::XMMATRIX GetViewProjMat();

	void Strafe(float dist);
	void Move(float dist);
	void Pitch(float angle);

	void RotateY(float angle);

	void UpdateViewMatrix();

private:
	DirectX::XMFLOAT3 position, right, up, look;
	DirectX::XMFLOAT4X4 view, proj;

};

