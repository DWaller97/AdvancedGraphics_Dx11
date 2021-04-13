#include "MD5Loader.h"

HRESULT MD5Loader::LoadModel(char* _filePath, Animation::Model* _modelOutput, ID3D11Device* _device)
{
    HRESULT hr = S_OK;
    fstream stream(_filePath);
    if (stream.fail()) {
        MessageBox(0, L"MD5 failed to open", L"Error", MB_OK);
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
        else if(currString == "mesh") {
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
            for (int i = 0; i < subset.vertices.size(); i++) {
                BasicVertex v = subset.vertices.at(i);
                v.pos = XMFLOAT3(0, 0, 0);
                for (int j = 0; j < v.weightCount; j++) {
                    Animation::Weight weight = subset.weights[v.startWeight + j];
                    Animation::Joint joint = _modelOutput->joints[weight.jointID];
                    XMVECTOR jointOrientation = XMVectorSet(joint.orientation.x, joint.orientation.y, joint.orientation.z, joint.orientation.w);
                    XMVECTOR weightPos = XMVectorSet(weight.position.x, weight.position.y, weight.position.z, 0.0f);
                    XMVECTOR jointOrientationConjugate = XMVectorSet(-joint.orientation.x, -joint.orientation.y, -joint.orientation.z, joint.orientation.w);
                    XMFLOAT3 rotatedPoint;
                    XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(jointOrientation, weightPos), jointOrientationConjugate));

                    v.pos.x += (joint.position.x + rotatedPoint.x) * weight.bias;
                    v.pos.y += (joint.position.y + rotatedPoint.y) * weight.bias;
                    v.pos.z += (joint.position.z + rotatedPoint.z) * weight.bias;

                }
                subset.positions.push_back(v.pos);
            }
            for (int i = 0; i < subset.vertices.size(); i++) {
                subset.vertices.at(i).pos = subset.positions.at(i);
            }
            //Calculate normals here

            HRESULT hr;

            D3D11_BUFFER_DESC desc = {};
            ZeroMemory(&desc, sizeof(desc));
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.ByteWidth = sizeof(UINT) * subset.indexCount * 3;
            desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;

            D3D11_SUBRESOURCE_DATA initData;
            ZeroMemory(&initData, sizeof(initData));

            initData.pSysMem = &subset.indices[0];
            hr = _device->CreateBuffer(&desc, &initData, &subset.indexBuffer);
            if (hr == E_FAIL)
                return E_FAIL;
            ZeroMemory(&desc, sizeof(desc));
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.ByteWidth = sizeof(BasicVertex) * subset.vertices.size();
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;

            ZeroMemory(&initData, sizeof(initData));
            initData.pSysMem = &subset.vertices[0];
            
            hr = _device->CreateBuffer(&desc, &initData, &subset.vertexBuffer);
            if (hr == E_FAIL)
                return E_FAIL;
            _modelOutput->subset.push_back(subset);

        }
    }
    return hr;
}

void MD5Loader::Release()
{
}
