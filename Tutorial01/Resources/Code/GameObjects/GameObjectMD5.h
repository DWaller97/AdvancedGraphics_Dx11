#pragma once
#include "GameObject.h"
#include "../Utilities/MD5Loader.h"
#include "../Animation.h"
class GameObjectMD5 :
    public GameObject
{
public:
    GameObjectMD5(char* _filePath, ID3D11Device* _device);
    virtual ~GameObjectMD5() override;

    HRESULT virtual InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
    void virtual Update(float t);
    void	virtual Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat);

    void	virtual Release() override;
private:
    Animation::Model* m_modelData;
    ID3D11DeviceContext* m_context;
};

