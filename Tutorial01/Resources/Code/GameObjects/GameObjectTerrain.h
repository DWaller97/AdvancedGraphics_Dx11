#pragma once
#include "GameObject.h"
#include "../External/pugiXML/pugixml.hpp"
#include <memory>
#include <random>
using namespace std;
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
    void DiamondSquare(UINT _size, int _randomness = 0, int _heightScale = 1, int _c1 = 0, int _c2 = 0, int _c3 = 0, int _c4 = 0);
    void SmoothHeights(int _boxSize, int _iterations);

private:
    void LoadFromXML(char* _fileName);
    bool IsInBounds(int _1DPos, int _1DMax, int _1DMin = 0);
    float CheckHeight(int _center, int _max, int _random);
    float CheckHeight(int _x, int _y);
    int ConvertTo1D(int x, int y);

    int m_terrainWidth = 1;
    int m_terrainLength = 1;
    int m_heightScale = 10;

    vector<double> m_heightMap;
    vector<BasicVertex> m_vertices;
    vector<UINT> m_indices;



};

