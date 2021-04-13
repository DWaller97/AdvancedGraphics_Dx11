#pragma once
#include <vector>
#include <d3d11.h>

#include "../GameObjects/GameObject.h"
#include "../GameObjects/GameObjectPlane.h"
#include "../GameObjects/GameObjectCube.h"
#include "../GameObjects/GameObjectBezierSpline.h"
#include "../GameObjects/GameObjectTerrain.h"
#include "../GameObjects/GameObjectTerrainVoxels.h"

#include "../Animation.h"
#include "../Utilities/MD5Loader.h"

class ObjectManager
{
private:
    static GameObjectPlane* m_renderPlane;
    static GameObjectTerrain* m_terrain;
    static GameObjectTerrainVoxels* m_terrain3D;
    static std::vector<GameObject*> m_objects;
    static std::vector<GameObject*> m_deferredObjects;
    ID3D11RasterizerState *m_rasteriserWF = nullptr, *m_rasteriserSolid = nullptr;
    Animation::Model* m_model = nullptr;
public:
    ObjectManager();
    ~ObjectManager();
    void CreateObjects(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext);
    HRESULT CreateRasterisers(ID3D11Device* _device);
    void Update(float _deltaTime);
    void Render(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _lightBuffer, XMFLOAT4X4* _projMat, XMFLOAT4X4* _viewMat);
    static std::vector<GameObject*> GetObjects() { return m_objects; }
    static std::vector<GameObject*> GetDeferredObjects() { return m_deferredObjects; }
    static GameObjectPlane* GetRenderPlane() { return m_renderPlane; }
    static GameObjectTerrain* GetTerrain() { return m_terrain; }
};

