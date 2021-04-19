#include "GameObjectTerrain.h"

short* GameObjectTerrain::m_tessAmount = nullptr;

float* GameObjectTerrain::m_textureHeight0;
float* GameObjectTerrain::m_textureHeight1;
float* GameObjectTerrain::m_textureHeight2;
float* GameObjectTerrain::m_textureHeight3;

GameObjectTerrain::GameObjectTerrain(char* _filePath)
{
	NUM_VERTICES = 1;
    NUM_INDICES = 6;
	SetPosition(XMFLOAT3(-5, 0, -5));
	Init();
	LoadFromXML(_filePath);
}

GameObjectTerrain::GameObjectTerrain(int _seed)
{
	NUM_VERTICES = 1;
	NUM_INDICES = 6;
	m_seed = _seed;
	srand(m_seed);
	SetPosition(XMFLOAT3(-5, 0, -5));
	Init();

}

GameObjectTerrain::~GameObjectTerrain()
{
	delete m_tessAmount;
	m_tessAmount = nullptr;

	if (m_textureHeight)
		m_textureHeight->Release();
	m_textureHeight = nullptr;

	delete m_textureHeight0;
	delete m_textureHeight1;
	delete m_textureHeight2;
	delete m_textureHeight3;

}

void GameObjectTerrain::Init()
{
	m_tessAmount = new short();
	*m_tessAmount = 1;
	m_textureHeight0 = new float(20);
	m_textureHeight1 = new float(100);
	m_textureHeight2 = new float(250);
	m_textureHeight3 = new float(750);
	m_currCamera = CameraManager::GetCurrCamera();
}

float GameObjectTerrain::Lerp(float _a, float _b, float _t)
{
	return (1 - _t) * _a + _t * _b;
}

HRESULT GameObjectTerrain::InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr;

	SetHeights();

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
	InitData.SysMemPitch = NUM_INDICES / m_terrainWidth;
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
	bd.ByteWidth = sizeof(TesselationBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_tesselationBuffer);
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


	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(PixelTextureHeights);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_textureHeight);
	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Textures\\darkdirt.dds", nullptr, &m_albedo0);
	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Textures\\lightdirt.dds", nullptr, &m_albedo1);
	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Textures\\grass.dds", nullptr, &m_albedo2);
	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Textures\\stone.dds", nullptr, &m_albedo3);
	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Textures\\snow.dds", nullptr, &m_albedo4);
	if (FAILED(hr))
		return hr;

	m_albedoTextures[0] = m_albedo0;
	m_albedoTextures[1] = m_albedo1;
	m_albedoTextures[2] = m_albedo2;
	m_albedoTextures[3] = m_albedo3;
	m_albedoTextures[4] = m_albedo4;


    return hr;

}

void GameObjectTerrain::Update(float t)
{
	GameObject::Update(t);
	XMFLOAT4 camPos = m_currCamera->GetPosition();
	float heightAtPos = CheckHeight(camPos.x / 10, camPos.z / 10) * m_heightScale;
	float heightDiff = camPos.y - heightAtPos; 
	int min = 0;
	int max = 1;
	int maxHeight = m_terrainWidth;
	heightDiff = (heightDiff / (m_heightScale));



	/*********************************************************************************************************
	* MARKING SCHEME 2021
	* CATEGORY: TESSELLATION SHADERS WITH LEVEL OF DETAIL
	* DESCRIPTION: The tessellation values are set here in the update, based on the heigh difference 
	* between the current camera's position and the terrain's height map
	* [Part 1]
	* *******************************************************************************************************/
	//SetTessellationAmount(result);
	if (heightDiff <= -1) {
		SetTessellationAmount(1);
		return;
	}
	if (heightDiff <= 0.2f) {
		SetTessellationAmount(4);
		return;
	}
	if (heightDiff <= 0.5f) {
		SetTessellationAmount(3);
		return;
	}
	if (heightDiff <= 0.75f) {
		SetTessellationAmount(2);
		return;
	}
	SetTessellationAmount(1);
}

