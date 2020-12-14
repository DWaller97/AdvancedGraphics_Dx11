#include "DeferredRenderer.h"

ID3D11ShaderResourceView* DeferredRenderer::srvAlbedo;
ID3D11ShaderResourceView* DeferredRenderer::srvPosition;
ID3D11ShaderResourceView* DeferredRenderer::srvNormals;

HRESULT DeferredRenderer::Initialise(ID3D11Device* _device)
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

    h = _device->CreateTexture2D(&textureDesc, nullptr, &pTexAlbedo);
    if (FAILED(h))
        return h;
    h = _device->CreateTexture2D(&textureDesc, nullptr, &pTexNormals);
    if (FAILED(h))
        return h;
    h = _device->CreateTexture2D(&textureDesc, nullptr, &pTexPosition);
    if (FAILED(h))
        return h;


    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = textureDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    
    h = _device->CreateRenderTargetView(pTexAlbedo, &rtvDesc, &rtvAlbedo);
    if (FAILED(h))
        return h;
    h = _device->CreateRenderTargetView(pTexNormals, &rtvDesc, &rtvNormals);
    if (FAILED(h))
        return h; 
    h = _device->CreateRenderTargetView(pTexPosition, &rtvDesc, &rtvPosition);
    if (FAILED(h))
        return h;

    D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
    rvDesc.Format = textureDesc.Format;
    rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    rvDesc.Texture2D.MostDetailedMip = 0;
    rvDesc.Texture2D.MipLevels = 1;

    h = _device->CreateShaderResourceView(pTexAlbedo, &rvDesc, &srvAlbedo);
    if (FAILED(h))
        return h; 
    h = _device->CreateShaderResourceView(pTexNormals, &rvDesc, &srvNormals);
    if (FAILED(h))
        return h; 
    h = _device->CreateShaderResourceView(pTexPosition, &rvDesc, &srvPosition);
    return h;
}

void DeferredRenderer::Render(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _lightBuffer, XMFLOAT4X4* _projMat, XMFLOAT4X4* _viewMat, ID3D11DepthStencilView* _dsv)
{
    std::vector<GameObject*> objects = ObjectManager::GetDeferredObjects();

    ShaderData shaders[] = { ShaderManager::shaderDAlbedo, ShaderManager::shaderDNormal, ShaderManager::shaderDPosition };
    ID3D11RenderTargetView* views[] = { rtvAlbedo, rtvNormals, rtvPosition };
    for (int i = 0; i < 3; i++) {
        _deviceContext->OMSetRenderTargets(1, &views[i], _dsv);    
        _deviceContext->ClearRenderTargetView(views[i], Colors::Black);
        _deviceContext->ClearDepthStencilView(_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
        for (int j = 0; j < objects.size(); j++) {
            objects.at(j)->SetShaders(shaders[i]);
            objects.at(j)->Draw(_deviceContext, _lightBuffer, _projMat, _viewMat);
        }
    }
}

void DeferredRenderer::Release()
{
    if (srvAlbedo) srvAlbedo->Release();
    if (srvNormals) srvNormals->Release();
    if (srvPosition) srvPosition->Release();

    if (rtvAlbedo) rtvAlbedo->Release();
    if (rtvNormals) rtvNormals->Release();
    if (rtvPosition) rtvPosition->Release();

    if (pTexAlbedo) pTexAlbedo->Release();
    if (pTexNormals) pTexNormals->Release();
    if (pTexPosition) pTexPosition->Release();
}
