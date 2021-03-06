#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>
class RenderTexture
{
public:
    RenderTexture();
    ~RenderTexture();
    HRESULT Initialise(ID3D11Device* _device, IDXGISwapChain* _swapChain);
    void Release();
    void SetAsRenderTarget(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilView* _dstView);
    void ClearView(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilView* _dstView, DirectX::XMVECTORF32 _clearColour);
    ID3D11ShaderResourceView* GetShaderResourceView() { return rtShaderResourceView; }
private:
    D3D11_VIEWPORT vp;
    ID3D11RenderTargetView* rtView;
    ID3D11Texture2D* rtTexture;
    ID3D11Texture2D* rtDepthStencilTexture = nullptr;
    ID3D11ShaderResourceView* rtShaderResourceView;
};

