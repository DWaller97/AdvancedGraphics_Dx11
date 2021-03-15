#pragma once
#include "GameObject.h"
class GameObjectTerrain :
    public GameObject
{
public:
    GameObjectTerrain();
    ~GameObjectTerrain();
    
    HRESULT						virtual InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    
    void								virtual Update(float t) override;
    void								virtual Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;

    void SetSize(int _width, int _length);
    void LoadHeightMap(char* _fileName);

private:
    int m_terrainWidth = 1;
    int m_terrainLength = 1;
    int m_heightScale = 1;

    float* m_heightMap;
    
    BasicVertex* m_vertices;
    UINT* m_indices;


    ID3D11ShaderResourceView* m_heightTexture;


};

