#include "ObjectManager.h"

GameObjectPlane* ObjectManager::m_renderPlane;
std::vector<GameObject*> ObjectManager::m_objects;
std::vector<GameObject*> ObjectManager::m_deferredObjects;

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
    if (m_renderPlane)
        delete m_renderPlane;
    m_renderPlane = nullptr;
}

void ObjectManager::CreateObjects(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext)
{
    GameObjectCube* cube, *cube2;
    GameObjectBezierSpline* line;
    GameObjectTerrain* terrain;

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

    //m_terrain = new GameObjectTerrain((char*)"Resources\\XML\\terrain.xml");
    terrain = new GameObjectTerrain();
    terrain->DiamondSquare(16, 1, 2, 3, 4);
    terrain->InitMesh(_device, _deviceContext);
    terrain->SetShaders(ShaderManager::shaderTerrain);


    for (int i = 0; i < 10; i++) {
        GameObjectCube* o = new GameObjectCube();
        o->InitMesh(_device, _deviceContext);
        o->SetShaders(ShaderManager::shaderDAlbedo);
        o->SetPosition(XMFLOAT3(-10 + i + (i * 2), 0, 0));
        m_deferredObjects.push_back(o);
    }

    m_objects.push_back(terrain);

    m_objects.push_back(cube);
    m_objects.push_back(cube2);

}

void ObjectManager::Update(float _deltaTime)
{
    for (int i = 0; i < m_objects.size(); i++) {
        m_objects.at(i)->Update(_deltaTime);
    }

    for (int i = 0; i < m_deferredObjects.size(); i++) {
        m_deferredObjects.at(i)->Update(_deltaTime);
    }

}

void ObjectManager::Render(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _lightBuffer, XMFLOAT4X4* _projMat, XMFLOAT4X4* _viewMat)
{
    for (int i = 0; i < m_objects.size(); i++) {
        m_objects.at(i)->Draw(_deviceContext, _lightBuffer, _projMat, _viewMat);
    }
}