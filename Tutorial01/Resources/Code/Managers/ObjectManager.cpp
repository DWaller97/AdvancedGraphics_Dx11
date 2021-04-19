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
}

void ObjectManager::CreateObjects(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext)
{
    //GameObjectCube* cube, *cube2;

    //cube = new GameObjectCube();
    //cube->InitMesh(_device, _deviceContext);
    //cube->SetShaders(ShaderManager::shaderStandard);
    //cube->SetPosition(XMFLOAT3(0, 0, 0));

    //cube2 = new GameObjectCube();
    //cube2->SetShaders(ShaderManager::shaderStandard);
    //cube2->SetPosition(XMFLOAT3(5, 0, 0));
    //cube2->InitMesh(_device, _deviceContext);

    m_renderPlane = new GameObjectPlane();
    m_renderPlane->InitMesh(_device, _deviceContext);
    m_renderPlane->SetShaders(ShaderManager::shaderRTT);
    m_renderPlane->SetPosition(XMFLOAT3(0, 0, 0));

    GameObjectTerrain* terrainLoad = new GameObjectTerrain((char*)"Resources\\XML\\terrainload.xml");
    terrainLoad->InitMesh(_device, _deviceContext);
    terrainLoad->SetShaders(ShaderManager::shaderTerrain);

    GameObjectTerrain* terrainHill = new GameObjectTerrain((char*)"Resources\\XML\\terrainhill.xml");
    terrainHill->InitMesh(_device, _deviceContext);
    terrainHill->SetShaders(ShaderManager::shaderTerrain);
    m_objects.push_back(terrainHill);

    m_objects.push_back(terrainLoad);


    GameObjectTerrain* terrainFaultLine = new GameObjectTerrain((char*)"Resources\\XML\\terrainfaultline.xml");
    terrainFaultLine->InitMesh(_device, _deviceContext);
    terrainFaultLine->SetShaders(ShaderManager::shaderTerrain);
    m_objects.push_back(terrainFaultLine);

    GameObjectTerrain* terrainDiamondSquare = new GameObjectTerrain((char*)"Resources\\XML\\terraindiamondsquare.xml");
    terrainDiamondSquare->InitMesh(_device, _deviceContext);
    terrainDiamondSquare->SetShaders(ShaderManager::shaderTerrain);
    m_objects.push_back(terrainDiamondSquare);

    m_md5 = new GameObjectMD5((char*)"skeleton", _device);
    m_md5->SetPosition(XMFLOAT3(50, 0, 0));
    m_md5->InitMesh(_device, _deviceContext);
    AnimationManager::GetReference()->AddModels(m_md5->GetModel(), 1);
    m_objects.push_back(m_md5);

    GameObjectTerrainVoxels* terrain3D = new GameObjectTerrainVoxels((char*)"Resources\\XML\\terrain3d.xml");
    terrain3D->SetPosition(XMFLOAT3(-10, 0, 0));
    terrain3D->InitMesh(_device, _deviceContext);
    terrain3D->SetShaders(ShaderManager::shaderBasic);
    m_objects.push_back(terrain3D);

    //Old deferred stuff
    //for (int i = 0; i < 10; i++) {
    //    GameObjectCube* o = new GameObjectCube();
    //    o->InitMesh(_device, _deviceContext);
    //    o->SetShaders(ShaderManager::shaderDAlbedo);
    //    o->SetPosition(XMFLOAT3(-10 + i + (i * 2), 0, 0));
    //    m_deferredObjects.push_back(o);
    //}

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
    //m_terrain3D->Draw(_deviceContext, _lightBuffer, _projMat, _viewMat);

    _deviceContext->RSSetState(m_rasteriserWF);

    for (int i = 0; i < m_objects.size(); i++) {
        m_objects.at(i)->Draw(_deviceContext, _lightBuffer, _projMat, _viewMat);
        _deviceContext->RSSetState(m_rasteriserSolid);
    }

}