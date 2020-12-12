#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>

struct ShaderData {
    ID3D11PixelShader* _pixelShader;
    ID3D11VertexShader* _vertexShader;
    ID3D11InputLayout* _inputLayout;
};

class ShaderManager
{
private:
    static ID3D11VertexShader* standardVertexShader;
    static ID3D11PixelShader* standardPixelShader;
    static ID3D11InputLayout* standardInputLayout;

    static ID3D11VertexShader* rttVertexShader;
    static ID3D11PixelShader* rttPixelShader;
    static ID3D11InputLayout* rttInputLayout;
public:
    static ShaderData shaderStandard;
    static ShaderData shaderRTT;

    static HRESULT InitShaders(ID3D11Device* _device);
    static HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
};

