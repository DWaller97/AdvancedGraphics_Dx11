#include "GameObjectTerrain.h"

GameObjectTerrain::GameObjectTerrain(char* _fileName)
{
    SetWorldMatrix(new XMFLOAT4X4());
	NUM_VERTICES = 1;
    NUM_INDICES = 6;
	SetPosition(XMFLOAT3(-5, 0, -5));
	LoadFromXML(_fileName);

}

GameObjectTerrain::GameObjectTerrain()
{
	NUM_VERTICES = 1;
	NUM_INDICES = 6;
	SetWorldMatrix(new XMFLOAT4X4());
	SetPosition(XMFLOAT3(-5, 0, -5));
}

GameObjectTerrain::~GameObjectTerrain()
{
    delete m_World;
    m_World = nullptr;
	if (m_heightMap)
		delete[] m_heightMap;
	if (m_vertices)
		delete[] m_vertices;
	if (m_indices)
		delete[] m_indices;
	m_heightMap = nullptr;
	m_vertices = nullptr;
	m_indices = nullptr;
    Release();
}

HRESULT GameObjectTerrain::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
    HRESULT hr;
	NUM_VERTICES *= (m_terrainWidth * m_terrainLength);
	NUM_INDICES *= (m_terrainWidth * m_terrainLength);
	//int* test = new int[NUM_VERTICES];
	m_vertices = new BasicVertex[NUM_VERTICES];
	m_indices = new UINT[NUM_INDICES];

	for (int i = 0; i < m_terrainLength; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			float u = (float)i / m_terrainLength;
			float v = (float)j / m_terrainWidth;
			m_vertices[(i * m_terrainLength) + j] = { XMFLOAT3((i), m_heightMap[(i * m_terrainLength) + j] * m_heightScale, (j)), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(u, v) };
		}
	}

	for (int i = 0, indices = 0; i < m_terrainLength - 1; i++) {
		for (int j = 0; j < m_terrainWidth - 1; j++, indices+=6) {
			m_indices[indices] = (i * m_terrainLength) + j;
			m_indices[indices + 1] = (i * m_terrainLength) + j + 1;
			m_indices[indices + 2] = ((i + 1) * m_terrainLength) + j;

			m_indices[indices + 3] = ((i + 1) * m_terrainLength) + j;
			m_indices[indices + 4] = (i * m_terrainLength) + j + 1;
			m_indices[indices + 5] = ((i + 1) * m_terrainLength) + j + 1;
		}
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = m_terrainWidth;
	desc.Height = m_terrainLength;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(BasicVertex) * NUM_VERTICES;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	ZeroMemory(&InitData, sizeof(InitData));
	
	InitData.pSysMem = m_vertices;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &mesh.VertexBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT) * NUM_INDICES;
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
	hr = pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerHeight);

	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Textures\\darkdirt.dds", nullptr, &m_albedoTexture);
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
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

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
	UINT stride = sizeof(BasicVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &mesh.VertexBuffer, &stride, &offset);
	// Set index buffer
	pContext->IASetIndexBuffer(mesh.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	pContext->IASetInputLayout(m_inputLayout);
	pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pContext->PSSetConstantBuffers(1, 1, &m_pMaterialConstantBuffer);
	pContext->VSSetShader(vertexShader, nullptr, 0);
	pContext->PSSetShader(pixelShader, nullptr, 0);

	pContext->PSSetShaderResources(0, 1, &m_albedoTexture);

	pContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	pContext->VSSetSamplers(1, 1, &m_pSamplerHeight);
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

	m_heightMap = new UINT[m_terrainWidth * m_terrainLength];

	// Copy the array data into a float array and scale it. mHeightmap.resize(heightmapHeight * heightmapWidth, 0);

	for (UINT i = 0; i < m_terrainWidth * m_terrainLength; ++i)
	{
		m_heightMap[i] = (in[i] / 255.0f) * 10;
	}
	

	//printf("Height map: %f\n Size:%f", m_heightMap[m_terrainWidth * m_terrainLength - 1], m_terrainWidth * m_terrainLength);
}

