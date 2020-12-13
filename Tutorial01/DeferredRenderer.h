#pragma once
#include <d3d11.h>
class DeferredRenderer
{
private:
    ID3D11Texture2D* pTexAlbedo;
    ID3D11Texture2D* pTexPosition;
    ID3D11Texture2D* pTexNormals;
};

