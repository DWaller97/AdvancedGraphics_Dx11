#pragma once
#include "DrawableGameObject.h"
class DrawableObjectCube :
    public DrawableGameObject
{
public:
    DrawableObjectCube(DirectX::XMFLOAT4X4* world);
    ~DrawableObjectCube();
    HRESULT InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    void Update(float t) override;
    void Draw(ID3D11DeviceContext* pContext) override;
};

