#pragma once
#include <d3d11.h>
#include <vector>

#include "../Animation.h"

using namespace std;
class AnimationManager
{
public:
    static AnimationManager* GetReference();
    void AddModels(Animation::Model *_models, int _modelCount);
    void Initialise(ID3D11DeviceContext* _context);
    void Update(float _deltaTime);
private:
    vector<Animation::Model> m_models;
    static AnimationManager* m_reference;
    ID3D11DeviceContext* m_deviceContext;
};

