#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "DDSTextureLoader.h"
#include "resource.h"
#include <iostream>
#include <string>

#include "ObjectManager.h"
#include "Light.h"
#include "structures.h"

#include "imgui.h"
#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx11.h"
#include "ImGuiRenderer.h"
#include <vector>

#include "CameraManager.h"
#include "Camera.h"
#include "Time.h"
#include "RenderTexture.h"
#include "DeferredRenderer.h"
using namespace std;
//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT		InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT		InitDevice();
HRESULT     InitImGui();
HRESULT		InitMesh();
HRESULT		InitWorld();
HRESULT     InitObjects();
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

const int				g_viewWidth = 1920;
const int				g_viewHeight = 1080;

RenderTexture* g_pRT;
DeferredRenderer* g_pDR;
ObjectManager* g_pOM;
GameObjectPlane* g_pPlane = nullptr;
Camera* g_pCamera = nullptr;
Time* g_pTime = nullptr;

Light* g_pLight = nullptr;
ImGuiRenderer* g_pImgui;