void GameObjectTerrain::Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat)
{
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_world));
	cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(viewMat));
	cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(projMat));
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	pContext->UpdateSubresource(m_constantBuffer, 0, nullptr, &cb1, 0, 0);

	/*********************************************************************************************************
	* MARKING SCHEME 2021
	* CATEGORY: TESSELLATION SHADERS WITH LEVEL OF DETAIL
	* DESCRIPTION: Tessellation data is stored in a struct and sent to the hull shader
	* [Part 2]
	* *******************************************************************************************************/

	TesselationBuffer tb;
	tb.tessAmount = *m_tessAmount;
	tb.padding = XMFLOAT3(0, 0, 0);
	pContext->UpdateSubresource(m_tesselationBuffer, 0, nullptr, &tb, 0, 0);
	
	PixelTextureHeights pth;
	pth.albedoZeroToThreeHeight = XMFLOAT4(*m_textureHeight0, *m_textureHeight1, *m_textureHeight2, *m_textureHeight3);
	pContext->UpdateSubresource(m_textureHeight, 0, nullptr, &pth, 0, 0);


	// Set vertex buffer
	UINT stride = sizeof(BasicVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_mesh.VertexBuffer, &stride, &offset);
	// Set index buffer
	pContext->IASetIndexBuffer(m_mesh.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	pContext->IASetInputLayout(m_inputLayout);
	pContext->VSSetShader(m_vertexShader, nullptr, 0);
	pContext->HSSetShader(m_hullShader, nullptr, 0);
	pContext->DSSetShader(m_domainShader, nullptr, 0);
	pContext->PSSetShader(m_pixelShader, nullptr, 0);

	pContext->HSSetConstantBuffers(0, 1, &m_tesselationBuffer);
	pContext->DSSetConstantBuffers(0, 1, &m_constantBuffer);
	pContext->PSSetShaderResources(0, 5, m_albedoTextures);
	pContext->PSSetConstantBuffers(0, 1, &m_textureHeight);
	pContext->PSSetSamplers(0, 1, &m_samplerLinear);

	pContext->DrawIndexed(NUM_INDICES, 0, 0);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->HSSetShader(NULL, nullptr, 0);
	pContext->DSSetShader(NULL, nullptr, 0);

}

void GameObjectTerrain::SetSize(int _width, int _length)
{
	m_terrainWidth = _width;
	m_terrainLength = _length;
}

void GameObjectTerrain::SetHeightmapScale(int _scale)
{
	m_heightScale = _scale;
}

/*********************************************************************************************************
* MARKING SCHEME 2021
* CATEGORY: TERRAIN CONSTRUCTION
* DESCRIPTION: Heightmap is loaded here by looping through the file, grabbing each byte, then 
* converting each byte to a float, then storing this in a vector for me to change the vertex at each
* positions' height
* [Part 3]
* *******************************************************************************************************/
void GameObjectTerrain::LoadHeightMap(char* _filePath)
{
	// A height for each vertex 
	std::vector<unsigned char> in(m_terrainWidth * m_terrainLength);

	// Open the file.
	std::ifstream inFile;
	inFile.open(_filePath, std::ios_base::binary);

	if (inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());
		// Done with file.
		inFile.close();
	}
	//m_heightMap = new long(m_terrainWidth * m_terrainLength);

	// Copy the array data into a float array and scale it. mHeightmap.resize(heightmapHeight * heightmapWidth, 0);
	for (int i = 0; i < m_terrainWidth * m_terrainLength; ++i)
	{
		m_heightMap[i] = ((in[i] / 255.0f));
	}

}

void GameObjectTerrain::GenerateFlat(int _sizeX, int _sizeY)
{
	m_heightMap.clear();
	m_vertices.clear();
	m_indices.clear();
	m_terrainLength = _sizeX;
	m_terrainWidth = _sizeY;
	for (int i = 0; i < (_sizeX * _sizeY); i++) {
		m_heightMap.push_back(0);
	}
	NUM_VERTICES *= (m_terrainWidth * m_terrainLength);
	NUM_INDICES *= ((m_terrainWidth - 1) * (m_terrainLength - 1));

	for (int i = 0; i < m_terrainLength; i++) {
		for (int j = 0; j < m_terrainWidth; j++) {
			float u = (float)i / m_terrainLength;
			float v = (float)j / m_terrainWidth;
			int x = m_position.x + (i * 10);
			int y = m_position.y + (m_heightMap[ConvertTo1D(i, j)] * m_heightScale);
			int z = m_position.z + (j * 10);

			BasicVertex b;
			b.pos = XMFLOAT3(x, y, z);
			b.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			b.texCoord = XMFLOAT2(u, v);
			m_vertices.push_back(b);
		}
	}
	for (int i = 0; i < m_terrainLength; i++) {
		for (int j = 0; j < m_terrainWidth - 1; j++) {

			m_indices.push_back(i * m_terrainLength + j);
			m_indices.push_back(i * m_terrainLength + j + 1);
			m_indices.push_back((i + 1) * m_terrainLength + j);

			m_indices.push_back((i + 1) * m_terrainLength + j);
			m_indices.push_back(i * m_terrainLength + j + 1);
			m_indices.push_back((i + 1) * m_terrainLength + j + 1);
		}
	}

}

