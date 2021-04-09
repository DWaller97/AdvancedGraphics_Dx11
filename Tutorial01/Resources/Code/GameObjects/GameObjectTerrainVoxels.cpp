#include "GameObjectTerrainVoxels.h"

GameObjectTerrainVoxels::GameObjectTerrainVoxels(char* _filePath)
{
	m_position = XMFLOAT3(0, 0, 0);
	LoadFromXML(_filePath);
}

GameObjectTerrainVoxels::~GameObjectTerrainVoxels()
{
}

HRESULT GameObjectTerrainVoxels::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	return E_NOTIMPL;
}

void GameObjectTerrainVoxels::Update(float t)
{
}

void GameObjectTerrainVoxels::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
{
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

void GameObjectTerrainVoxels::InitialiseRandomTerrain3D(int _sizeX, int _sizeY, int _sizeZ)
{
		m_heightMap.clear();
		m_vertices.clear();
		m_indices.clear();
		m_terrainLength = _sizeX;
		m_terrainWidth = _sizeY;
		m_terrainDepth = _sizeZ;
		int totalSize = _sizeX * _sizeY * _sizeZ;
		for (int i = 0; i < totalSize; i++) {
			m_heightMap.push_back(0);
		}
		NUM_VERTICES *= totalSize;
		NUM_INDICES *= ((m_terrainWidth - 1) * (m_terrainLength - 1) * (m_terrainDepth - 1));

}
