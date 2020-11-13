#pragma once
#include "Camera.h"
#include <queue>
static class CameraManager {
private:
	static Camera* currCamera;
public:
	~CameraManager();
	static std::queue<Camera*> cameras;
	static Camera* CreateCamera(DirectX::XMFLOAT3& _eye, DirectX::XMFLOAT3& _at, DirectX::XMFLOAT3& _up, const int _width, const int _height);
	static void SwitchCamera(Camera* newCamera);
	static Camera* GetCurrCamera() { return currCamera; }
};