/*********************************************************************************************************
* MARKING SCHEME 2021
* CATEGORY: PROCEDURAL TERRAIN GENERATION
* DESCRIPTION: Diamond Square. 4 points are either randomly generated or inputted which are the
* initial points of the terrain's height, the algorithm then averages these 4 points and then stores the
* result, including a random number, in the point in the middle of the terrain. 
* From then, it does the same but in a "diamond" shape, as opposed to the corners. It continues
* iterating through each point switching between diamond and square mode, reducing the random
* range  and the step size until it finishes.
* I then smooth the terrain by iterating through each point and the surrounding 8 points and 
* averaging their heights. It makes the terrain much less "spikey"
* [Part 1]
* *******************************************************************************************************/

void GameObjectTerrain::DiamondSquare(UINT _size, int _randomness, int _c1, int _c2, int _c3, int _c4)
{
	_size += 1;
	GenerateFlat(_size, _size);

	int width = m_terrainWidth - 1;
	int length = m_terrainLength - 1;

	int totalSize = m_terrainWidth * m_terrainLength;
	int size = width * length;

	int randomSize = _randomness;
	if (_randomness == 0)
		randomSize = _size / 2;

	if (_c1 == 0 && _c2 == 0 && _c3 == 0 && _c4 == 0)
	{
		_c1 = rand() % -randomSize + randomSize;
		_c2 = rand() % -randomSize + randomSize;
		_c3 = rand() % -randomSize + randomSize;
		_c4 = rand() % -randomSize + randomSize;
	}
	m_heightMap[0] = _c1;
	m_heightMap[width] = _c2;
	m_heightMap[totalSize - length] = _c3;
	m_heightMap[totalSize - 1] = _c4;
	int stepSize = _size - 1;
	int half = stepSize;

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

		for (int i = 0; i < m_terrainLength; i += half) {
			for (int j = (i + half) % stepSize; j < m_terrainWidth; j += stepSize) {
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
		stepSize /= 2;
		if (randomSize <= 1)
			continue;
		randomSize /= 2;
	}

}

/*********************************************************************************************************
* MARKING SCHEME 2021
* CATEGORY: PROCEDURAL TERRAIN GENERATION
* DESCRIPTION: Fault Line. This algorithm works by creating a "line" in the terrain from one side to
* the other, increasing one side of the line by a displacement value and reducing the other side by
* the same displacement value. It will then run multiple times (_iteration parameter) each time 
* creating a new line. I tried decreasing the displacement over time but I preferred the look of a 
* constant displacement so left it as is. Smoothing is used to make the slopes seem more natural.
* [Part 2]
* *******************************************************************************************************/

void GameObjectTerrain::FaultLine(UINT _size, int _iterations, int _displacement)
{
	GenerateFlat(_size, _size);

	float a = 0;
	float b = 0;
	float c = 0;
	float d = 0;
	float v = 0;
	float distance = 0;
	float displacement = _displacement;
	for (int it = 0; it < _iterations; it++) {
		v = rand() % (_size * _size);
		a = sin(v);
		b = cos(v);
		d = sqrt((_size * _size) + (_size * _size));
		c = ((float)rand() / RAND_MAX) * d - d / 2;
		for (int i = 0; i < _size; i++) {
			for (int j = 0; j < _size; j++) {
				if (a * i + b * j - c > 0) {
					m_heightMap[ConvertTo1D(i, j)] += displacement;
				}
				else {
					m_heightMap[ConvertTo1D(i, j)] -= displacement;
				}
			}
		}

	}
}

/*********************************************************************************************************
* MARKING SCHEME 2021
* CATEGORY: PROCEDURAL TERRAIN GENERATION
* DESCRIPTION: Hill Algorithm. This works by creating a circular shape of points in the terrain based
* on a minimum and maximum radius. It loops through the provided iteration count and generates
* this circle before calculating the height based on the current radius and the distance from the
* point and this radius, which creates a semi-sphere shape. Each iteration, the radius is randomly
* generated based on the parameters. When adding the point to the list of heights, it can be passed
* as just the height, the height squared, or the height cubed. Using squared or cubed values 
* makes the slope size larger and can make near-sheer cliffs and valleys.
* [Part 3]
* *******************************************************************************************************/

void GameObjectTerrain::HillAlgorithm(int _size, int _minRadius, int _maxRadius, int _iterations, UINT _heightDimensions)
{
	GenerateFlat(_size, _size);

	int randomX = 0;
	int randomY = 0;
	int radius = 0;
	float distance = 0;
	float theta = 0;
	for (int it = 0; it < _iterations; it++) {
		randomX = rand() % _size;
		randomY = rand() % _size;
		radius = rand() % _maxRadius + _minRadius;
		distance = rand() % _size / 2 - radius;
		theta = rand() % 2 * PI;
		//randomX = (_size / 2) + (cos(theta) * distance);
		//randomY = (_size / 2) + (sin(theta) * distance);
		for (int i = randomX - radius; i < randomX + radius; i++) {
			for (int j = randomY - radius; j < randomY + radius; j++) {
				float curr = CheckHeight(i, j);
				if (curr == -D3D11_FLOAT32_MAX)
					continue;
				float height = ((radius * radius) - (((i - randomX) * (i - randomX)) + ((j - randomY) * (j - randomY))));
				if (height < 0)
					continue;
				m_heightMap.at(ConvertTo1D(i, j)) += ( _heightDimensions == 1 ? height : _heightDimensions == 2 ? height * height : height * height * height); //Squaring or cubing this creates steeper hills.
			}
		}
	}
	Normalise();
}

void GameObjectTerrain::SetHeights()
{
	for (int i = 0; i < NUM_VERTICES; i++) {
			m_vertices[i].pos.y = m_position.y + (m_heightMap[i] * m_heightScale);
	}
}

void GameObjectTerrain::LoadFromXML(char* _filePath)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(_filePath);
	if (result.status != pugi::xml_parse_status::status_ok)
	{
		printf("Terrain file failed to open, error: %s", result.description());
		return;
	}
	pugi::xml_node node = doc.first_child();
	pugi::xml_attribute attr = node.first_attribute();
	m_position.x = attr.as_int();
	attr = attr.next_attribute();
	m_position.y = attr.as_int();
	attr = attr.next_attribute();
	m_position.z = attr.as_int();
	attr = attr.next_attribute();
	std::string terrainType = attr.as_string();
	attr = attr.next_attribute();
	/*********************************************************************************************************
	* MARKING SCHEME 2021
	* CATEGORY: TERRAIN CONSTRUCTION
	* DESCRIPTION: Using Pugi, I load in details such as terrain width, length, and scale, as well as a file
	* path leading to a .raw file of the terrain heightmap.
	* [Part 2]
	* *******************************************************************************************************/
	if (terrainType == "Load") {
		m_terrainLength = attr.as_uint();
		attr = attr.next_attribute();
		m_terrainWidth = attr.as_uint();
		attr = attr.next_attribute();
		m_heightScale = attr.as_uint();
		attr = attr.next_attribute();
		GenerateFlat(m_terrainWidth, m_terrainLength);
		LoadHeightMap((char*)attr.as_string());
		SetHeights();
	}
	else {
		int seed = attr.as_int();
		if (seed == 0)
			seed = time(NULL);
		srand(seed);
		attr = attr.next_attribute();
		m_terrainLength = attr.as_uint();
		m_terrainWidth = m_terrainLength;
		attr = attr.next_attribute();
		m_heightScale = attr.as_uint();
		attr = attr.next_attribute();
		if (terrainType == "FaultLine") {
			UINT iterations = attr.as_uint();
			attr = attr.next_attribute();
			int displacement = attr.as_int();
			FaultLine(m_terrainLength, iterations, displacement);
			attr = attr.next_attribute();
		}
		else if (terrainType == "Hill") {
			UINT iterations = attr.as_uint();
			attr = attr.next_attribute();
			int minRadius = attr.as_int();
			attr = attr.next_attribute();
			int maxRadius = attr.as_int();
			attr = attr.next_attribute();
			int heightDimensions = attr.as_int();
			if (heightDimensions < 0 || heightDimensions > 2)
				heightDimensions = 0;
			HillAlgorithm(m_terrainLength, minRadius, maxRadius, iterations, heightDimensions);
			attr = attr.next_attribute();
		}
		else if (terrainType == "DiamondSquare") {
			UINT randomness = attr.as_uint();
			attr = attr.next_attribute();
			int c1 = attr.as_int();
			attr = attr.next_attribute();
			int c2 = attr.as_int();
			attr = attr.next_attribute();
			int c3 = attr.as_int();
			attr = attr.next_attribute();
			int c4 = attr.as_int();
			attr = attr.next_attribute();
			DiamondSquare(m_terrainLength, randomness, c1, c2, c3, c4);
			attr = attr.next_attribute();
		}
		bool smooth = attr.as_bool();
		if (!smooth)
			return;
		attr.next_attribute();
		UINT smoothIterations = attr.as_uint();
		SmoothHeights(1, smoothIterations);
	}
}

