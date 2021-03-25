#pragma once
#include "GameObject.h"
#include "../External/pugiXML/pugixml.hpp"
class GameObjectTerrain :
    public GameObject
{
public:
    GameObjectTerrain(char* _fileName);
    GameObjectTerrain();
    ~GameObjectTerrain();
    HRESULT						virtual InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    
    void								virtual Update(float t) override;
    void								virtual Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;

    void SetSize(int _width, int _length);
    void LoadHeightMap(char* _fileName);
    void DiamondSquare(UINT _size, int _c1 = 0, int _c2 = 0, int _c3 = 0, int _c4 = 0);
private:
    void LoadFromXML(char* _fileName);
    void SquareStep(int _center, int _radius);
    void DiamondStep(int _center, int _radius);

    int m_terrainWidth = 1;
    int m_terrainLength = 1;
    int m_heightScale = 1;

    double* m_heightMap;
    
    BasicVertex* m_vertices;
    UINT* m_indices;



};

