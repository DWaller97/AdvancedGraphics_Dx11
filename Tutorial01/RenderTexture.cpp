#include "RenderTexture.h"
using namespace DirectX;
RenderTexture::RenderTexture()
{
    rtView = nullptr;
    rtTexture = nullptr;
    rtShaderResourceView = nullptr;
}

RenderTexture::~RenderTexture()
{
    Release();
}

HRESULT RenderTexture::Initialise(ID3D11Device* _device)
{
    HRESULT h;
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = 1920;
    textureDesc.Height = 1080;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    h = _device->CreateTexture2D(&textureDesc, nullptr, &rtTexture);
    if (FAILED(h))
        return h;


    D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
    rtViewDesc.Format = textureDesc.Format;
    rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtViewDesc.Texture2D.MipSlice = 0;

    h = _device->CreateRenderTargetView(rtTexture, &rtViewDesc, &rtView);
    if (FAILED(h))
        return h;

    D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
    resourceViewDesc.Format = textureDesc.Format;
    resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    resourceViewDesc.Texture2D.MostDetailedMip = 0;
    resourceViewDesc.Texture2D.MipLevels = 1;
    h = _device->CreateShaderResourceView(rtTexture, &resourceViewDesc, &rtShaderResourceView);
    if (FAILED(h))
        return h;

    return S_OK;

}

void RenderTexture::Release()
{
    if (rtTexture) {
        rtTexture->Release();
        delete rtTexture;
        rtTexture = nullptr;
    }
    if (rtShaderResourceView) {
        rtShaderResourceView->Release();
        delete rtShaderResourceView;
        rtShaderResourceView = nullptr;
    }
    if (rtView) {
        rtView->Release();
        delete rtView;
        rtView = nullptr;
    }
}

void RenderTexture::SetAsRenderTarget(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilView* _depthStencilView)
{
    _deviceContext->OMSetRenderTargets(1, &rtView, _depthStencilView);
}

void RenderTexture::ClearView(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilView* _depthStencilView, XMVECTORF32 _clearColour)
{
    _deviceContext->ClearRenderTargetView(rtView, _clearColour);
    _deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0F, 0);
}
