//--------------------------------------------------------------------------------------
// File: main.cpp
//
// This application demonstrates animation using matrix transformations
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729722.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#define _XM_NO_INTRINSICS_

#include "main.h"

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle g_pTime is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow ){
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;
    //AllocConsole();
    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }
    InitImGui();

    g_pTime = new Time();
    g_pOM = new ObjectManager();
    g_pOM->CreateObjects(g_pd3dDevice, g_pImmediateContext);
    g_pRenderPlane = g_pOM->GetRenderPlane();
    g_pDR = new DeferredRenderer();
    g_pDR->Initialise(g_pd3dDevice);
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {

        UpdateWindow(g_hWnd);
        
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        Update();
        Render();
    }

    CleanupDevice();
    Cleanup();
    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;


    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, g_viewWidth, g_viewHeight };

    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass",
                            L"Direct3D 11",
                           WS_OVERLAPPEDWINDOW/* | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX*/,
                           CW_USEDEFAULT,
                            CW_USEDEFAULT, 
                            rc.right - rc.left, 
                            rc.bottom - rc.top, 
                            nullptr, 
                            nullptr, 
                            hInstance,
                           nullptr );
    if( !g_hWnd )
        return E_FAIL;
    ShowWindow( g_hWnd, nCmdShow );
    UpdateWindow(g_hWnd);

    
    return S_OK;
}





//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        if ( hr == E_INVALIDARG )
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                                    D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        }

        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = g_pd3dDevice->QueryInterface( __uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice) );
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent( __uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory) );
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface( __uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2) );
    if ( dxgiFactory2 )
    {
        // DirectX 11.1 or later
        hr = g_pd3dDevice->QueryInterface( __uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1) );
        if (SUCCEEDED(hr))
        {
            (void) g_pImmediateContext->QueryInterface( __uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1) );
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = width;
        sd.Height = height;
		sd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;//  DXGI_FORMAT_R16G16B16A16_FLOAT;////DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd( g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1 );
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface( __uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain) );
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 2;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
        sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain( g_pd3dDevice, &sd, &g_pSwapChain );
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation( g_hWnd, DXGI_MWA_NO_ALT_ENTER );

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &pBackBuffer ) );
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pRT = new RenderTexture();
    hr = g_pRT->Initialise(g_pd3dDevice, g_pSwapChain);
    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, nullptr, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );
    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    if (FAILED(InitWorld()))
    {
        MessageBox(nullptr,
            L"Failed to initialise world.", L"Error", MB_OK);
        return 0;
    }

    if (FAILED(InitMesh()))
    {
        MessageBox(nullptr,
            L"Failed to initialise mesh.", L"Error", MB_OK);
        return 0;
    }

    return S_OK;
}

HRESULT InitImGui()
{
    HRESULT hr;
    hr = ImGuiRenderer::Init(g_hWnd, g_pd3dDevice, g_pImmediateContext);
    if (FAILED(hr))
        return hr;
    g_pImgui = ImGuiRenderer::GetInstance();
    return S_OK;
}




// ***************************************************************************************
// InitMesh
// ***************************************************************************************

HRESULT		InitMesh()
{ 
    HRESULT hr = ShaderManager::InitShaders(g_pd3dDevice);
    D3D11_BUFFER_DESC bd = {};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LightPropertiesConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pLightConstantBuffer);
	return hr;
}
// ***************************************************************************************
// InitWorld
// ***************************************************************************************
HRESULT		InitWorld()
{
    DirectX::XMFLOAT4 eye = DirectX::XMFLOAT4(0, 2, -3, 1);
    DirectX::XMFLOAT4 at = DirectX::XMFLOAT4(0, -0.5f, 0, 1);
    DirectX::XMFLOAT4 up = DirectX::XMFLOAT4(0, 1, 0, 1);

    g_pCamera = CameraManager::CreateCamera(eye, at, up, g_viewWidth, g_viewHeight);
    g_pCamera->SetFrustum(DirectX::XM_PIDIV2, g_viewWidth / (float)g_viewHeight, 0.001f, 100);
    
    g_pLight = new Light();
    g_pLight->enabled = static_cast<int>(true);
    g_pLight->lightType = Light::LightType::PointLight;
    g_pLight->spotAngle = XMConvertToRadians(15.0f);
    g_pLight->constantAttenuation = 1.0f;
    g_pLight->linearAttenuation = 1;
    g_pLight->quadraticAttenuation = 1;

	return S_OK;
}
//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext )   g_pImmediateContext->ClearState();
    if( g_pDepthStencil )       g_pDepthStencil->Release();
    if( g_pDepthStencilView )   g_pDepthStencilView->Release();
    if( g_pRenderTargetView )   g_pRenderTargetView->Release();
    if( g_pSwapChain1 )         g_pSwapChain1->Release();
    if( g_pSwapChain )          g_pSwapChain->Release();
    if( g_pd3dDevice )          g_pd3dDevice->Release();
    if (g_pRenderTexture)       g_pRenderTexture->Release();
    if (g_pRTTTargetView)       g_pRTTTargetView->Release();
    if (g_pImgui) g_pImgui->Cleanup();
    if (g_pDR) g_pDR->Release();

}


