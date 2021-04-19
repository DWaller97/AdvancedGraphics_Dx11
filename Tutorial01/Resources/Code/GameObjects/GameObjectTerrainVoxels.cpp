#include "GameObjectTerrainVoxels.h"

GameObjectTerrainVoxels::GameObjectTerrainVoxels(char* _filePath)
{
	m_position = XMFLOAT3(0, 0, 0);
	NUM_VERTICES = 36;
	NUM_INDICES = 36;
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
	//InitData.SysMemPitch = m_terrainLength;
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

	pContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);

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

void GameObjectTerrainVoxels::GeneratePerlinNoise(int _sizeX, int _sizeY, int _sizeZ)
{
	vector<float> noise;
	for (int i = 0; i < _sizeX; i++) {
		for (int j = 0; j < _sizeZ; j++) {
			noise.push_back((float)rand() / RAND_MAX);
		}
	}

	int octaves = 8;
	for (int i = 0; i < _sizeX; i++) {
		for (int j = 0; j < _sizeZ; j++) {
			float result = 0.0f;

			for (int k = 0; k < octaves; k++) {
				int i1, i2, j1, j2;

				int pitch = _sizeX >> k;

				i1 = (i / pitch) * pitch;
				i2 = (i1 + pitch) % _sizeX;

				j1 = (j / pitch) * pitch;
				j2 = (j1 + pitch) % _sizeZ;

				float blendX = (float)(i - i1) / (float)pitch;
				float blendY = (float)(j - j1) / (float)pitch;

				float lerpX = (1.0f - blendX) * noise.at(j1 * _sizeX + i1) + blendX * noise.at(j1 * _sizeX + i2);
				float lerpY = (1.0f - blendX) * noise.at(j2 * _sizeX + i1) + blendX * noise.at(j2 * _sizeX + i2);

				result += (blendY * (lerpY - lerpX) + lerpX);
			}

			m_heightMap.push_back(result);
		}
	}

}

