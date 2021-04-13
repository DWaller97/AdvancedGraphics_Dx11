#pragma once
#include "GameObject.h"
#include "../External/pugiXML/pugixml.hpp"
#include "../Managers/CameraManager.h"
#include <memory>
#include <random>
#include <cmath>
using namespace std;
class GameObjectTerrain :
    public GameObject
{
public:

    struct PixelTextureHeights {
        XMFLOAT4 albedoZeroToThreeHeight;
    };

    GameObjectTerrain(char* _fileName);
    GameObjectTerrain(int _seed = time(NULL));
    virtual ~GameObjectTerrain() override;
    HRESULT						virtual InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) override;
    
    void								virtual Update(float t) override;
    void								virtual Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat) override;

    void SetSize(int _width, int _length);
    void SetHeightmapScale(int _scale);

    void LoadHeightMap(char* _filePath);


    void DiamondSquare(UINT _size, int _randomness = 0, int _heightScale = 1, int _c1 = 0, int _c2 = 0, int _c3 = 0, int _c4 = 0);
    void FaultLine(UINT _size, int _iterations = 10, int _displacement = 16);
    void HillAlgorithm(int _size, int _minRadius, int _maxRadius, int _iterations);
    void SmoothHeights(int _boxSize, int _iterations);

    int GetSeed() { return m_seed; }
    int GetSize() { return m_terrainLength; }

    static void SetTessellationAmount(short _amount);
    static short* GetTessellationAmount() { return m_tessAmount; };


    /// <param name="_index">Zero to Four otherwise nullptr</param>
    /// <returns></returns>
    static float* GetTextureHeight(int _index);

    const double PI = 3.14159265359;
protected:
    void GenerateFlat(int _sizeX, int _sizeY);
    virtual void LoadFromXML(char* _filePath);
    vector<float> m_heightMap;
    vector<BasicVertex> m_vertices;
    vector<UINT> m_indices;
    int m_terrainWidth = 1;
    int m_terrainLength = 1;
    ID3D11ShaderResourceView* m_albedo0 = nullptr;
    ID3D11ShaderResourceView* m_albedo1 = nullptr;
    ID3D11ShaderResourceView* m_albedo2 = nullptr;
    ID3D11ShaderResourceView* m_albedo3 = nullptr;
    ID3D11ShaderResourceView* m_albedo4 = nullptr;
    ID3D11ShaderResourceView* m_albedoTextures[5];

    ID3D11Buffer* m_textureHeight = nullptr;

    static float* m_textureHeight0;
    static float* m_textureHeight1;
    static float* m_textureHeight2;
    static float* m_textureHeight3;

    int ConvertTo1D(int x, int y);
private:
    void Init();

    float Lerp(float _a, float _b, float _t);
    
    void SetHeights();

    bool IsInBounds(int _1DPos, int _1DMax, int _1DMin = 0);
    float CheckHeight(int _center, int _max, int _random);
    float CheckHeight(int _x, int _y);
    float CheckHeight(int _1DPos);
    bool IsOnSameLine(float _a, float _b, int _width);
    void Normalise(int _scale = 100);


    int m_heightScale = 10;







    static short* m_tessAmount;

    Camera* m_currCamera = nullptr;

    int m_seed = 0;


};

