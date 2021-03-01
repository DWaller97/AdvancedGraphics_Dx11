#pragma once
#include <vector>
#include <d3d11.h>

#include "../GameObjects/GameObject.h"
#include "../GameObjects/GameObjectPlane.h"
#include "../GameObjects/GameObjectCube.h"
#include "../GameObjects/GameObjectBezierSpline.h"

class ObjectManager
{
private:
    GameObjectCube* cube, *cube2;
    GameObjectBezierSpline* line;
    static GameObjectPlane* plane;
    GameObject* obj;
    static std::vector<GameObject*> objects;
    static std::vector<GameObject*> deferredObjects;
public:
    void CreateObjects(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext);
    void Update(float _deltaTime);
    void Render(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _lightBuffer, XMFLOAT4X4* _projMat, XMFLOAT4X4* _viewMat);
    static std::vector<GameObject*> GetObjects() { return objects; }
    static std::vector<GameObject*> GetDeferredObjects() { return deferredObjects; }
    static GameObjectPlane* GetScreenPlane() { return plane; }
};

