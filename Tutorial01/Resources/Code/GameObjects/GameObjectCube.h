#pragma once
#include "GameObject.h"
class GameObjectCube :
    public GameObject
{
private:
    bool spinning = true;
public:
    GameObjectCube();
    ~GameObjectCube();
    HRESULT InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    void Update(float t) override;
    void SetSpin(bool spin);
    void Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;
};

