#include "ObjectManager.h"

std::vector<GameObject*> ObjectManager::objects;
std::vector<GameObject*> ObjectManager::deferredObjects;
GameObjectPlane* ObjectManager::m_renderPlane;

void ObjectManager::CreateObjects(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext)
{

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
    m_terrain = new GameObjectTerrain();
    m_terrain->DiamondSquare(16, 1, 2, 3, 4);
    m_terrain->InitMesh(_device, _deviceContext);
    m_terrain->SetShaders(ShaderManager::shaderTerrain);

    for (int i = 0; i < 10; i++) {
        GameObjectCube* o = new GameObjectCube();
        o->InitMesh(_device, _deviceContext);
        o->SetShaders(ShaderManager::shaderDAlbedo);
        o->SetPosition(XMFLOAT3(-10 + i + (i * 2), 0, 0));
        deferredObjects.push_back(o);
    }

    objects.push_back(m_terrain);

    objects.push_back(cube);
    objects.push_back(cube2);

}

void ObjectManager::Update(float _deltaTime)
{
    for (int i = 0; i < objects.size(); i++) {
        objects.at(i)->Update(_deltaTime);
    }

    for (int i = 0; i < deferredObjects.size(); i++) {
        deferredObjects.at(i)->Update(_deltaTime);
    }

}

void ObjectManager::Render(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _lightBuffer, XMFLOAT4X4* _projMat, XMFLOAT4X4* _viewMat)
{
    for (int i = 0; i < objects.size(); i++) {
        objects.at(i)->Draw(_deviceContext, _lightBuffer, _projMat, _viewMat);
    }
}