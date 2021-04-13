#include "ObjectManager.h"

GameObjectPlane* ObjectManager::m_renderPlane;
std::vector<GameObject*> ObjectManager::m_objects;
std::vector<GameObject*> ObjectManager::m_deferredObjects;
GameObjectTerrain* ObjectManager::m_terrain;
GameObjectTerrainVoxels* ObjectManager::m_terrain3D;

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
    if (m_renderPlane)
        delete m_renderPlane;
    m_renderPlane = nullptr;
    
    if (m_terrain)
        delete m_terrain;
    m_terrain = nullptr;
    
    if (m_rasteriserWF)
        m_rasteriserWF->Release();
    m_rasteriserWF = nullptr;

    if (m_rasteriserSolid)
        m_rasteriserSolid->Release();
    m_rasteriserSolid = nullptr;

    if (m_model)
        delete m_model;
    m_model = nullptr;
}

void ObjectManager::CreateObjects(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext)
{
    GameObjectCube* cube, *cube2;
    GameObjectBezierSpline* line;

    cube = new GameObjectCube();
    cube->InitMesh(_device, _deviceContext);
    cube->SetShaders(ShaderManager::shaderStandard);
    cube->SetPosition(XMFLOAT3(0, 0, 0));

    cube2 = new GameObjectCube();
    cube2->SetShaders(ShaderManager::shaderStandard);
    cube2->SetPosition(XMFLOAT3(5, 0, 0));
    cube2->InitMesh(_device, _deviceContext);

    m_renderPlane = new GameObjectPlane();
    m_renderPlane->InitMesh(_device, _deviceContext);
    m_renderPlane->SetShaders(ShaderManager::shaderRTT);
    m_renderPlane->SetPosition(XMFLOAT3(0, 0, 0));

    //m_terrain = new GameObjectTerrain();
    //m_terrain3D = new GameObjectTerrainVoxels((char*)"Resources\\XML\\terrain3d.xml");
    //m_terrain3D->SetShaders(ShaderManager::shaderBasic);
    //m_terrain3D->InitMesh(_device, _deviceContext);
    //Loads terrain from XML file, grabbing the path for the terrain raw file and loads it that way.
    //m_terrain = new GameObjectTerrain((char*)"Resources\\XML\\terrain.xml");
    //
    //m_terrain->DiamondSquare(512, 512);
    //m_terrain->SetHeightmapScale(1000);
    //m_terrain->SmoothHeights(1, 1);

    //I think this gives the best results, if lowering the size of the map, also lower the height modifier (at the end of the HillAlgorithm function), to about 1
    //m_terrain->HillAlgorithm(1024, 50, 60, 300);
    //m_terrain->SetHeightmapScale(10);
    
    //m_terrain->HillAlgorithm(512, 50, 8, 64);

    //I thought using a constant displacement value looked much better, however, it looks very blocky overrall.
    //m_terrain->FaultLine(2048, 600, 1);
    //m_terrain->FaultLine(512, 100, 1);
    ////
    //m_terrain->InitMesh(_device, _deviceContext);
    //m_terrain->SetShaders(ShaderManager::shaderTerrain);
    //m_deferredObjects.push_back(m_terrain);

    m_model = new Animation::Model();
    m_model->world = new XMFLOAT4X4();
    D3D11_BUFFER_DESC bd = {};
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    _device->CreateBuffer(&bd, nullptr, &m_model->constantBuffer);
    MD5Loader::LoadModel((char*)"Resources\\Animation\\bob_lamp_update.md5mesh", m_model, _device);
    for (int i = 0; i < 10; i++) {
        GameObjectCube* o = new GameObjectCube();
        o->InitMesh(_device, _deviceContext);
        o->SetShaders(ShaderManager::shaderDAlbedo);
        o->SetPosition(XMFLOAT3(-10 + i + (i * 2), 0, 0));
        m_deferredObjects.push_back(o);
    }

    //m_objects.push_back(m_terrain3D);

    //m_objects.push_back(cube);
    //m_objects.push_back(cube2);

}

HRESULT ObjectManager::CreateRasterisers(ID3D11Device* _device)
{
    HRESULT hr;
    D3D11_RASTERIZER_DESC desc = {};
    desc.FillMode = D3D11_FILL_WIREFRAME;
    desc.CullMode = D3D11_CULL_BACK;
    hr = _device->CreateRasterizerState(&desc, &m_rasteriserWF);
    if (FAILED(hr)) {
        return hr;
    }

    desc = {};
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_BACK;
    hr = _device->CreateRasterizerState(&desc, &m_rasteriserSolid);
    if (FAILED(hr)) {
        return hr;
    }

    return hr;
}

void ObjectManager::Update(float _deltaTime)
{
    //m_terrain->Update(_deltaTime);
    //m_terrain3D->Update(_deltaTime);
    XMMATRIX mTranslate = XMMatrixTranslation(0, 0, 0);
    XMMATRIX world = mTranslate;
    XMStoreFloat4x4(m_model->world, world);

    for (int i = 0; i < m_objects.size(); i++) {
        m_objects.at(i)->Update(_deltaTime);
    }

    for (int i = 0; i < m_deferredObjects.size(); i++) {
        m_deferredObjects.at(i)->Update(_deltaTime);
    }

}

void ObjectManager::Render(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _lightBuffer, XMFLOAT4X4* _projMat, XMFLOAT4X4* _viewMat)
{
    //_deviceContext->RSSetState(m_rasteriserWF);
    //m_terrain->Draw(_deviceContext, _lightBuffer, _projMat, _viewMat);
    _deviceContext->RSSetState(m_rasteriserSolid);
    //m_terrain3D->Draw(_deviceContext, _lightBuffer, _projMat, _viewMat);

    for (int i = 0; i < m_model->subsetCount; i++) {
        _deviceContext->IASetInputLayout(ShaderManager::shaderBasic._inputLayout);
        _deviceContext->VSSetShader(ShaderManager::shaderBasic._vertexShader, nullptr, 0);
        _deviceContext->PSSetShader(ShaderManager::shaderBasic._pixelShader, nullptr, 0);
        _deviceContext->IASetIndexBuffer(m_model->subset[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        UINT stride = sizeof(BasicVertex);
        UINT offset = 0;
        _deviceContext->IASetVertexBuffers(0, 1, &m_model->subset[i].vertexBuffer, &stride, &offset);
        _deviceContext->VSSetConstantBuffers(0, 1, &m_model->constantBuffer);

         ConstantBuffer cb1;
        cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_model->world));
        cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(_viewMat));
        cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(_projMat));
        cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
        _deviceContext->UpdateSubresource(m_model->constantBuffer, 0, nullptr, &cb1, 0, 0);




        _deviceContext->DrawIndexed(m_model->subset[i].indices.size(), 0, 0);
    }
    for (int i = 0; i < m_objects.size(); i++) {
        m_objects.at(i)->Draw(_deviceContext, _lightBuffer, _projMat, _viewMat);
    }
}