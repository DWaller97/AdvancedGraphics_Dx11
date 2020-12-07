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



//DirectX::XMFLOAT4 g_EyePosition(0.0f, 0, -3, 1.0f);
//DirectX::XMFLOAT4 g_AtPosition(0.0f, 1.0f, 0.0f, 0.0f);
//DirectX::XMFLOAT4 g_UpPosition(0.0f, 1.0f, 0.0f, 0.0f);

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
void        CleanupImGui();
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
ID3D11Device*           g_pd3dDevice = nullptr;
ID3D11Device1*          g_pd3dDevice1 = nullptr;
ID3D11DeviceContext*    g_pImmediateContext = nullptr;
ID3D11DeviceContext*   g_pImmediateContext1 = nullptr;
IDXGISwapChain*         g_pSwapChain = nullptr;
IDXGISwapChain1*        g_pSwapChain1 = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11Texture2D*        g_pDepthStencil = nullptr;
ID3D11Texture2D*        g_pRenderTexture = nullptr;
ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
ID3D11VertexShader*     g_pVertexShader = nullptr;
ID3D11VertexShader*     g_pVertexShaderRTT = nullptr;

ID3D11PixelShader*      g_pPixelShader = nullptr;
ID3D11PixelShader*      g_pPixelShaderRTT = nullptr;

ID3D11InputLayout*      g_pVertexLayout = nullptr;
ID3D11InputLayout*      g_pVertexLayoutRTT = nullptr;

ID3D11Buffer*           g_pVertexBuffer = nullptr;
ID3D11Buffer*           g_pIndexBuffer = nullptr;
ID3D11Buffer*           g_pConstantBuffer = nullptr;
ID3D11Buffer*           g_pLightConstantBuffer = nullptr;

ID3D11ShaderResourceView*   g_pTextureRV = nullptr;
ID3D11ShaderResourceView*   g_pNormalTextureRV = nullptr;
ID3D11ShaderResourceView*   g_pRTTTargetView = nullptr;
ID3D11SamplerState*	        g_pSamplerLinear = nullptr;
ID3D11SamplerState*	        g_pSamplerNormal = nullptr;


const int				g_viewWidth = 1920;
const int				g_viewHeight = 1080;

DrawableObjectCube*		g_Cube = nullptr;
DrawableGameObject*     g_Monkey = nullptr;
DrawableGameObjectPlane* g_plane = nullptr;
Camera*                 g_Camera = nullptr;
Time*                   time = nullptr;

//Lighting Variables
float lightColour[4] = { 1, 1, 1, 1 };
XMFLOAT4 lightPosition = XMFLOAT4(0, 0, 0, 0);
XMFLOAT4 lightRotation = XMFLOAT4(0, 0, 1, 1);


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow ){
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }
    
    



    if (FAILED(InitImGui())){
        CleanupImGui();
        return 0;
    }

    ImGuiIO& io = ImGui::GetIO();

    time = new Time();

    InitObjects();

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {

        //g_pSwapChain->Present(1, 0);
        UpdateWindow(g_hWnd);

        io = ImGui::GetIO();

        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        Update();
        Render();
    }

    CleanupDevice();
    CleanupImGui();
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
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile( szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob );
    if( FAILED(hr) )
    {
        if( pErrorBlob )
        {
            OutputDebugStringA( reinterpret_cast<const char*>( pErrorBlob->GetBufferPointer() ) );
            pErrorBlob->Release();
        }
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

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
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
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
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
    InitObjects();

    return S_OK;
}

HRESULT InitImGui()
{
    // Application init: create a dear imgui context, setup some options, load fonts
    ImGui::CreateContext();
    // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
    // TODO: Fill optional fields of the io structure later.
    // TODO: Load TTF/OTF fonts if you don't want to use the default font.

    // Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32.cpp and imgui_impl_dx11.cpp)
    if (!ImGui_ImplWin32_Init(g_hWnd) || !ImGui_ImplDX11_Init(g_pd3dDevice, g_pImmediateContext))
        return E_FAIL;
    return S_OK;
}

DrawableGameObject::ShaderData shaderFX;
DrawableGameObject::ShaderData shaderRTT;


// ***************************************************************************************
// InitMesh
// ***************************************************************************************

HRESULT		InitMesh()
{ 

#pragma region Default
	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	HRESULT hr = CompileShaderFromFile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	//g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader);
	pPSBlob->Release();
    shaderFX._inputLayout = g_pVertexLayout;
    shaderFX._pixelShader = g_pPixelShader;
    shaderFX._vertexShader = g_pVertexShader;

	if (FAILED(hr))
		return hr;
#pragma endregion Default
#pragma region RTT
    pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"RTTShader.fx", "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }
    g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShaderRTT);
    D3D11_INPUT_ELEMENT_DESC layoutRTT[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    // Create the input layout
    hr = g_pd3dDevice->CreateInputLayout(layoutRTT, ARRAYSIZE(layoutRTT), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pVertexLayoutRTT);
    pVSBlob->Release();
    pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"RTTShader.fx", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }
    g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShaderRTT);
    pPSBlob->Release();

    shaderRTT._inputLayout = g_pVertexLayoutRTT;
    shaderRTT._vertexShader = g_pVertexShaderRTT;
    shaderRTT._pixelShader = g_pPixelShaderRTT;



    if (FAILED(hr))
        return hr;


