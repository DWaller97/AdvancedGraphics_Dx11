#pragma once
#include "DrawableGameObject.h"
class DrawableGameObjectPlane :
    public DrawableGameObject
{
public:
    DrawableGameObjectPlane();
    ~DrawableGameObjectPlane();
    HRESULT								InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    void								Update(float t) override;
    void								Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;
};

