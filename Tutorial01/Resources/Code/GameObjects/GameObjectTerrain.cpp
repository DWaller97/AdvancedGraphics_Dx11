#include "GameObjectTerrain.h"

GameObjectTerrain::GameObjectTerrain(char* _fileName)
{
	NUM_VERTICES = 1;
    NUM_INDICES = 6;
	SetPosition(XMFLOAT3(-5, 0, -5));
	LoadFromXML(_fileName);

}

GameObjectTerrain::GameObjectTerrain()
{
	NUM_VERTICES = 1;
	NUM_INDICES = 6;
	SetPosition(XMFLOAT3(-5, 0, -5));
}

GameObjectTerrain::~GameObjectTerrain()
{
    Release();
}

HRESULT GameObjectTerrain::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
    HRESULT hr;

	for (int i = 0; i < m_terrainLength; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			float u = (float)i / m_terrainLength;
			float v = (float)j / m_terrainWidth;
			m_vertices.push_back({ XMFLOAT3(i, m_heightMap[(i *( m_terrainLength - 1)) + j] * m_heightScale, (j)), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(u, v) });
		}
	}
	for (int i = 0; i < m_terrainLength - 1; i++) {
		for (int j = 0; j < m_terrainWidth- 1; j++) {

			m_indices.push_back(i * m_terrainLength + j);
			m_indices.push_back(i * m_terrainLength + j + 1);
			m_indices.push_back(((i + 1) * m_terrainLength) + j);

			m_indices.push_back(((i + 1) * m_terrainLength) + j);
			m_indices.push_back((i * m_terrainLength) + j + 1);
			m_indices.push_back(((i + 1) * m_terrainLength) + j + 1);
		}
	}




	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(BasicVertex) * NUM_VERTICES;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData = {};

	InitData.pSysMem = &m_vertices[0];
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_mesh.VertexBuffer);
	if (FAILED(hr))
		return hr;


	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT) * NUM_INDICES;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &m_indices[0];
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

	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Textures\\darkdirt.dds", nullptr, &m_albedoTexture);
	if (FAILED(hr))
		return hr;

    return hr;

}

void GameObjectTerrain::Update(float t)
{
	GameObject::Update(t);
}

