#pragma once

#include "GameObject.h"
#include "GameObjectPlane.h"
#include "GameObjectCube.h"
#include <vector>
#include <d3d11.h>
class ObjectManager
{
private:
    GameObjectCube* cube, *cube2;
    GameObjectPlane* plane;
    GameObject* obj;
    static std::vector<GameObject*> objects;
public:
    void CreateObjects(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext);
    void Update(float _deltaTime);
    void Render(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _lightBuffer, XMFLOAT4X4* _projMat, XMFLOAT4X4* _viewMat);
    static std::vector<GameObject*> GetObjects() { return objects; }
    GameObjectPlane* GetScreenPlane() { return plane; }
};