#pragma endregion RTT
    D3D11_BUFFER_DESC bd = {};

	

	// Create the light constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LightPropertiesConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pLightConstantBuffer);
	if (FAILED(hr))
		return hr;


	return hr;
}
DrawableObjectCube* newCube = nullptr;
// ***************************************************************************************
// InitWorld
// ***************************************************************************************
HRESULT		InitWorld()
{
    DirectX::XMFLOAT4 eye = DirectX::XMFLOAT4(0, 2, -3, 1);
    DirectX::XMFLOAT4 at = DirectX::XMFLOAT4(0, -0.5f, 0, 1);
    DirectX::XMFLOAT4 up = DirectX::XMFLOAT4(0, 1, 0, 1);

    g_Camera = CameraManager::CreateCamera(eye, at, up, g_viewWidth, g_viewHeight);
    g_Camera->SetFrustum(DirectX::XM_PIDIV2, g_viewWidth / (float)g_viewHeight, 0.001f, 100);

	return S_OK;
}

HRESULT InitObjects() {

    g_Cube = new DrawableObjectCube();
    g_Cube->InitMesh(g_pd3dDevice, g_pImmediateContext);
    g_Cube->SetShaders(shaderFX);
    g_Cube->SetPosition(XMFLOAT3(0, 0, 0));
    newCube = new DrawableObjectCube();
    newCube->SetShaders(shaderFX);
    newCube->SetPosition(XMFLOAT3(5, 0, 0));
    newCube->InitMesh(g_pd3dDevice, g_pImmediateContext);
    
    g_plane = new DrawableGameObjectPlane();
    g_plane->SetShaders(shaderRTT);
    g_plane->SetPosition(XMFLOAT3(0, 0, 0));
    g_plane->InitMesh(g_pd3dDevice, g_pImmediateContext);

    g_Monkey = new DrawableGameObject();
    g_Monkey->InitMesh(g_pd3dDevice, g_pImmediateContext);
    g_Monkey->SetMesh((char*)"Resources/cube.obj", g_pd3dDevice, true);
    g_Monkey->SetShaders(shaderFX);
    g_Monkey->SetPosition(XMFLOAT3(0, 1, 5));
    


    
    vecDrawables.push_back(g_Monkey);
    vecDrawables.push_back(g_plane);
    vecDrawables.push_back(g_Cube);
    vecDrawables.push_back(newCube);



    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext )   g_pImmediateContext->ClearState();
    if( g_pConstantBuffer )     g_pConstantBuffer->Release();
    if( g_pVertexBuffer )       g_pVertexBuffer->Release();
    if( g_pIndexBuffer )        g_pIndexBuffer->Release();
    if( g_pVertexLayout )       g_pVertexLayout->Release();
    if( g_pVertexLayoutRTT )    g_pVertexLayoutRTT->Release();
    if( g_pVertexShader )       g_pVertexShader->Release();
    if( g_pVertexShaderRTT)     g_pVertexShaderRTT->Release();
    if( g_pPixelShader )        g_pPixelShader->Release();
    if( g_pPixelShaderRTT)      g_pPixelShaderRTT->Release();
    if( g_pDepthStencil )       g_pDepthStencil->Release();
    if( g_pDepthStencilView )   g_pDepthStencilView->Release();
    if( g_pRenderTargetView )   g_pRenderTargetView->Release();
    if( g_pSwapChain1 )         g_pSwapChain1->Release();
    if( g_pSwapChain )          g_pSwapChain->Release();
    if( g_pImmediateContext1 )  g_pImmediateContext1->Release();
    if( g_pd3dDevice1 )         g_pd3dDevice1->Release();
    if( g_pd3dDevice )          g_pd3dDevice->Release();
    if (g_pRenderTexture)       g_pRenderTexture->Release();
    if (g_pRTTTargetView)       g_pRTTTargetView->Release();
    delete g_Camera;
    g_Camera = nullptr;
}

void CleanupImGui() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Cleanup() {

    delete time;
    time = nullptr;
    
    delete g_Monkey;
    g_Monkey = nullptr;

    delete g_Cube;
    g_Cube = nullptr;

    delete g_plane;
    g_plane = nullptr;

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
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            if (g_pRenderTargetView) {
                g_pRenderTargetView->Release();
                g_pRenderTargetView = NULL;
            }
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            ID3D11Texture2D* pBackBuffer;
            g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
            pBackBuffer->Release();
        }
        return 0; 
    case WM_DESTROY:   
        PostQuitMessage( 0 );
        break;
     
    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }
     
    return 0;
}

