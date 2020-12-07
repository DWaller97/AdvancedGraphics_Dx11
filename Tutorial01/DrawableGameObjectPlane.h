#pragma once
#include "DrawableGameObject.h"
class DrawableGameObjectPlane :
    public DrawableGameObject
{
public:
    HRESULT								virtual InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    void								virtual Update(float t) override;
    void								virtual Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;
};

