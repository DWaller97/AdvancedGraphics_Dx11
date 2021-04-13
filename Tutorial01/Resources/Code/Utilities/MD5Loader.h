#pragma once
#include <fstream>
#include <string>
#include "../Animation.h"

using namespace std;

static class MD5Loader
{
public:
    static HRESULT LoadMesh(char* _filePath, Animation::Model* _modelOutput, ID3D11Device* _device);
    static HRESULT LoadAnimation(char* _filePath, Animation::Model* _modelOutput, ID3D11Device* _device);
    static void UpdateAnimation(Animation::Model* _modelOutput, ID3D11DeviceContext* _context, float _deltaTime, int _currAnimation);
};

