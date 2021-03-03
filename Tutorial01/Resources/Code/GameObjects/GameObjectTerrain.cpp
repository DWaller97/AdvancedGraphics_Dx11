#include "GameObjectTerrain.h"

GameObjectTerrain::GameObjectTerrain()
{
    SetWorldMatrix(new XMFLOAT4X4());
    NUM_VERTICES = 6;
    NUM_INDICES = 6;
}

GameObjectTerrain::~GameObjectTerrain()
{
    delete m_World;
    m_World = nullptr;
	if (m_vertices)
		delete m_vertices;
	if (m_indices)
		delete m_indices;
	m_vertices = nullptr;
	m_indices = nullptr;
    Release();
}

HRESULT GameObjectTerrain::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
    HRESULT hr;
	NUM_VERTICES = (m_terrainWidth * m_terrainLength * 6);
	NUM_INDICES = (m_terrainWidth * m_terrainLength * 6);
	m_vertices = new SimpleVertex[NUM_VERTICES];
	m_indices = new WORD[NUM_INDICES];

	for (int i = 0, vertCount = 0, indexCount = 0; i < m_terrainWidth; i++) {
		for (int j = 0; j < m_terrainLength; j++) {
			m_vertices[vertCount] = SimpleVertex
			{ XMFLOAT3(1.0f + (i * 1), 0.0f, 1.0f + j), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) };
			m_vertices[vertCount + 1] = SimpleVertex
			{ XMFLOAT3(-1.0f + (i * 1), 0.0f, -1.0f + j), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) };
			m_vertices[vertCount + 2] = SimpleVertex
			{ XMFLOAT3(1.0f + (i * 1), 0.0f, -1.0f + j), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) };

			m_vertices[vertCount + 3] = SimpleVertex
			{ XMFLOAT3(-1.0f + (i * 1), 0.0f, 1.0f + j), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) };
			m_vertices[vertCount + 4] = SimpleVertex
			{ XMFLOAT3(-1.0f + (i * 1), 0.0f, -1.0f + j), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) };
			m_vertices[vertCount + 5] = SimpleVertex
			{ XMFLOAT3(1.0f + (i * 1), 0.0f, 1.0f + j), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) };

			m_indices[indexCount] = vertCount + 1;
			m_indices[indexCount + 1] = vertCount;
			m_indices[indexCount + 2] = vertCount + 2;

			m_indices[indexCount + 3] = vertCount + 4;
			m_indices[indexCount + 4] = vertCount + 3;
			m_indices[indexCount + 5] = vertCount + 5;
			vertCount += 6, indexCount += 6;
		}
	}

	//{
	//	{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 6 // 6
	//	{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 4 // 7
	//	{ XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }, // 5 // 8

	//	{ XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }, // 7 // 9
	//	{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 4 // 10 
	//	{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 6 // 11
	//};

	//WORD m_indices[] = {
	//	// Front Face
	//	1, 0, 2,
	//	4, 3, 5
	//};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * NUM_VERTICES;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = m_vertices;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &mesh.VertexBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * NUM_INDICES;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = m_indices;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &mesh.IndexBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MaterialPropertiesConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pMaterialConstantBuffer);
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

	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Textures\\darkdirt.dds", nullptr, &m_albedoTexture);
	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Textures\\stone.dds", nullptr, &m_heightTexture);
	if (FAILED(hr))
		return hr;

    return hr;

}

void GameObjectTerrain::Update(float t)
{
	XMMATRIX mTranslate = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMMATRIX world = mTranslate;
	XMStoreFloat4x4(m_World, world);
}

void GameObjectTerrain::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
{
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_World));
	cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(viewMat));
	cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(projMat));
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	pContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);

	MaterialPropertiesConstantBuffer redPlasticMaterial;
	redPlasticMaterial.Material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	redPlasticMaterial.Material.Specular = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
	redPlasticMaterial.Material.SpecularPower = 32.0f;
	redPlasticMaterial.Material.UseTexture = true;
	pContext->UpdateSubresource(m_pMaterialConstantBuffer, 0, nullptr, &redPlasticMaterial, 0, 0);


	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &mesh.VertexBuffer, &stride, &offset);
	// Set index buffer
	pContext->IASetIndexBuffer(mesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pContext->IASetInputLayout(m_inputLayout);
	// Render the cube
	pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pContext->PSSetConstantBuffers(1, 1, &m_pMaterialConstantBuffer);
	pContext->VSSetShader(vertexShader, nullptr, 0);
	pContext->PSSetShader(pixelShader, nullptr, 0);

	//pContext->PSSetConstantBuffers(3, 1, &m_pCameraBuffer);
	pContext->PSSetShaderResources(0, 1, &m_albedoTexture);
	pContext->PSSetShaderResources(1, 1, &m_heightTexture);

	pContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	pContext->DrawIndexed(NUM_INDICES, 0, 0);

}

void GameObjectTerrain::SetSize(int _width, int _length)
{
	m_terrainWidth = _width;
	m_terrainLength = _length;
}

void GameObjectTerrain::LoadHeightMap(char* _fileName)
{
	// A height for each vertex 
	std::vector<unsigned char> in(513 * 513);

	// Open the file.
	std::ifstream inFile;
	inFile.open(_fileName, std::ios_base::binary);

	if (inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());
		// Done with file.
			inFile.close();
	}

	// Copy the array data into a float array and scale it. mHeightmap.resize(heightmapHeight * heightmapWidth, 0);

	for (UINT i = 0; i < 513 * 513; ++i)
	{
		m_heightMap[i] = (in[i] / 255.0f) * m_heightScale;
	}
}