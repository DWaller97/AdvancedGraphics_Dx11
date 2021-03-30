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

HRESULT RenderTexture::Initialise(ID3D11Device* _device, IDXGISwapChain* _swapChain)
{
    HRESULT h;
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = 1280;
    textureDesc.Height = 720;
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

    //D3D11_TEXTURE2D_DESC descDepth = {};
    //descDepth.Width = 1280;
    //descDepth.Height = 720;
    //descDepth.MipLevels = 1;
    //descDepth.ArraySize = 1;
    //descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    //descDepth.SampleDesc.Count = 1;
    //descDepth.SampleDesc.Quality = 0;
    //descDepth.Usage = D3D11_USAGE_DEFAULT;
    //descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    //descDepth.CPUAccessFlags = 0;
    //descDepth.MiscFlags = 0;
    //h = _device->CreateTexture2D(&descDepth, nullptr, &rtDepthStencilTexture);
    //if (FAILED(h))
    //    return h;

    //D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    //descDSV.Format = descDepth.Format;
    //descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    //descDSV.Texture2D.MipSlice = 0;
    //h = _device->CreateDepthStencilView(rtDepthStencilTexture, &descDSV, &rtDepthStencilView);
    //if (FAILED(h))
    //    return h;

    //// Create a render target view
    //ID3D11Texture2D* pBackBuffer = nullptr;
    //h = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    //if (FAILED(h))
    //    return h;


    D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
    rtViewDesc.Format = textureDesc.Format;
    rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtViewDesc.Texture2D.MipSlice = 0;

    h = _device->CreateRenderTargetView(rtTexture, &rtViewDesc, &rtView);
    //pBackBuffer->Release();
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
        rtTexture = nullptr;
    }
    if (rtDepthStencilTexture) {
        rtDepthStencilTexture->Release();
        delete rtDepthStencilTexture;
        rtDepthStencilTexture = nullptr;
    }
    if (rtShaderResourceView) {
        rtShaderResourceView->Release();
        rtShaderResourceView = nullptr;
    }
    if (rtView) {
        rtView->Release();
        rtView = nullptr;
    }
}

void RenderTexture::SetAsRenderTarget(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilView* _dstView)
{
    _deviceContext->OMSetRenderTargets(1, &rtView, _dstView);
}

void RenderTexture::ClearView(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilView* _dstView, XMVECTORF32 _clearColour)
{
    _deviceContext->ClearRenderTargetView(rtView, _clearColour);
    _deviceContext->ClearDepthStencilView(_dstView, D3D11_CLEAR_DEPTH, 1.0F, 0);
}
