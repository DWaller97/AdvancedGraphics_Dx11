#include "CameraManager.h"

Camera* CameraManager::currCamera = nullptr;
std::queue<Camera*> CameraManager::cameras;


CameraManager::~CameraManager()
{
    for (int i = 0; i < cameras.size() / sizeof(Camera); i++) {
        delete cameras.front();
        cameras.pop();
    }
}

Camera* CameraManager::CreateCamera(DirectX::XMFLOAT3& _eye, DirectX::XMFLOAT3& _at, DirectX::XMFLOAT3& _up, const int _width, const int _height)
{
    Camera* newCam = new Camera(_eye, _at, _up, _width, _height);
    if (!currCamera) {
        currCamera = newCam;
    }
    cameras.push(newCam);
    return newCam;
}

void CameraManager::SwitchCamera(Camera* newCamera)
{
    currCamera = newCamera;
}
