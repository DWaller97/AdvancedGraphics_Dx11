#pragma once
#include <DirectXMath.h>
#include <vector>
#include <d3d11.h>
#include <string>
#include "Utilities\structures.h"
using namespace DirectX;
class Animation
{
public:

    typedef unsigned int uint;

    struct Joint {
        std::string name;
        int parentID;
        XMFLOAT3 position;
        XMFLOAT4 orientation;
    };

    struct Weight {
        int jointID;
        float bias;
        XMFLOAT3 position;
    };

    struct ModelSubset {
        int textureArrayIndex;
        int indexCount;
        std::vector<BasicVertex> vertices;
        std::vector<uint> indices;
        std::vector<Weight> weights;
        std::vector<XMFLOAT3> positions;

        ID3D11Buffer* vertexBuffer;
        ID3D11Buffer* indexBuffer;
    };

    struct Model {
        int subsetCount;
        int jointCount;
        std::vector<Joint> joints;
        std::vector<ModelSubset> subset;
        XMFLOAT4X4* world;
        ID3D11Buffer* constantBuffer;
        ID3D11InputLayout* inputLayout;
        ID3D11PixelShader* pixelShader;
        ID3D11VertexShader* vertexShader;
    };

};

