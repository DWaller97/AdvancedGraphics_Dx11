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
	//m_terrainLength = 1;
	//m_terrainWidth = 1;
	//NUM_INDICES = 6;
	//NUM_VERTICES = 4;
	//BasicVertex b1, b2, b3, b4;
	//b1.pos = XMFLOAT3(0, /*m_heightMap[(i * (m_terrainLength)) + j] * m_heightScale*/ 0, 0);
	//b1.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//b1.texCoord = XMFLOAT2(0, 0);

	//b2.pos = XMFLOAT3(1, /*m_heightMap[(i * (m_terrainLength)) + j] * m_heightScale*/ 0, 0);
	//b2.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//b2.texCoord = XMFLOAT2(0, 0);

	//b3.pos = XMFLOAT3(0, /*m_heightMap[(i * (m_terrainLength)) + j] * m_heightScale*/ 0, 1);
	//b3.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//b3.texCoord = XMFLOAT2(0, 0);

	//b4.pos = XMFLOAT3(1, /*m_heightMap[(i * (m_terrainLength)) + j] * m_heightScale*/ 0, 1);
	//b4.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//b4.texCoord = XMFLOAT2(0, 0);


	//m_vertices.push_back(b1);
	//m_vertices.push_back(b2);
	//m_vertices.push_back(b3);
	//m_vertices.push_back(b4);

	//m_indices.push_back(2);
	//m_indices.push_back(1);
	//m_indices.push_back(0);

	//m_indices.push_back(3);
	//m_indices.push_back(1);
	//m_indices.push_back(2);
	m_heightScale = 1;
	for (int i = 0; i < m_terrainLength; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			float u = (float)i / m_terrainLength;
			float v = (float)j / m_terrainWidth;
			BasicVertex b;
			b.pos = XMFLOAT3(i * 10, m_heightMap[ConvertTo1D(i, j)] * m_heightScale, j * 10);
			b.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			b.texCoord = XMFLOAT2(u, v);
			m_vertices.push_back(b);
		}
	}
	for (int i = 0; i < m_terrainLength - 1; i++) {
		for (int j = 0; j < m_terrainWidth - 1; j++) {

			m_indices.push_back(i * m_terrainLength + j);
			m_indices.push_back(i * m_terrainLength + j + 1);
			m_indices.push_back((i + 1) * m_terrainLength + j);

			m_indices.push_back((i + 1)  * m_terrainLength + j);
			m_indices.push_back(i * m_terrainLength + j + 1);
			m_indices.push_back((i + 1) * m_terrainLength + j + 1);
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
	for (int i = 0; i < (_size * _size); i++) {
		m_heightMap.push_back(0);
	}

	int width = m_terrainWidth - 1;
	int length = m_terrainLength - 1;

	int totalSize = m_terrainWidth * m_terrainLength;
	int size = totalSize - 1;

	NUM_VERTICES *= (m_terrainWidth * m_terrainLength);
	NUM_INDICES *= (m_terrainWidth * m_terrainLength);
	m_heightMap[0] = _c1;
	m_heightMap[width] = _c2;
	m_heightMap[totalSize - 1 - length] = _c3;
	m_heightMap[totalSize - 1] = _c4;
	//m_heightMap[totalSize / 2] = 1;
	int stepSize = _size - 1;
	//DiamondStep(size / 2 - 1, size / 2 - 1);
	//SquareStep(ConvertTo1D(i + half, j))
	int half = stepSize;
	int randomSize = 256;
	while (stepSize > 1) {
		half = stepSize / 2;
		for (int i = 0; i < m_terrainLength - 1; i += stepSize) {
			for (int j = 0; j < m_terrainWidth - 1; j += stepSize) {
				float average = 0;
				float x = CheckHeight(ConvertTo1D(i, j), totalSize, randomSize);
				float y = CheckHeight(ConvertTo1D(i + stepSize, j), totalSize, randomSize);
				float z = CheckHeight(ConvertTo1D(i, j + stepSize), totalSize, randomSize);
				float w = CheckHeight(ConvertTo1D(i + stepSize, j + stepSize), totalSize, randomSize);
				average = x + y + z + w;
				average /= 4;
				average += rand() % randomSize - randomSize;
				m_heightMap[ConvertTo1D(i + half, j + half)] = average;

			}
		}

		for (int i = 0; i < m_terrainLength - 1; i += half) {
			for (int j = (i + half) % stepSize; j < m_terrainWidth - 1; j += stepSize) {
				float x = CheckHeight(ConvertTo1D((i - half + _size - 1) % (_size - 1), j), totalSize, randomSize);
				float y = CheckHeight(ConvertTo1D((i + half) % (_size - 1), j), totalSize, randomSize);
				float z = CheckHeight(ConvertTo1D(i, (j + half) % (_size - 1)), totalSize, randomSize);
				float w = CheckHeight(ConvertTo1D(i, (j - half + _size - 1) % (_size - 1)), totalSize, randomSize);
				float average = 0;
				average = x + y + z + w;
				average /= 4;
				average += rand() % randomSize - randomSize;
				m_heightMap[ConvertTo1D(i, j)] = average;
				if (i == 0)
					m_heightMap[ConvertTo1D(_size - 1, j)] = average;
				if (j == 0)
					m_heightMap[ConvertTo1D(i, _size - 1)] = average;

			}
		}

		//for (int i = 0; i < m_terrainLength; i += stepSize) {
		//	for (int j = 0; j < m_terrainWidth; j += stepSize) {
		//		DiamondStep(ConvertTo1D(i + half, j), stepSize);
		//		DiamondStep(ConvertTo1D(i, j + half), stepSize);
		//	}
		//}
		stepSize /= 2;
		if (randomSize <= 1)
			continue;
		randomSize /= 2;
	}

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
	int average = 0;
	average += CheckHeight(x, max, _radius);
	average += CheckHeight(y, max, _radius);
	average += CheckHeight(z, max, _radius);
	average += CheckHeight(w, max, _radius);

	average /= 4;

	m_heightMap[_center] = average;

	//DiamondStep(_center + ConvertTo1D(0, -_radius), _radius);
	//DiamondStep(_center + ConvertTo1D(-_radius, 0), _radius);
	//DiamondStep(_center + ConvertTo1D(0, _radius), _radius);
	//DiamondStep(_center + ConvertTo1D(_radius, 0), _radius);

}

void GameObjectTerrain::DiamondStep(int _center, int _radius)
{
	int x = _center + ConvertTo1D(0, -_radius);
	int y = _center + ConvertTo1D(_radius, 0);
	int z = _center + ConvertTo1D(-_radius, 0);
	int w = _center + ConvertTo1D(0, _radius);

	int max = m_terrainLength * m_terrainWidth;
	float average = 0;
	average += CheckHeight(x, max, _radius);
	average += CheckHeight(y, max, _radius);
	average += CheckHeight(z, max, _radius);
	average += CheckHeight(w, max, _radius);

	m_heightMap[_center] = average;
}

bool GameObjectTerrain::IsInBounds(int _1DPos, int _1DMax, int _1DMin)
{
	if(_1DPos < _1DMin || _1DPos > _1DMax )
		return false;
	return true;
}

float GameObjectTerrain::CheckHeight(int _center, int _max, int _random)
{
	if (!IsInBounds(_center, _max)) {
		return (rand() % _random - _random);
	} 
	return m_heightMap[_center];
}

int GameObjectTerrain::ConvertTo1D(int x, int y)
{
	return (y * ( m_terrainWidth) ) + x;
}
