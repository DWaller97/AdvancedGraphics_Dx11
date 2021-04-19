#pragma once
#include <DirectXMath.h>
#include <vector>
#include <d3d11.h>
#include <string>
#include "Utilities\structures.h"
using namespace DirectX;
/*********************************************************************************************************
* MARKING SCHEME 2021
* CATEGORY: HUMAN SKELETAL STRUCTURE
* DESCRIPTION: The skeleton is made up of many parts which are represented here as structs.
* At the top of the hierarchy is the model which contains a list of joints/bones, model subset, 
* animations, as well as the current animation index and total numbers of the other variables.
* Below that are the model subsets which contain the vertices, indices, weights, positions, and their
* buffers. These are iterated through and drawn to the screen as their own objects. The joints/bones
* themselves contain the name given by the modelling program as well as its position, orientation,
* and parentID, which determines the hierarchy order.
* The animations themselves are separated into their own structure to allow for multiple animations
* per single model and contains higher level information for the specific animation such as the
* framerate it runs at, the amount of frames total, as well as the current animation time. Other
* information for  the animation such as the base joints/bones (which are the bones that all of the
* other joints/bones are based on) and the skeleton which is a 2d array of bones to represent each
* part of the animation is contained here.
* The structs between the mesh data and the animation data are different which is why there are 
* so many here (An .md5 file comes in two parts, one being .md5mesh and the other 
* .md5anim)
* [Part 1]
* *******************************************************************************************************/
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

