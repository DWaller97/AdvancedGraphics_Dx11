#include "MD5Loader.h"

HRESULT MD5Loader::LoadMesh(char* _filePath, Animation::Model* _modelOutput, ID3D11Device* _device)
{
    HRESULT hr = S_OK;
    fstream stream(_filePath);
    if (stream.fail()) {
        MessageBox(0, L".MD5mesh failed to open", L"Error", MB_OK);
        return E_ABORT;
    }

    string currString;

    while (stream) {
        stream >> currString;

        if (currString == "numJoints") {
            stream >> _modelOutput->jointCount;
            continue;
        }
        else if (currString == "numMeshes") {
            stream >> _modelOutput->subsetCount;
            continue;
        }
        else if (currString == "MD5Version") {
            continue;
        }
        else if (currString == "commandline") {
            continue;
        }
        else if (currString == "joints") {
            Animation::Joint joint{};
            stream >> currString;
            for (int i = 0; i < _modelOutput->jointCount; i++) {
                stream >> joint.name;
                if (joint.name[joint.name.size() - 1] != '"') { //Checking to make sure that the entire name is stored since it uses quotation marks and can have beginning whitespace.
                    char currChar;
                    while (1) {
                        currChar = stream.get();
                        joint.name += currChar;
                        if (currChar == '"')
                            break;
                    }
                }
                joint.name.erase(0, 1);
                joint.name.erase(joint.name.size() - 1);

                stream >> joint.parentID;
                stream >> currString;
                stream >> joint.position.x >> joint.position.z >> joint.position.y;

                stream >> currString >> currString;
                stream >> joint.orientation.x >> joint.orientation.z >> joint.orientation.y;

                float w = 1.0f - (joint.orientation.x * joint.orientation.x) - (joint.orientation.y * joint.orientation.y) - (joint.orientation.z * joint.orientation.z);
                if (w < 0.0f)
                    joint.orientation.w = 0.0f;
                else
                    joint.orientation.w = -sqrtf(w);

                getline(stream, currString);
                _modelOutput->joints.push_back(joint);
            }
        }
        else if (currString == "mesh") {
            Animation::ModelSubset subset = {};
            stream >> currString >> currString;
            while (currString != "}") {
                if (currString == "shader") {
                    //Not dealing with this right now.
                    //Need a TGA loader or a mesh that uses a texture I can load
                    stream >> currString >> currString;
                }

                else if (currString == "numverts") {
                    int vertexCount;
                    stream >> vertexCount;
                    getline(stream, currString);
                    for (int i = 0; i < vertexCount; i++) {
                        BasicVertex v = {};
                        stream >> currString >> currString >> currString;

                        stream >> v.texCoord.x >> v.texCoord.y;
                        stream >> currString;
                        stream >> v.startWeight >> v.weightCount;

                        getline(stream, currString);
                        subset.vertices.push_back(v);
                    }
                }

                else if (currString == "numtris") {
                    stream >> subset.indexCount;
                    getline(stream, currString);
                    for (int i = 0; i < subset.indexCount; i++) {
                        UINT index;
                        stream >> currString;
                        stream >> currString;
                        for (int j = 0; j < 3; j++) {
                            stream >> index;
                            subset.indices.push_back(index);
                        }
                        getline(stream, currString);
                    }
                }

                else if (currString == "numweights") {
                    int weightCount = 0;
                    stream >> weightCount;
                    getline(stream, currString);
                    for (int i = 0; i < weightCount; i++) {
                        Animation::Weight weight = {};
                        stream >> currString >> currString;
                        stream >> weight.jointID;
                        stream >> weight.bias;
                        stream >> currString;
                        stream >> weight.position.x >> weight.position.z >> weight.position.y;
                        getline(stream, currString);
                        subset.weights.push_back(weight);
                    }
                }
                else {
                    stream >> currString;
                    //getline(stream, currString);
                }

            }
            _modelOutput->subset.push_back(subset);
        }
    }
    return hr;
}

