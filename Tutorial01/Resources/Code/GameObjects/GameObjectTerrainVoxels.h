#pragma once
#include "GameObjectTerrain.h"
class GameObjectTerrainVoxels :
    public GameObjectTerrain
{
public:
    GameObjectTerrainVoxels(char* _filePath);
    virtual ~GameObjectTerrainVoxels() override;
    
    HRESULT	virtual InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;

    void		virtual Update(float t) override;
    void		virtual Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;

    virtual void LoadFromXML (char* _filePath) override;
    void GenerateFlat3D(int _sizeX, int _sizeY, int _sizeZ);

private:
    UINT m_terrainDepth;
    enum VoxelType {
        AIR = 0,
        WATER,
        SOLID
    };
    vector<VoxelType> m_voxels;
};

