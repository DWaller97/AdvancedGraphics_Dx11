#include "DrawableGameObjectPlane.h"

DrawableGameObjectPlane::DrawableGameObjectPlane()
{
	SetWorldMatrix(new XMFLOAT4X4());
	NUM_VERTICES = 6;
}

DrawableGameObjectPlane::~DrawableGameObjectPlane()
{
	delete m_World;
	m_World = nullptr;
	Release();
}

HRESULT DrawableGameObjectPlane::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr;

	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },

	};

	WORD indices[] = {
		0,1,2,
		0,3,2
	};

	
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * NUM_VERTICES;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &mesh.VertexBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &mesh.IndexBuffer);
	if (FAILED(hr))
		return hr;


	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
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
	hr = pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);


	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\color.dds", nullptr, &m_albedoTexture);
	if (FAILED(hr))
		return hr;
}

void DrawableGameObjectPlane::Update(float t)
{
	XMMATRIX mTranslate = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMMATRIX world = mTranslate;
	XMStoreFloat4x4(m_World, world);
}

void DrawableGameObjectPlane::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
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
	pContext->IASetVertexBuffers(0, 1, &mesh.VertexBuffer, &stride, &offset);
	// Set index buffer
	pContext->IASetIndexBuffer(mesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pContext->IASetInputLayout(m_inputLayout);
	pContext->VSSetShader(vertexShader, nullptr, 0);
	pContext->PSSetShader(pixelShader, nullptr, 0);
	pContext->PSSetShaderResources(0, 1, &m_albedoTexture);
	pContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	pContext->DrawIndexed(NUM_VERTICES, 0, 0);
}
