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
    GameObjectTerrain(int _seed = time(NULL));
    virtual ~GameObjectTerrain() override;
    HRESULT						virtual InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    
    void								virtual Update(float t) override;
    void								virtual Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;

    void SetSize(int _width, int _length);
    void SetHeightmapScale(int _scale);

    void LoadHeightMap(char* _fileName);
    void DiamondSquare(UINT _size, int _randomness = 0, int _heightScale = 1, int _c1 = 0, int _c2 = 0, int _c3 = 0, int _c4 = 0);
    void FaultLine(UINT _size, int _iterations = 10, int _displacement = 16);
    void HillAlgorithm(int _size, int _minRadius, int _maxRadius, int _iterations);
    void SmoothHeights(int _boxSize, int _iterations);

    int GetSeed() { return m_seed; }
    int GetSize() { return m_terrainLength; }


    const double PI = 3.14159265359;
private:
    void InitialiseRandomTerrain(int _sizeX, int _sizeY);
    
    void LoadFromXML(char* _fileName);
    bool IsInBounds(int _1DPos, int _1DMax, int _1DMin = 0);
    float CheckHeight(int _center, int _max, int _random);
    float CheckHeight(int _x, int _y);
    bool IsOnSameLine(float _a, float _b, int _width);
    int ConvertTo1D(int x, int y);
    void Normalise(int _scale = 100);

    int m_terrainWidth = 1;
    int m_terrainLength = 1;
    int m_heightScale = 10;

    vector<float> m_heightMap;
    vector<BasicVertex> m_vertices;
    vector<UINT> m_indices;
    int m_seed = 0;


};