void GameObjectTerrainVoxels::GenerateFlat3D(int _sizeX, int _sizeY, int _sizeZ)
{
	m_vertices.clear();
	m_indices.clear();
	m_voxels.clear();
	//m_terrainLength = _sizeX;
	//m_terrainWidth = _sizeY;
	//m_terrainDepth = _sizeZ;


	//m_terrainLength = 256;
	//m_terrainDepth = 32;
	//m_terrainWidth = 256;
	//GeneratePerlinNoise(m_terrainLength, m_terrainDepth, m_terrainWidth);
	int totalSize = _sizeX * _sizeY *_sizeZ;
	NUM_VERTICES = 36 * totalSize;
	NUM_INDICES = 36 * totalSize;

	for (int i = 0; i < m_terrainLength; i++) {
		for (int j = 0; j < m_terrainDepth; j++) {
			for (int k = 0; k < m_terrainWidth; k++) {
				VoxelType v;
				UINT n = (rand() % 100 + 1);
				if (n <= 90)
					v = VoxelType::AIR;
				if (n > 90)
					v = VoxelType::SOLID;
				m_voxels.push_back(v);
			}
		}
	}


	UINT indices = 0;
	for (int i = 0; i < m_terrainLength; i++) {
		for (int j = 0; j < m_terrainDepth; j++) {
			for (int k = 0; k < m_terrainWidth; k++) {
				if (m_voxels[(m_terrainDepth * m_terrainWidth * i) + (m_terrainWidth * j) + k] != VoxelType::SOLID) {
					NUM_INDICES -= 36;
					NUM_VERTICES -= 36;
					continue;

				}
				m_vertices.insert(m_vertices.end(),
					{
						// front
						{ XMFLOAT3(-1.0f + (i * 2), 1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) }, // 19 // 24
						{ XMFLOAT3(1.0f + (i * 2), -1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, -1.0f) , XMFLOAT2(0.0f, 0.0f) }, // 17 // 25
						{ XMFLOAT3(-1.0f + (i * 2), -1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }, // 16 // 26 

						{ XMFLOAT3(1.0f + (i * 2), 1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) }, // 18 // 27
						{ XMFLOAT3(1.0f + (i * 2), -1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, -1.0f) , XMFLOAT2(0.0f, 0.0f) }, // 17 // 28
						{ XMFLOAT3(-1.0f + (i * 2), 1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) }, // 19 // 29

						// top
						{ XMFLOAT3(-1.0f + (i * 2), 1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 3 // 0
						{ XMFLOAT3(1.0f + (i * 2), 1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 1 // 1
						{ XMFLOAT3(-1.0f + (i * 2), 1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }, // 0 // 2

						{ XMFLOAT3(1.0f + (i * 2), 1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }, // 2 // 3
						{ XMFLOAT3(1.0f + (i * 2), 1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 1 // 4
						{ XMFLOAT3(-1.0f + (i * 2), 1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 3 // 5

						// bottom
						{ XMFLOAT3(1.0f + (i * 2), -1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 6 // 6
						{ XMFLOAT3(-1.0f + (i * 2), -1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 4 // 7
						{ XMFLOAT3(1.0f + (i * 2), -1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }, // 5 // 8

						{ XMFLOAT3(-1.0f + (i * 2), -1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }, // 7 // 9
						{ XMFLOAT3(-1.0f + (i * 2), -1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 4 // 10 
						{ XMFLOAT3(1.0f + (i * 2), -1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 6 // 11

						// left
						{ XMFLOAT3(-1.0f + (i * 2), 1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 11 // 12
						{ XMFLOAT3(-1.0f + (i * 2), -1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 9 // 13
						{ XMFLOAT3(-1.0f + (i * 2), -1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }, // 8 // 14

						{ XMFLOAT3(-1.0f + (i * 2), 1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }, // 10 // 15
						{ XMFLOAT3(-1.0f + (i * 2), -1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 9 // 16
						{ XMFLOAT3(-1.0f + (i * 2), 1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 11 // 17

						// rjght
						{ XMFLOAT3(1.0f + (i * 2), 1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 14 // 18
						{ XMFLOAT3(1.0f + (i * 2), -1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 12 // 19
						{ XMFLOAT3(1.0f + (i * 2), -1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }, // 13 // 20

						{ XMFLOAT3(1.0f + (i * 2), 1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }, // 15 // 21
						{ XMFLOAT3(1.0f + (i * 2), -1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 12 // 22
						{ XMFLOAT3(1.0f + (i * 2), 1.0f + (j * 2)  , -1.0f + (k * 2)), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 14 // 23

						// back
						{ XMFLOAT3(1.0f + (i * 2), 1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }, // 22
						{ XMFLOAT3(-1.0f + (i * 2), -1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }, // 20s
						{ XMFLOAT3(1.0f + (i * 2), -1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) }, // 21

						{ XMFLOAT3(-1.0f + (i * 2), 1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }, // 23
						{ XMFLOAT3(-1.0f + (i * 2), -1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }, // 20
						{ XMFLOAT3(1.0f + (i * 2), 1.0f + (j * 2)  , 1.0f + (k * 2)), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }, // 22
					}
				);
				m_indices.insert(m_indices.end(), {
					0+ indices,1+ indices,2+ indices,
					3+ indices,4+ indices,5+ indices,

					6+ indices,7+ indices,8+ indices,
					9+ indices,10+ indices,11+ indices,

					12+ indices,13+ indices,14+ indices,
					15+ indices,16+ indices,17+ indices,

					18+ indices,19+ indices,20+ indices,
					21+ indices,22+ indices,23+ indices,

					24+ indices,25+ indices,26+ indices,
					27+ indices,28+ indices,29+ indices,

					30+ indices,31+ indices,32+ indices,
					33+ indices,34+ indices,35 + indices
				});
				indices += 36;
			}
		}
	}


	//CalculateModelVectors(m_indices[0], NUM_VERTICES);

	//for (int i = 0; i < m_terrainWidth; i++) {
	//	for (int j = 0; j < m_terrainLength; j++) {
	//		for (int k = 0; k < m_terrainDepth; k++) {
	//			float u = (float) j / m_terrainLength;
	//			float v = (float)k / m_terrainWidth;
	//			BasicVertex b;
	//			b.pos = XMFLOAT3(j, i, k);
	//			b.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//			b.texCoord = XMFLOAT2(u, v);
	//			m_vertices.push_back(b);
	//		}
	//	}
	//}
	//for (int i = 0; i < m_terrainWidth; i++) {
	//	for (int j = 0; j < m_terrainLength; j++) {
	//		for (int k = 0; k < m_terrainDepth - 1; k++) {
	//			int pos1D = (m_terrainLength * m_terrainDepth * i) + (m_terrainDepth * j) + k;
	//			int pos1DNextLine = pos1D + (m_terrainDepth * j);
	//			m_indices.push_back(pos1D );
	//			m_indices.push_back(pos1D + 1 );
	//			m_indices.push_back(pos1DNextLine );

	//			m_indices.push_back(pos1DNextLine );
	//			m_indices.push_back(pos1D + 1);
	//			m_indices.push_back(pos1DNextLine + 1 );
	//		}
	//	}
	//}
}