bool GameObjectTerrain::IsInBounds(int _1DPos, int _1DMax, int _1DMin)
{
	if(_1DPos < _1DMin || _1DPos > _1DMax )
		return false;
	return true;
}

void GameObjectTerrain::SmoothHeights(int _boxSize, int _iterations)
{
	vector<double> heights;

	for (int it = 0; it < _iterations; it++) {
		for (int i = 0; i < m_terrainLength; i++) {
			for (int j = 0; j < m_terrainWidth; j++) {

				for (int k = -_boxSize; k < _boxSize; k++) {
					for (int l = -_boxSize; l < _boxSize; l++) {
						float height = CheckHeight(i + k, j + l);
						if (height == -D3D11_FLOAT32_MAX)
							continue;
						heights.push_back(height);

					}
				}
				float average = 0;
				for (int k = 0; k < heights.size(); k++) {
					average += heights.at(k);
				}
				if (average == 0) {
					m_heightMap[ConvertTo1D(i, j)] = 0;
				}
				else {
					average /= heights.size();
					m_heightMap[ConvertTo1D(i, j)] = average;
				}
				heights.clear();
			}
		}
	}
	Normalise(1);
}

void GameObjectTerrain::SetTessellationAmount(short _amount)
{
	if (_amount > 64 || _amount < 1)
		return;
	*m_tessAmount = _amount;
}