void GameObjectTerrain::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
{
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_world));
	cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(viewMat));
	cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(projMat));
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	pContext->UpdateSubresource(m_constantBuffer, 0, nullptr, &cb1, 0, 0);
	
	MaterialPropertiesConstantBuffer redPlasticMaterial;
	redPlasticMaterial.Material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	redPlasticMaterial.Material.Specular = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
	redPlasticMaterial.Material.SpecularPower = 32.0f;
	redPlasticMaterial.Material.UseTexture = true;
	pContext->UpdateSubresource(m_materialConstantBuffer, 0, nullptr, &redPlasticMaterial, 0, 0);


	// Set vertex buffer
	UINT stride = sizeof(BasicVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_mesh.VertexBuffer, &stride, &offset);
	// Set index buffer
	pContext->IASetIndexBuffer(m_mesh.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	pContext->IASetInputLayout(m_inputLayout);
	pContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	pContext->PSSetConstantBuffers(1, 1, &m_materialConstantBuffer);
	pContext->VSSetShader(m_vertexShader, nullptr, 0);
	pContext->PSSetShader(m_pixelShader, nullptr, 0);

	pContext->PSSetShaderResources(0, 1, &m_albedoTexture);

	pContext->PSSetSamplers(0, 1, &m_samplerLinear);
	pContext->DrawIndexed(NUM_INDICES, 0, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void GameObjectTerrain::SetSize(int _width, int _length)
{
	m_terrainWidth = _width;
	m_terrainLength = _length;
}

void GameObjectTerrain::LoadHeightMap(char* _fileName)
{
	// A height for each vertex 
	std::vector<unsigned char> in(m_terrainWidth * m_terrainLength);

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

	//m_heightMap = new long(m_terrainWidth * m_terrainLength);

	// Copy the array data into a float array and scale it. mHeightmap.resize(heightmapHeight * heightmapWidth, 0);
	int i = 0;
	for (; i < m_terrainWidth * m_terrainLength; ++i)
	{
		m_heightMap[i] = (in[i] / 255.0f) * 10;
	}
	

	//printf("Height map: %f\n Size:%f", m_heightMap.get()[m_terrainWidth * m_terrainLength - 1], m_terrainWidth * m_terrainLength);
}

void GameObjectTerrain::DiamondSquare(UINT _size, int _c1, int _c2, int _c3, int _c4)
{
	//2^n + 1
	_size += 1;
	//m_heightMap[_size * _size] = 0;
	m_terrainLength = _size;
	m_terrainWidth = _size;
	for (int i = 0; i < (_size * _size-1); i++) {
		m_heightMap.push_back(0);
	}

	NUM_VERTICES *= (m_terrainWidth * m_terrainLength);
	NUM_INDICES *= (m_terrainWidth * m_terrainLength);
	m_heightMap[0] = _c1;
	m_heightMap[ConvertTo1D(_size - 1, 0)] = _c2;
	//m_heightMap[ConvertTo1D(0, _size - 1)] = _c3;
	//m_heightMap[ConvertTo1D(_size - 1, _size - 1)] = _c4;
	//m_heightMap.get()[ConvertTo1D(_size / 2, _size / 2)] = 1;
	//int stepSize = _size - 1;
	//while (stepSize > 1) {
	//	int half = stepSize / 2;
	//	for (int i = 0; i < _size; i += stepSize) {
	//		for (int j = 0; j < _size; j += stepSize) {
	//			SquareStep(ConvertTo1D(i + half, j + half), stepSize);
	//		}
	//	}
	//	//for (int i = 0; i < m_terrainLength; i += stepSize) {
	//	//	for (int j = 0; j < m_terrainWidth; j += stepSize) {
	//	//		DiamondStep(ConvertTo1D(i + half, j), stepSize);
	//	//		DiamondStep(ConvertTo1D(i, j + half), stepSize);
	//	//	}
	//	//}
	//	stepSize /= 2;
	//}

}

void GameObjectTerrain::LoadFromXML(char* _fileName)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(_fileName);
	if (result.status != pugi::xml_parse_status::status_ok)
	{
		printf("Terrain file failed to open, error: %s", result.description());
	}
	pugi::xml_node node = doc.first_child();
	pugi::xml_attribute attr = node.first_attribute();
	m_terrainLength = attr.as_uint();
	attr = attr.next_attribute();
	m_terrainWidth = attr.as_uint();
	attr = attr.next_attribute();
	LoadHeightMap((char*)attr.as_string());
	attr = attr.next_attribute();
	m_heightScale = attr.as_uint();

}

void GameObjectTerrain::SquareStep(int _center, int _radius)
{
	int x = _center + ConvertTo1D(-_radius, -_radius);
	int y = _center + ConvertTo1D(_radius, -_radius);
	int z = _center + ConvertTo1D(-_radius, _radius);
	int w = _center + ConvertTo1D(_radius, _radius);


	int max = m_terrainLength * m_terrainWidth;
	
	if (_center >= max) {
		return;
	}


	int elements = 0;
	int average = 0;
	if (IsInBounds(x, max))
	{
		elements++;
		average += m_heightMap[x];
	}

	if (IsInBounds(y, max))
	{
		elements++;
		average += m_heightMap[y];
	}

	if (IsInBounds(z, max))
	{
		elements++;
		average += m_heightMap[z];
	}

	if (IsInBounds(w, max))
	{
		elements++;
		average += m_heightMap[w];
	}
	else {
		if (average <= 0 || elements <= 0)
			return;
	}

	average /= elements;

	m_heightMap[_center] = average;
}

void GameObjectTerrain::DiamondStep(int _center, int _radius)
{
	int x = _center + ConvertTo1D(0, -_radius);
	int y = _center + ConvertTo1D(_radius, 0);
	int z = _center + ConvertTo1D(-_radius, 0);
	int w = _center + ConvertTo1D(0, _radius);

	int max = m_terrainLength * m_terrainWidth;
	int elements = 0;
	int average = 0;
	if (IsInBounds(x, max))
	{
		elements++;
		average += m_heightMap[x];
	}

	if (IsInBounds(y, max))
	{
		elements++;
		average += m_heightMap[y];
	}

	if (IsInBounds(z, max))
	{
		elements++;
		average += m_heightMap[z];
	}

	if (IsInBounds(w, max))
	{
		elements++;
		average += m_heightMap[w];
	}

	average /= elements;

	m_heightMap[_center] = average;
}

bool GameObjectTerrain::IsInBounds(int _1DPos, int _1DMax, int _1DMin)
{
	if(_1DPos < _1DMin || _1DPos > _1DMax - 1)
		return false;
	return true;
}

int GameObjectTerrain::ConvertTo1D(int x, int y)
{
	return (y * ( m_terrainWidth) ) + x;
}
