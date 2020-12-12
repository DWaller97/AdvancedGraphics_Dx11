#include "ObjectManager.h"

std::vector<GameObject*> ObjectManager::objects;

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

    plane = new GameObjectPlane();
    plane->SetShaders(ShaderManager::shaderRTT);
    plane->SetPosition(XMFLOAT3(0, 0, 0));
    plane->InitMesh(_device, _deviceContext);

    obj = new GameObject();
    obj->InitMesh(_device, _deviceContext);
    obj->SetMesh((char*)"Resources/monkey.obj", _device, true);
    obj->SetShaders(ShaderManager::shaderStandard);
    obj->SetPosition(XMFLOAT3(0, 1, 5));

    objects.push_back(cube);
    objects.push_back(cube2);
    objects.push_back(obj);

}

void ObjectManager::Update(float _deltaTime)
{
    for (int i = 0; i < objects.size(); i++) {
        objects.at(i)->Update(_deltaTime);
    }
}

void ObjectManager::Render(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _lightBuffer, XMFLOAT4X4* _projMat, XMFLOAT4X4* _viewMat)
{
    for (int i = 0; i < objects.size(); i++) {
        objects.at(i)->Draw(_deviceContext, _lightBuffer, _projMat, _viewMat);
    }
}
