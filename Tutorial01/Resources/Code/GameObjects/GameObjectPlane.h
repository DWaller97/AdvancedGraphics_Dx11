#pragma once
#include "GameObject.h"
class GameObjectPlane :
    public GameObject
{
private:
    ID3D11Buffer* ppBuff = nullptr;
public:
    struct SettingsBuffer {
        int InvertColours;
        int padding, padding1, padding2;
    };
    GameObjectPlane();
    ~GameObjectPlane();
    HRESULT						InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    virtual void Update(float t) override;
    virtual void Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;
    virtual void Release() override;
    static bool pp_invertColours;
};

