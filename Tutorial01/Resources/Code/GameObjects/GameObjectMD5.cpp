#include "GameObjectMD5.h"

GameObjectMD5::GameObjectMD5(char* _filePath, ID3D11Device* _device) {
    m_modelData = new Animation::Model();
    string appended = "Resources\\Animation\\";
    appended+=_filePath;
    string meshPath = appended + ".md5mesh";
    string animPath = appended + ".md5anim";
    MD5Loader::LoadMesh((char*)meshPath.c_str(), m_modelData, _device);
    MD5Loader::LoadAnimation((char*)animPath.c_str(), m_modelData, _device);
    SetShaders(ShaderManager::shaderBasic);
}

GameObjectMD5::~GameObjectMD5()
{
    Release();
    if (m_modelData)
        delete m_modelData;
    m_modelData = nullptr;

}

HRESULT GameObjectMD5::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
    D3D11_BUFFER_DESC bd = {};
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    return pd3dDevice->CreateBuffer(&bd, nullptr, &m_constantBuffer);
}

void GameObjectMD5::Update(float t)
{
    GameObject::Update(t);
}

void GameObjectMD5::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
{
    for (int i = 0; i < m_modelData->subsetCount; i++) {
        pContext->IASetInputLayout(m_inputLayout);
        pContext->VSSetShader(m_vertexShader, nullptr, 0);
        pContext->PSSetShader(m_pixelShader, nullptr, 0);
        pContext->IASetIndexBuffer(m_modelData->subset[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        UINT stride = sizeof(BasicVertex);
        UINT offset = 0;
        pContext->IASetVertexBuffers(0, 1, &m_modelData->subset[i].vertexBuffer, &stride, &offset);
        pContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);

        ConstantBuffer cb1;
        cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_world));
        cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(viewMat));
        cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(projMat));
        cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
        pContext->UpdateSubresource(m_constantBuffer, 0, nullptr, &cb1, 0, 0);

        pContext->DrawIndexed(m_modelData->subset[i].indices.size(), 0, 0);
    }
}

void GameObjectMD5::Release()
{
    for (int i = 0; i < m_modelData->subset.size(); i++) {
        ID3D11Buffer* buff = m_modelData->subset[i].indexBuffer;
        if (buff)
            buff->Release();
        buff = nullptr;
        ID3D11Buffer* vertBuff = m_modelData->subset[i].vertexBuffer;
        if (vertBuff)
            vertBuff->Release();
        vertBuff = nullptr;
    }
}
