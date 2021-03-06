#include "GameObjectPlane.h"

bool GameObjectPlane::pp_invertColours = 0;


GameObjectPlane::GameObjectPlane()
{
	NUM_VERTICES = 4;
	NUM_INDICES = 6;
}

GameObjectPlane::~GameObjectPlane()
{

}

HRESULT GameObjectPlane::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr;

	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f,  1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f,  1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
	};

	WORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * NUM_VERTICES;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_mesh.VertexBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_mesh.IndexBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MaterialPropertiesConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_materialConstantBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_constantBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SettingsBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	SettingsBuffer buff;
	buff.InvertColours = pp_invertColours;

	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &ppBuff);
	if (FAILED(hr))
		return hr;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pd3dDevice->CreateSamplerState(&sampDesc, &m_samplerLinear);
}

void GameObjectPlane::Update(float t)
{
	GameObject::Update(t);
}

void GameObjectPlane::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
{
	SettingsBuffer buff;
	buff.InvertColours = pp_invertColours;


	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_world));
	cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(viewMat));
	cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(projMat));
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	
	pContext->UpdateSubresource(m_constantBuffer, 0, nullptr, &cb1, 0, 0);
	pContext->UpdateSubresource(ppBuff, 0, nullptr, &buff, 0, 0);


	MaterialPropertiesConstantBuffer redPlasticMaterial;
	redPlasticMaterial.Material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	redPlasticMaterial.Material.Specular = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
	redPlasticMaterial.Material.SpecularPower = 32.0f;
	redPlasticMaterial.Material.UseTexture = true;
	pContext->UpdateSubresource(m_materialConstantBuffer, 0, nullptr, &redPlasticMaterial, 0, 0);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_mesh.VertexBuffer, &stride, &offset);
	pContext->IASetIndexBuffer(m_mesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pContext->IASetInputLayout(m_inputLayout);

	pContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	pContext->PSSetConstantBuffers(1, 1, &ppBuff);
	pContext->PSSetConstantBuffers(2, 1, &m_materialConstantBuffer);
	pContext->PSSetConstantBuffers(3, 1, &lightConstantBuffer);
	pContext->VSSetShader(m_vertexShader, nullptr, 0);
	pContext->PSSetShader(m_pixelShader, nullptr, 0);
	pContext->PSSetShaderResources(0, 1, &m_albedoTexture);
	pContext->PSSetShaderResources(1, 1, &m_normalTexture);
	pContext->PSSetShaderResources(2, 1, &m_parallaxTexture);

	pContext->PSSetSamplers(0, 1, &m_samplerLinear);
	pContext->DrawIndexed(NUM_INDICES, 0, 0);


}

void GameObjectPlane::Release()
{
	if (ppBuff) {
		ppBuff->Release();
		delete ppBuff;
		ppBuff = nullptr;
	}
	GameObject::Release();
}

