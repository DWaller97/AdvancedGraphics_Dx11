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
    //Arrays
    static ID3D11VertexShader* standardVertexShader;
    static ID3D11PixelShader* standardPixelShader;
    static ID3D11InputLayout* standardInputLayout;

    static ID3D11VertexShader* rttVertexShader;
    static ID3D11PixelShader* rttPixelShader;
    static ID3D11InputLayout* rttInputLayout;

    static ID3D11VertexShader* dAlbedoVertexShader;
    static ID3D11PixelShader* dAlbedoPixelShader;
    static ID3D11InputLayout* dAlbedoInputLayout;

    static ID3D11VertexShader* dNormalVertexShader;
    static ID3D11PixelShader* dNormalPixelShader;
    static ID3D11InputLayout* dNormalInputLayout;

    static ID3D11VertexShader* dPostVertexShader;
    static ID3D11PixelShader* dPostPixelShader;
    static ID3D11InputLayout* dPostInputLayout;

    static ID3D11VertexShader* dPositionVertexShader;
    static ID3D11PixelShader* dPositionPixelShader;
    static ID3D11InputLayout* dPositionInputLayout;
public:
    static ShaderData shaderStandard;
    static ShaderData shaderRTT;
    static ShaderData shaderDAlbedo;
    static ShaderData shaderDNormal;
    static ShaderData shaderDPosition;
    static ShaderData shaderDPost;

    static HRESULT InitShaders(ID3D11Device* _device);
    static HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
};

