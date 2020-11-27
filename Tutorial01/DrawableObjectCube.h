#pragma once
#include "DrawableGameObject.h"
class DrawableObjectCube :
    public DrawableGameObject
{
private:
    bool spinning = true;
public:
    DrawableObjectCube();
    ~DrawableObjectCube();
    HRESULT InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    void Update(float t) override;
    void SetSpin(bool spin);
    //void Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;
};

