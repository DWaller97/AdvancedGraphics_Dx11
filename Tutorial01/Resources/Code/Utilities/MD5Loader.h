#pragma once
#include <fstream>
#include <string>
#include "../Animation.h"

using namespace std;

class MD5Loader
{
public:
    static HRESULT LoadModel(char* _filePath, Animation::Model* _modelOutput, ID3D11Device* _device);
    void Release();
};