void Cleanup() {
    delete g_pLight;
    g_pLight = nullptr;

    delete g_pCamera;
    g_pCamera = nullptr;

    delete g_pRT;
    g_pRT = nullptr;

    delete g_pDR;
    g_pDR = nullptr;

    delete g_pTime;
    g_pTime = nullptr;
    
    delete g_pOM;
    g_pOM = nullptr;

    delete g_pRenderPlane;
    g_pRenderPlane = nullptr;

}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;


    switch( message )
    {

    case WM_DESTROY:   
        PostQuitMessage( 0 );
        break;
     
    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }
     
    return 0;
}

void Update() {

    float dTime = g_pTime->DeltaTime();


    if (GetAsyncKeyState('W')) // W
        g_pCamera->MovePosition(0, 0, dTime);
    if (GetAsyncKeyState('A')) // A
        g_pCamera->MovePosition(-dTime, 0, 0);
    if (GetAsyncKeyState('S')) // S
        g_pCamera->MovePosition(0, 0, -dTime);
    if (GetAsyncKeyState('D')) // D
        g_pCamera->MovePosition(dTime, 0, 0);
    
    //Doesn't work
    //if (GetAsyncKeyState('Q')) // Q
    //    g_pCamera->Pitch(dTime);
    //if (GetAsyncKeyState('E')) // E
    //    g_pCamera->Pitch(-dTime);
    if (GetAsyncKeyState(VK_SPACE)) // Space
        g_pCamera->MovePosition(0, dTime, 0);
    if (GetAsyncKeyState(VK_SHIFT)) // L Shift
        g_pCamera->MovePosition(0, -dTime, 0);


    g_pCamera->Update();
    g_pImgui->Update(g_pLight);
    LightPropertiesConstantBuffer lightProperties;
    XMStoreFloat4(&lightProperties.EyePosition, g_pCamera->GetLookVec());
    lightProperties.Lights[0] = *g_pLight;
    lightProperties.GlobalAmbient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
    g_pImmediateContext->UpdateSubresource(g_pLightConstantBuffer, 0, nullptr, &lightProperties, 0, 0);
    g_pOM->Update(dTime);
    g_pRenderPlane->Update(dTime);
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{


    //See DeferredRenderer.cpp for the bulk of the work
    if (DeferredRenderer::GetActive()) {
        g_pDR->Render(g_pImmediateContext, g_pLightConstantBuffer, g_pCamera->GetProjMat(), g_pCamera->GetViewMat(), g_pDepthStencilView);

        g_pRT->SetAsRenderTarget(g_pImmediateContext, g_pDepthStencilView);
        g_pRT->ClearView(g_pImmediateContext, g_pDepthStencilView, Colors::Crimson);
        g_pRenderPlane->SetShaders(ShaderManager::shaderDPost);
        g_pRenderPlane->SetAlbedoTexture(g_pDR->GetAlbedo());
        g_pRenderPlane->SetNormalTexture(g_pDR->GetNormals());

        g_pRenderPlane->Draw(g_pImmediateContext, g_pLightConstantBuffer, g_pCamera->GetProjMat(), g_pCamera->GetViewMat());

        g_pOM->Render(g_pImmediateContext, g_pLightConstantBuffer, g_pCamera->GetProjMat(), g_pCamera->GetViewMat());

        g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

        g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, Colors::Aquamarine);
        g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        g_pRenderPlane->SetShaders(ShaderManager::shaderRTT);
        g_pRenderPlane->SetAlbedoTexture(g_pRT->GetShaderResourceView());
        g_pRenderPlane->Draw(g_pImmediateContext, g_pLightConstantBuffer, g_pCamera->GetProjMat(), g_pCamera->GetViewMat());
    }
    else {
/**********************************************************
    MARKING SCHEME:Render to Texture and Full-Screen Quad
    DESCRIPTION: Renders all of the objects to my separate
    render target, then switches target to the back buffer
    before rendering again, then setting the texture of my
    plane to the previous render target's resource view
    before the shader displays it on the screen
**********************************************************/
        g_pRT->SetAsRenderTarget(g_pImmediateContext, g_pDepthStencilView);
        g_pRT->ClearView(g_pImmediateContext, g_pDepthStencilView, Colors::Crimson);
        g_pOM->Render(g_pImmediateContext, g_pLightConstantBuffer, g_pCamera->GetProjMat(), g_pCamera->GetViewMat());
        g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

        g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, Colors::Aquamarine);
        g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        g_pRenderPlane->SetShaders(ShaderManager::shaderRTT);
        g_pRenderPlane->SetAlbedoTexture(g_pRT->GetShaderResourceView());
        g_pRenderPlane->Draw(g_pImmediateContext, g_pLightConstantBuffer, g_pCamera->GetProjMat(), g_pCamera->GetViewMat());

    }




    g_pImgui->Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    g_pSwapChain->Present( 0, 0);


}