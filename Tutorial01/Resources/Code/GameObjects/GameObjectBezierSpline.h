#pragma once
#include "GameObject.h"
class GameObjectBezierSpline :
    public GameObject
{
private:
public:
    GameObjectBezierSpline();
    ~GameObjectBezierSpline();
    virtual HRESULT InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    virtual void Update(float deltaTime) override;
    virtual void Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;
};