HRESULT MD5Loader::LoadAnimation(char* _filePath, Animation::Model* _modelOutput, ID3D11Device* _device)
{
    HRESULT hr = S_OK;
    Animation::Anim anim;
    fstream stream(_filePath);
    if (stream.fail()) {
        MessageBox(0, L".MD5anim failed to open", L"Error", MB_OK);
        return E_ABORT;
    }
    string currString;
    while (stream) {
        stream >> currString;
        if (currString == "MD5Version") {
            stream >> currString;
        }
        else if (currString == "commandline") {
            getline(stream, currString);
        }
        else if (currString == "numFrames") {
            stream >> anim.frameCount;
        }
        else if (currString == "numJoints") {
            stream >> anim.jointCount;
        }
        else if (currString == "frameRate") {
            stream >> anim.framerate;
        }
        else if (currString == "numAnimatedComponents") {
            stream >> anim.animatedCount;
        }
        else if (currString == "hierarchy") {
            stream >> currString;
            for (int i = 0; i < anim.jointCount; i++) {
                Animation::AnimJoint animJoint = {};
                stream >> animJoint.name;
                if (animJoint.name[animJoint.name.size() - 1] != '"') { //Checking to make sure that the entire name is stored since it uses quotation marks and can have beginning whitespace.
                    char currChar;
                    while (1) {
                        currChar = stream.get();
                        animJoint.name += currChar;
                        if (currChar == '"')
                            break;
                    }
                }
                animJoint.name.erase(0, 1);
                animJoint.name.erase(animJoint.name.size() - 1);

                stream >> animJoint.parentID;
                stream >> animJoint.flags;
                stream >> animJoint.startIndex;

                bool jointMatch = false;
                for (int j = 0; j < _modelOutput->jointCount; j++) {
                    if (_modelOutput->joints[j].name == animJoint.name) {
                        if (_modelOutput->joints[j].parentID == animJoint.parentID) {
                            anim.animJoints.push_back(animJoint);
                            jointMatch = true;
                        }
                    }
                }

                if (!jointMatch) {
                    MessageBox(0, L".MD5anim joint mismatch", L"Error", MB_OK);
                    return E_ABORT;
                }
                getline(stream, currString);

            }
        }
        else if (currString == "bounds") {
            stream >> currString;
            for (int i = 0; i < anim.frameCount; i++) {
                Animation::BoundingBox bb = {};
                stream >> currString;
                stream >> bb.min.x >> bb.min.y >> bb.min.z;
                stream >> currString >> currString;
                stream >> bb.max.x >> bb.max.y >> bb.max.z;
                stream >> currString;
                anim.bounds.push_back(bb);
            }
        }
        else if (currString == "baseframe") {
            stream >> currString;
            for (int i = 0; i < anim.jointCount; i++) {
                Animation::Joint joint = {};
                stream >> currString;
                stream >> joint.position.x >> joint.position.z >> joint.position.y;
                stream >> currString >> currString;
                stream >> joint.orientation.x >> joint.orientation.z >> joint.orientation.y;
                stream >> currString;
                anim.baseJoints.push_back(joint);
            }
        }
        else if (currString == "frame") {
            Animation::FrameData frameData = {};
            stream >> frameData.frameID;
            stream >> currString;
            for (int i = 0; i < anim.animatedCount; i++) {
                float data;
                stream >> data;
                frameData.frameData.push_back(data);
            }
            anim.frameData.push_back(frameData);
            vector<Animation::Joint> skeleton;
            for (int i = 0, j = 0; i < anim.animJoints.size(); i++, j = 0) {

                Animation::Joint frameJoint = anim.baseJoints[i];
                frameJoint.parentID = anim.animJoints[i].parentID;

                if (anim.animJoints[i].flags & 1)
                    frameJoint.position.x = frameData.frameData[anim.animJoints[i].startIndex + j++];
                if (anim.animJoints[i].flags & 2)
                    frameJoint.position.z = frameData.frameData[anim.animJoints[i].startIndex + j++];
                if (anim.animJoints[i].flags & 4)
                    frameJoint.position.y = frameData.frameData[anim.animJoints[i].startIndex + j++];
                if (anim.animJoints[i].flags & 8)
                    frameJoint.orientation.x = frameData.frameData[anim.animJoints[i].startIndex + j++];
                if (anim.animJoints[i].flags & 16)
                    frameJoint.orientation.z = frameData.frameData[anim.animJoints[i].startIndex + j++];
                if (anim.animJoints[i].flags & 32)
                    frameJoint.orientation.y = frameData.frameData[anim.animJoints[i].startIndex + j++];

                float w = 1.0f - (frameJoint.orientation.x * frameJoint.orientation.x) - (frameJoint.orientation.y * frameJoint.orientation.y) - (frameJoint.orientation.z * frameJoint.orientation.z);
                if (w < 0.0f) {
                    frameJoint.orientation.w = 0.0f;
                }
                else {
                    frameJoint.orientation.w = -sqrtf(w);
                }

                if (frameJoint.parentID >= 0) {
                    Animation::Joint parentJoint = skeleton[frameJoint.parentID];

                    XMVECTOR parentJointOrientation = XMVectorSet(parentJoint.orientation.x, parentJoint.orientation.y, parentJoint.orientation.z, parentJoint.orientation.w);
                    XMVECTOR jointPos = XMVectorSet(frameJoint.position.x, frameJoint.position.y, frameJoint.position.z, 0.0f);
                    XMVECTOR parentOrientationConjugate = XMVectorSet(-parentJoint.orientation.x, -parentJoint.orientation.y, -parentJoint.orientation.z, parentJoint.orientation.w);

                    XMFLOAT3 rotatedPos;
                    XMStoreFloat3(&rotatedPos, XMQuaternionMultiply(XMQuaternionMultiply(parentJointOrientation, jointPos), parentOrientationConjugate));

                    frameJoint.position.x = rotatedPos.x + parentJoint.position.x;
                    frameJoint.position.y = rotatedPos.y + parentJoint.position.y;
                    frameJoint.position.z = rotatedPos.z + parentJoint.position.z;

                    XMVECTOR jointOrientation = XMVectorSet(frameJoint.orientation.x, frameJoint.orientation.y, frameJoint.orientation.z, frameJoint.orientation.w);
                    jointOrientation = XMQuaternionMultiply(parentJointOrientation, jointOrientation);
                    jointOrientation = XMQuaternionNormalize(jointOrientation);

                    XMStoreFloat4(&frameJoint.orientation, jointOrientation);
                }
                skeleton.push_back(frameJoint);
            }
            anim.frameSkeleton.push_back(skeleton);
            stream >> currString;
        }
    }
    anim.frametime = 1.0f / anim.framerate;
    anim.totalAnimTime = anim.frameCount * anim.frametime;
    anim.currAnimTime = 0.0f;
    _modelOutput->animations.push_back(anim);
        return hr;
}