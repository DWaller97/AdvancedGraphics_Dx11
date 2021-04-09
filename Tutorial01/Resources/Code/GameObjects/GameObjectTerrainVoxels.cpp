#include "GameObjectTerrainVoxels.h"

GameObjectTerrainVoxels::GameObjectTerrainVoxels(char* _filePath)
{
	m_position = XMFLOAT3(0, 0, 0);
	NUM_VERTICES = 1;
	NUM_INDICES = 6;
	LoadFromXML(_filePath);
	GenerateFlat3D(m_terrainLength, m_terrainWidth, m_terrainDepth);
}

GameObjectTerrainVoxels::~GameObjectTerrainVoxels()
{
}

HRESULT GameObjectTerrainVoxels::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(BasicVertex) * NUM_VERTICES;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData = {};

	m_mesh.IndexCount = NUM_INDICES;
	m_mesh.VertexCount = NUM_VERTICES;
	m_mesh.IndexBuffer = nullptr;
	m_mesh.VertexBuffer = nullptr;

	InitData.pSysMem = &m_vertices[0];
	InitData.SysMemPitch = m_terrainLength;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_mesh.VertexBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(float) * NUM_INDICES;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &m_indices[0];
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_mesh.IndexBuffer);
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
	return hr;
}

void GameObjectTerrainVoxels::Update(float t)
{
	GameObject::Update(t);
}

void GameObjectTerrainVoxels::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
{
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_world));
	cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(viewMat));
	cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(projMat));
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	pContext->UpdateSubresource(m_constantBuffer, 0, nullptr, &cb1, 0, 0);

	// Set vertex buffer
	UINT stride = sizeof(BasicVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_mesh.VertexBuffer, &stride, &offset);
	// Set index buffer
	pContext->IASetIndexBuffer(m_mesh.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	pContext->IASetInputLayout(m_inputLayout);
	pContext->VSSetShader(m_vertexShader, nullptr, 0);
	pContext->PSSetShader(m_pixelShader, nullptr, 0);

	pContext->DSSetConstantBuffers(0, 1, &m_constantBuffer);

	pContext->DrawIndexed(NUM_INDICES, 0, 0);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void GameObjectTerrainVoxels::LoadFromXML(char* _filePath)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(_filePath);
	if (result.status != pugi::xml_parse_status::status_ok)
	{
		printf("(3D) Terrain file failed to open, error: %s", result.description());
	}
	pugi::xml_node node = doc.first_child();
	pugi::xml_attribute attr = node.first_attribute();
	m_terrainLength = attr.as_uint();
	attr = attr.next_attribute();
	m_terrainWidth = attr.as_uint();
	attr = attr.next_attribute();
	m_terrainDepth = attr.as_uint();
}

void GameObjectTerrainVoxels::GenerateFlat3D(int _sizeX, int _sizeY, int _sizeZ)
{
	m_heightMap.clear();
	m_vertices.clear();
	m_indices.clear();
	//m_terrainLength = _sizeX;
	//m_terrainWidth = _sizeY;
	//m_terrainDepth = _sizeZ;

	m_terrainLength = 8;
	m_terrainWidth = 8;
	m_terrainDepth = 8;
	int totalSize = _sizeX * _sizeY;// *_sizeZ;
	//for (int i = 0; i < totalSize; i++) {
	//	m_heightMap.push_back(0);
	//}
	NUM_VERTICES *= (m_terrainWidth * m_terrainLength);
	NUM_INDICES *= ((m_terrainWidth - 1) * (m_terrainLength - 1));

	for (int i = 0; i < m_terrainWidth; i++) {
		for (int j = 0; j < m_terrainLength; j++) {
			for (int k = 0; k < m_terrainDepth; k++) {
				float u = (float) j / m_terrainLength;
				float v = (float)k / m_terrainWidth;
				BasicVertex b;
				b.pos = XMFLOAT3(j, i, k);
				b.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				b.texCoord = XMFLOAT2(u, v);
				m_vertices.push_back(b);
			}
		}
	}
	for (int i = 0; i < m_terrainWidth; i++) {
		for (int j = 0; j < m_terrainLength; j++) {
			for (int k = 0; k < m_terrainDepth - 1; k++) {
				int pos1D = (m_terrainLength * m_terrainDepth * i) + (m_terrainDepth * j) + k;
				int pos1DNextLine = pos1D + (m_terrainDepth * j);
				m_indices.push_back(pos1D );
				m_indices.push_back(pos1D + 1 );
				m_indices.push_back(pos1DNextLine );

				m_indices.push_back(pos1DNextLine );
				m_indices.push_back(pos1D + 1);
				m_indices.push_back(pos1DNextLine + 1 );
			}
		}
	}
}