Light light;
float planePosX;
float planePosY;
float planePosZ;
void Update() {

    float dTime = time->DeltaTime();


    if (GetAsyncKeyState('W')) // W
        g_Camera->MovePosition(0, 0, dTime);
    if (GetAsyncKeyState('A')) // A
        g_Camera->MovePosition(-dTime, 0, 0);
    if (GetAsyncKeyState('S')) // S
        g_Camera->MovePosition(0, 0, -dTime);
    if (GetAsyncKeyState('D')) // D
        g_Camera->MovePosition(dTime, 0, 0);
    if (GetAsyncKeyState('Q')) // Q
        g_Camera->Pitch(dTime);
    if (GetAsyncKeyState('E')) // E
        g_Camera->Pitch(-dTime);
    if (GetAsyncKeyState(VK_SPACE)) // Space
        g_Camera->MovePosition(0, dTime, 0);
    if (GetAsyncKeyState(VK_SHIFT)) // L Shift
        g_Camera->MovePosition(0, -dTime, 0);

    light.Enabled = static_cast<int>(true);
    light.LightType = PointLight;
    light.Color = XMFLOAT4(lightColour);
    light.SpotAngle = XMConvertToRadians(15.0f);
    light.ConstantAttenuation = 1.0f;
    light.LinearAttenuation = 1;
    light.QuadraticAttenuation = 1;
    light.Direction = lightRotation;
    g_Camera->Update();

    light.Position = lightPosition;
    XMVECTOR LightDirection = XMLoadFloat4(&light.Direction);
    LightDirection = XMVector4Normalize(LightDirection);
    XMStoreFloat4(&light.Direction, LightDirection);

    LightPropertiesConstantBuffer lightProperties;
    XMStoreFloat4(&lightProperties.EyePosition, g_Camera->GetLookVec());
    lightProperties.Lights[0] = light;
    lightProperties.GlobalAmbient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
    g_pImmediateContext->UpdateSubresource(g_pLightConstantBuffer, 0, nullptr, &lightProperties, 0, 0);
    (g_Cube)->SetPosition(XMFLOAT3(planePosX, planePosY, planePosZ));
    for (int i = 0; i < vecDrawables.size(); i++) {
        vecDrawables.at(i)->Update(dTime);
    }

}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
    

    // Feed inputs to dear imgui, start new frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    {
        ImGui::NewFrame();
        ImGui::Begin("Light");
        ImGui::Text("Colour");
        ImGui::Separator();
        ImGui::Indent();
        ImGui::ColorEdit3("Light Colour", lightColour);
        ImGui::NewLine();
        ImGui::Unindent();
        ImGui::Text("Positions");
        ImGui::Separator();
        ImGui::Indent();
        ImGui::SliderFloat("Position X", &lightPosition.x, -3, 3.0f);
        ImGui::SliderFloat("Position Y", &lightPosition.y, -3.0f, 3.0f);
        ImGui::SliderFloat("Position Z", &lightPosition.z, -3.0f, 3.0f);
        ImGui::Unindent();
        ImGui::Text("Rotations");
        ImGui::Separator();
        ImGui::Indent();
        ImGui::SliderFloat("Rotation X", &lightRotation.x, -1, 1.0f);
        ImGui::SliderFloat("Rotation Y", &lightRotation.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Rotation Z", &lightRotation.z, -1.0f, 1.0f);
        ImGui::End();
    }
    {
        ImGui::Begin("Camera");
        ImGui::Text("Eye Position: %f %f %f", g_Camera->GetLook().x, g_Camera->GetLook().y, g_Camera->GetLook().z);
        ImGui::Text("At Position: %f %f %f", g_Camera->GetPosition().x, g_Camera->GetPosition().y, g_Camera->GetPosition().z);
        ImGui::Text("Up Position: %f %f %f", g_Camera->GetUp().x, g_Camera->GetUp().y, g_Camera->GetUp().z);
        ImGui::Text("View Direction: %f %f %f", g_Camera->GetLook().x, g_Camera->GetLook().y, g_Camera->GetLook().z);
        ImGui::End();
    }
    {
        ImGui::Begin("Object");
        ImGui::SliderFloat("Position X: %f", &planePosX, 0, 5);
        ImGui::SliderFloat("Position Y: %f", &planePosY, 0, 5);
        ImGui::SliderFloat("Position Z: %f", &planePosZ, 0, 5);
        ImGui::End();
    }
    ImGui::Render();

    // Clear the back buffer
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, Colors::MidnightBlue );

    // Clear the depth buffer to 1.0 (max depth)
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    for (int i = 0; i < vecDrawables.size(); i++) {
        vecDrawables.at(i)->Draw(g_pImmediateContext, g_pLightConstantBuffer, g_Camera->GetProjMat(), g_Camera->GetViewMat());
    }
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    // Present our back buffer to our front buffer
    g_pSwapChain->Present( 0, 0 );
}



