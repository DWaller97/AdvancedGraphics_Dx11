#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Managers/ObjectManager.h"
#include <vector>
#include "../Utilities/Constants.h"

class DeferredRenderer
{
public:
    HRESULT Initialise(ID3D11Device* _device);
    void Render(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _lightBuffer, XMFLOAT4X4* _projMat, XMFLOAT4X4* _viewMat, ID3D11DepthStencilView* _dsv);
    void Release();
    static ID3D11ShaderResourceView* GetAlbedo() { return srvAlbedo; };
    static ID3D11ShaderResourceView* GetNormals() { return srvNormals; };
    static bool GetActive() { return deferredActive; };
    static void SetActive(bool _b);
private:
    static bool deferredActive;
    static ID3D11ShaderResourceView* srvAlbedo;
    static ID3D11ShaderResourceView* srvPosition;
    static ID3D11ShaderResourceView* srvNormals;

    ID3D11RenderTargetView* rtvAlbedo;
    ID3D11RenderTargetView* rtvPosition;
    ID3D11RenderTargetView* rtvNormals;

    ID3D11Texture2D* pTexAlbedo;
    ID3D11Texture2D* pTexPosition;
    ID3D11Texture2D* pTexNormals;
};

