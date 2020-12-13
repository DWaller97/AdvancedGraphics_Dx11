#include "ShaderManager.h"

ShaderData ShaderManager::shaderStandard;
ShaderData ShaderManager::shaderRTT;
ShaderData ShaderManager::shaderD;

ID3D11VertexShader* ShaderManager::standardVertexShader;
ID3D11PixelShader* ShaderManager::standardPixelShader;
ID3D11InputLayout* ShaderManager::standardInputLayout;

ID3D11VertexShader* ShaderManager::rttVertexShader;
ID3D11PixelShader* ShaderManager::rttPixelShader;
ID3D11InputLayout* ShaderManager::rttInputLayout;

ID3D11VertexShader* ShaderManager::dVertexShader;
ID3D11PixelShader* ShaderManager::dPixelShader;
ID3D11InputLayout* ShaderManager::dInputLayout;

HRESULT ShaderManager::InitShaders(ID3D11Device* _device)
{
#pragma region Default
    ID3DBlob* pVSBlob = nullptr;
    HRESULT hr = CompileShaderFromFile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }
    hr = _device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &standardVertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

    };
    UINT numElements = ARRAYSIZE(layout);

    hr = _device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &standardInputLayout);
    pVSBlob->Release();
    if (FAILED(hr))
        return hr;

    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

    hr = _device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &standardPixelShader);
    pPSBlob->Release();
    shaderStandard._inputLayout = standardInputLayout;
    shaderStandard._pixelShader = standardPixelShader;
    shaderStandard._vertexShader = standardVertexShader;

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

    _device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &rttVertexShader);
    D3D11_INPUT_ELEMENT_DESC layoutRTT[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = _device->CreateInputLayout(layoutRTT, ARRAYSIZE(layoutRTT), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &rttInputLayout);
    pVSBlob->Release();
    hr = CompileShaderFromFile(L"RTTShader.fx", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }
    _device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &rttPixelShader);
    pPSBlob->Release();

    shaderRTT._inputLayout = rttInputLayout;
    shaderRTT._vertexShader = rttVertexShader;
    shaderRTT._pixelShader = rttPixelShader;

#pragma endregion RTT
#pragma region Deferred
    pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"deferredShader.fx", "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }
    hr = _device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &dVertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC layoutD[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    numElements = ARRAYSIZE(layoutD);

    hr = _device->CreateInputLayout(layoutD, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &dInputLayout);
    pVSBlob->Release();
    if (FAILED(hr))
        return hr;

    pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"deferredShader.fx", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

    hr = _device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &dPixelShader);
    pPSBlob->Release();
    shaderD._inputLayout = dInputLayout;
    shaderD._pixelShader = dPixelShader;
    shaderD._vertexShader = dVertexShader;

    if (FAILED(hr))
        return hr;
#pragma endregion Deferred
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT ShaderManager::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}