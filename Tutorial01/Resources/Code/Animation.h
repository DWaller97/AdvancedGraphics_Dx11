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

    struct BoundingBox {
        XMFLOAT3 min, max;
    };

    struct FrameData {
        int frameID;
        vector<float> frameData;
    };

    struct AnimJoint {
        string name;
        int parentID;
        int flags;
        int startIndex;
    };

    struct Anim {
        int frameCount;
        int jointCount;
        int framerate;
        int animatedCount;

        float frametime;
        float totalAnimTime;
        float currAnimTime;

        vector<AnimJoint> animJoints;
        vector<BoundingBox> bounds;
        vector<Joint> baseJoints;
        vector<FrameData> frameData;
        vector<vector<Joint>> frameSkeleton;
    };

    struct Model {
        int subsetCount;
        int jointCount;
        std::vector<Joint> joints;
        std::vector<ModelSubset> subset;
        vector<Anim> animations;
        int currAnimation;

    };
};

