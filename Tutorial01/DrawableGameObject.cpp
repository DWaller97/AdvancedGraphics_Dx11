#include "DrawableGameObject.h"

using namespace std;
using namespace DirectX;

void DrawableGameObject::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
{
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_World));
	cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(viewMat));
	cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(projMat));
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	pContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	// Set index buffer
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// Render the cube
	pContext->VSSetShader(vertexShader, nullptr, 0);
	pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pContext->PSSetShader(pixelShader, nullptr, 0);

	pContext->PSSetConstantBuffers(1, 1, &m_pMaterialConstantBuffer);
	pContext->PSSetConstantBuffers(2, 1, &lightConstantBuffer);

	pContext->PSSetShaderResources(0, 1, &m_albedoTexture);
	pContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	pContext->DrawIndexed(NUM_VERTICES, 0, 0);
}

void DrawableGameObject::SetPosition(XMFLOAT3 position)
{
	m_position = position;
}

void DrawableGameObject::SetShader(ID3D11PixelShader* _pixelShader)
{
	pixelShader = _pixelShader;
}

void DrawableGameObject::SetShader(ID3D11VertexShader* _vertexShader)
{
	vertexShader = _vertexShader;
}

void DrawableGameObject::SetShaders(ID3D11VertexShader* _vertexShader, ID3D11PixelShader* _pixelShader)
{
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
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