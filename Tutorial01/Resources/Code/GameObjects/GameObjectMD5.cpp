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
    HRESULT hr = S_OK;
    m_context = pContext;
    D3D11_BUFFER_DESC bd = {};
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_constantBuffer);

    for (int i = 0; i < m_modelData->subset.size(); i++) {
        Animation::ModelSubset subset = m_modelData->subset[i];
        for (int j = 0; j < subset.vertices.size(); j++) {
            BasicVertex v = subset.vertices[j];
            v.pos = XMFLOAT3(0, 0, 0);
            for (int k = 0; k < v.weightCount; k++) {
                Animation::Weight weight = subset.weights[v.startWeight + k];
                Animation::Joint joint = m_modelData->joints[weight.jointID];
                XMVECTOR jointOrientation = XMVectorSet(joint.orientation.x, joint.orientation.y, joint.orientation.z, joint.orientation.w);
                XMVECTOR weightPos = XMVectorSet(weight.position.x, weight.position.y, weight.position.z, 0.0f);
                XMVECTOR jointOrientationConjugate = XMVectorSet(-joint.orientation.x, -joint.orientation.y, -joint.orientation.z, joint.orientation.w);
                XMFLOAT3 rotatedPoint;
                XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(jointOrientation, weightPos), jointOrientationConjugate));

                v.pos.x += (joint.position.x + rotatedPoint.x) * weight.bias;
                v.pos.y += (joint.position.y + rotatedPoint.y) * weight.bias;
                v.pos.z += (joint.position.z + rotatedPoint.z) * weight.bias;

            }
            subset.positions.push_back(v.pos);
        }
        for (int j = 0; j < subset.vertices.size(); j++) {
            subset.vertices.at(j).pos = subset.positions.at(j);
        }
        //Calculate normals here

        D3D11_BUFFER_DESC desc = {};
        ZeroMemory(&desc, sizeof(desc));
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(UINT) * subset.indexCount * 3;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(initData));

        initData.pSysMem = &subset.indices[0];
        hr = pd3dDevice->CreateBuffer(&desc, &initData, &subset.indexBuffer);
        if (hr == E_FAIL)
            return E_FAIL;
        ZeroMemory(&desc, sizeof(desc));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = sizeof(BasicVertex) * subset.vertices.size();
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;

        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = &subset.vertices[0];

        hr = pd3dDevice->CreateBuffer(&desc, &initData, &subset.vertexBuffer);
        if (hr == E_FAIL)
            return E_FAIL;
        m_modelData->subset[i] = subset;

    }
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
