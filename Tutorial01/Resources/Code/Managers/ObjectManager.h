#pragma once
#include <vector>
#include <d3d11.h>

#include "../GameObjects/GameObject.h"
#include "../GameObjects/GameObjectPlane.h"
#include "../GameObjects/GameObjectCube.h"
#include "../GameObjects/GameObjectBezierSpline.h"
#include "../GameObjects/GameObjectTerrain.h"

class ObjectManager
{
private:
    static GameObjectPlane* m_renderPlane;
    static std::vector<GameObject*> m_objects;
    static std::vector<GameObject*> m_deferredObjects;
public:
    ObjectManager();
    ~ObjectManager();
    void CreateObjects(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext);
    void Update(float _deltaTime);
    void Render(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _lightBuffer, XMFLOAT4X4* _projMat, XMFLOAT4X4* _viewMat);
    static std::vector<GameObject*> GetObjects() { return m_objects; }
    static std::vector<GameObject*> GetDeferredObjects() { return m_deferredObjects; }
    static GameObjectPlane* GetRenderPlane() { return m_renderPlane; }
};

