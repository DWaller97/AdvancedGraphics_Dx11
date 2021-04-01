#pragma once
#include <vector>

#include "../External/imgui/imgui.h"
#include "../External/imgui/imgui_impl_dx11.h"
#include "../External/imgui/imgui_impl_win32.h"
#include "../Light.h"

#include "../GameObjects/GameObject.h"
#include "../GameObjects/GameObjectTerrain.h"

#include "../Graphics/DeferredRenderer.h"

#include "../Managers/ObjectManager.h"
#include "../Managers/CameraManager.h"

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
    float* camSpeed = nullptr;

    GameObjectTerrain* m_terrain;
    int m_terrainSeed;
    int m_terrainSize;

    ImGuiIO io;
    static bool initialised;
    bool deferred;
public:
    static HRESULT Init(HWND _hwnd, ID3D11Device* _device, ID3D11DeviceContext* _deviceContext);
    static ImGuiRenderer* GetInstance();
    void Start();
    void Update(Light* _light);
    void Render();
    void Cleanup();
};