float* GameObjectTerrain::GetTextureHeight(int _index)
{
	if (_index == 0)
		return m_textureHeight0;
	if (_index == 1)
		return m_textureHeight1;
	if (_index == 2)
		return m_textureHeight2;
	if (_index == 3)
		return m_textureHeight3;
	return nullptr;
}

float GameObjectTerrain::CheckHeight(int _center, int _max, int _random)
{
	if (!IsInBounds(_center, _max)) {
		return (rand() % _random -  _random);
	} 
	return m_heightMap[_center];
}

float GameObjectTerrain::CheckHeight(int _x, int _y)
{
	int converted = ConvertTo1D(_x, _y);

	if (!IsInBounds(converted, (m_terrainLength - 1) * (m_terrainWidth - 1)) || _x < 0 || _y < 0) {
		return -D3D11_FLOAT32_MAX;
	}
	return m_heightMap[converted];
}

float GameObjectTerrain::CheckHeight(int _1DPos)
{
	if (!IsInBounds(_1DPos, m_terrainLength * m_terrainWidth)) {
		return -D3D11_FLOAT32_MAX;
	}
	return m_heightMap.at(_1DPos);
}

bool GameObjectTerrain::IsOnSameLine(float _a, float _b, int _width)
{
	float a = static_cast<float>(_width) / _a;
	float b = static_cast<float>(_width) / _b;
	float c = abs(a - b);
	return c < 1.0f;
}

int GameObjectTerrain::ConvertTo1D(int x, int y)
{
	return (y * ( m_terrainWidth) ) + x;
}

void GameObjectTerrain::Normalise(int _scale)
{
	double min = D3D11_FLOAT32_MAX;
	double max = -D3D11_FLOAT32_MAX;
	for (int i = 0; i < m_heightMap.size(); i++) {
		double curr = m_heightMap.at(i);
		if (curr < min) {
			min = curr;
		}
		if (curr > max)
			max = curr;
	}
	
	for (int i = 0; i < m_heightMap.size(); i++) {
		double curr = m_heightMap.at(i);
		m_heightMap.at(i) = (curr - min) / (max - min) * _scale;
	}
}
