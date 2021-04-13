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

    m_md5 = new GameObjectMD5((char*)"Resources\\Animation\\bob_lamp_update.md5mesh", _device);
    m_md5->InitMesh(_device, _deviceContext);

    m_objects.push_back(m_md5);
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


    for (int i = 0; i < m_objects.size(); i++) {
        m_objects.at(i)->Draw(_deviceContext, _lightBuffer, _projMat, _viewMat);
    }
}