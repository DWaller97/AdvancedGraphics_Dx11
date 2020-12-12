#pragma once
#include <examples\imgui_impl_dx11.h>
#include <examples\imgui_impl_win32.h>
#include <vector>
#include "imgui.h"
#include "Light.h"
#include "GameObject.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include <iostream>
using namespace DirectX;
class ImGuiRenderer
{
private:
    static ImGuiRenderer* instance;
    float lightColour[4] = { 1, 1, 1, 1 };
    XMFLOAT4 lightPosition = XMFLOAT4(0, 0, 0, 0);
    XMFLOAT4 lightRotation = XMFLOAT4(0, 0, 1, 1);
    XMFLOAT4 camEye = XMFLOAT4(0, 0, 0, 0);
    XMFLOAT4 camUp = XMFLOAT4(0, 0, 0, 0);
    XMFLOAT4 camAt = XMFLOAT4(0, 0, 0, 0);
    ImGuiIO io;
    static bool initialised;
public:
    static HRESULT Init(HWND _hwnd, ID3D11Device* _device, ID3D11DeviceContext* _deviceContext);
    static ImGuiRenderer* GetInstance();

    void Update(Light* _light);
    void Render();
    void Cleanup();
};

