#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>

//TODO: shutdown any shaders/input layouts to release memory

struct ShaderData {
    ID3D11PixelShader* _pixelShader = nullptr;
    ID3D11VertexShader* _vertexShader = nullptr;
    ID3D11HullShader* _hullShader = nullptr;
    ID3D11DomainShader* _domainShader = nullptr;
    ID3D11InputLayout* _inputLayout = nullptr;
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

    static ID3D11VertexShader* lineVertexShader;
    static ID3D11PixelShader* linePixelShader;
    static ID3D11InputLayout* lineInputLayout;

    static ID3D11VertexShader* terrainVertexShader;
    static ID3D11PixelShader* terrainPixelShader;
    static ID3D11HullShader* terrainHullShader;
    static ID3D11DomainShader* terrainDomainShader;
    static ID3D11InputLayout* terrainInputLayout;

public:
    static ShaderData shaderStandard;
    static ShaderData shaderRTT;
    static ShaderData shaderDAlbedo;
    static ShaderData shaderDNormal;
    static ShaderData shaderDPosition;
    static ShaderData shaderDPost;
    static ShaderData shaderLine;
    static ShaderData shaderTerrain;

    static HRESULT InitShaders(ID3D11Device* _device);
    static HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
};