void GameObjectTerrain::DiamondSquare(UINT _size, int _c1, int _c2, int _c3, int _c4)
{
	//2^n + 1
	_size += 1;
	if (m_heightMap != nullptr)
		m_heightMap = nullptr;
	m_heightMap = new UINT[_size * _size];
	for (int i = 0; i < (_size) * (_size ); i++)
		m_heightMap[i] = 0;
	m_terrainLength = _size;
	m_terrainWidth = _size;
	m_heightMap[0] = _c1;
	int t = ConvertTo1D(_size/ 2, _size / 2);
	m_heightMap[ConvertTo1D(_size - 1, 0)] = _c2;
	m_heightMap[ConvertTo1D(0, _size - 1)] = _c3;
	m_heightMap[ConvertTo1D(_size - 1, _size - 1)] = _c4;
	//m_heightMap[ConvertTo1D(_size / 2, _size / 2)] = 1;
	return;

	int stepSize = _size - 1;
	while (stepSize > 1) {
		for (int i = 0; i < m_terrainLength; i += stepSize) {
			for (int j = 0; j < m_terrainWidth; j += stepSize) {
				SquareStep(ConvertTo1D(i, j), stepSize);
			}
		}

		for (int i = 0; i < m_terrainLength; i+= stepSize) {
			for (int j = 0; j < m_terrainWidth; j+= stepSize) {
				DiamondStep(ConvertTo1D(i + stepSize, j), stepSize);
				DiamondStep(ConvertTo1D(i, j + stepSize), stepSize);
			}
		}
		stepSize /= 2;

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
	int maxElements = 4;
	int* elements = new int[4];
	int tl = _center + ConvertTo1D(-_radius, -_radius);

	int tr = _center + ConvertTo1D(_radius, -_radius);

	int bl = _center + ConvertTo1D(-_radius, _radius);
	int br = _center + ConvertTo1D(_radius, _radius);

	if (tl < 0 || tl >(m_terrainLength) * (m_terrainWidth))
	{
		maxElements--;
	}
	else {
		elements[maxElements - 4] = m_heightMap[tl];
	}


	if (tr < 0 || tr >(m_terrainLength) * (m_terrainWidth))
	{
		maxElements--;
	}
	else {
		elements[maxElements - 3] = m_heightMap[tr];
	}

	if (bl < 0 || bl >(m_terrainLength) * (m_terrainWidth))
	{
		maxElements--;
	}
	else {
		elements[maxElements - 2] = m_heightMap[bl];
	}
	if (br < 0 || br >(m_terrainLength) * (m_terrainWidth))
	{
		maxElements--;
	}
	else {
		elements[maxElements - 1] = m_heightMap[br];
	}
	float average = 0;

	for (int i = 0; i < maxElements; i++) {
		average += elements[i];
	}
	delete[] elements;
	elements = nullptr;
	average /= maxElements;

	m_heightMap[_center] = average;
}

void GameObjectTerrain::DiamondStep(int _center, int _radius)
{
	int maxElements = 4;
	int* elements = new int[4];
	int t = _center + ConvertTo1D(0, -_radius);

	int r = _center + ConvertTo1D(_radius, 0);

	int l = _center + ConvertTo1D(-_radius, 0);
	int b = _center + ConvertTo1D(0, _radius);

	if (t < 0 || t >(m_terrainLength) * (m_terrainWidth))
	{
		maxElements--;
	}
	else {
		elements[maxElements - 4] = m_heightMap[t];
	}


	if (r < 0 || r >(m_terrainLength) * (m_terrainWidth))
	{
		maxElements--;
	}
	else {
		elements[maxElements - 3] = m_heightMap[r];
	}

	if (l < 0 || l >(m_terrainLength) * (m_terrainWidth))
	{
		maxElements--;
	}
	else {
		elements[maxElements - 2] = m_heightMap[l];
	}
	if (b < 0 || b >(m_terrainLength) * (m_terrainWidth))
	{
		maxElements--;
	}
	else {
		elements[maxElements - 1] = m_heightMap[b];
	}
	float average = 0;

	for (int i = 0; i < maxElements; i++) {
		average += elements[i];
	}
	delete[] elements;
	elements = nullptr;
	average /= maxElements;

	m_heightMap[_center] = average;
}

int GameObjectTerrain::ConvertTo1D(int x, int y)
{
	return (y * m_terrainWidth) + x;
}
