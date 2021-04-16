#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include <iostream>
#include <string>
#include <vector>

#include "Utilities/DDSTextureLoader.h"
#include "Utilities/Resource.h"
#include "Utilities/structures.h"
#include "Utilities/Time.h"
#include "Utilities/Constants.h"

#include "Managers/ObjectManager.h"
#include "Managers/CameraManager.h"
#include "Managers/AnimationManager.h"

#include "External/imgui/imgui.h"
#include "External/imgui/imgui_impl_win32.h"
#include "External/imgui/imgui_impl_dx11.h"

#include "Graphics/ImGuiRenderer.h"
#include "Graphics/DeferredRenderer.h"
#include "Graphics/RenderTexture.h"

#include "Camera.h"
#include "Light.h"
using namespace std;
//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT		InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT		InitDevice();
HRESULT     InitImGui();
HRESULT		InitMesh();
HRESULT		InitWorld();
void		CleanupDevice();
void        Cleanup();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void        Update();
void		Render();
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = nullptr;
HWND                    g_hWnd = nullptr;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device* g_pd3dDevice = nullptr;
ID3D11Device1* g_pd3dDevice1 = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
ID3D11DeviceContext* g_pImmediateContext1 = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
IDXGISwapChain1* g_pSwapChain1 = nullptr;

ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11Texture2D* g_pDepthStencil = nullptr;
ID3D11Texture2D* g_pRenderTexture = nullptr;
ID3D11DepthStencilView* g_pDepthStencilView = nullptr;

ID3D11Buffer* g_pLightConstantBuffer = nullptr;

ID3D11ShaderResourceView* g_pRTTTargetView = nullptr;
ID3D11ShaderResourceView* g_shaderResourceView = nullptr;
ID3D11SamplerState* g_pSamplerLinear = nullptr;

const int				g_viewWidth = Constants::WINDOW_WIDTH;
const int				g_viewHeight = Constants::WINDOW_HEIGHT;

RenderTexture* g_pRT;
DeferredRenderer* g_pDR;
ObjectManager* g_pOM;
AnimationManager* g_pAM;
GameObjectPlane* g_pRenderPlane = nullptr;
Camera* g_pCamera = nullptr;
Time* g_pTime = nullptr;

Light* g_pLight = nullptr;
ImGuiRenderer* g_pImgui;
