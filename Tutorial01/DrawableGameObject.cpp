#include "DrawableGameObject.h"

using namespace std;
using namespace DirectX;

void DrawableGameObject::SetPosition(XMFLOAT3 position)
{
	m_position = position;
}

void DrawableGameObject::Release()
{
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	if (m_pIndexBuffer)
		m_pIndexBuffer->Release();

	if (m_pTextureResourceView)
		m_pTextureResourceView->Release();

	if (m_pSamplerLinear)
		m_pSamplerLinear->Release();
}

void DrawableGameObject::SetWorldMatrix(XMFLOAT4X4* world)
{
	m_World = world;
}