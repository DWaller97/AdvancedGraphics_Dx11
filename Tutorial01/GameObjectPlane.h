#pragma once
#include "GameObject.h"
class GameObjectPlane :
    public GameObject
{
public:
    struct SettingsBuffer {
        int InvertColours;
        int padding, padding1, padding2;
    };
    ID3D11Buffer* ppBuff;
    GameObjectPlane();
    ~GameObjectPlane();
    HRESULT								InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    void								Update(float t) override;
    void								Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;
    static bool pp_invertColours;
};

