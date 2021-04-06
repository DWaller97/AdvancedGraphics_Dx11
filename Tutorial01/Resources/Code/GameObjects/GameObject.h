#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include <iostream>

#include "../Utilities/DDSTextureLoader.h"
#include "../Utilities/resource.h"
#include "../Utilities/structures.h"
#include "../Utilities/Time.h"
#include "../Utilities/OBJLoader.h"

#include "../Managers/CameraManager.h"
#include "../Managers/ShaderManager.h"



using namespace DirectX;



class GameObject
{
public:

	GameObject();
	virtual ~GameObject();

	XMFLOAT3							m_position = XMFLOAT3(0, 0, 0);

	MeshData m_mesh;


	static void					CalculateTangentBinormal2(SimpleVertex v0, SimpleVertex v1, SimpleVertex v2, XMFLOAT3& normal, XMFLOAT3& tangent, XMFLOAT3& binormal);
	static void					CalculateModelVectors(SimpleVertex* _vertices, int _vertexCount);

	HRESULT						virtual InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	void								virtual Update(float t);
	void								virtual Draw(ID3D11DeviceContext* pContext, ID3D11Buffer* lightConstantBuffer, XMFLOAT4X4* projMat, XMFLOAT4X4* viewMat);

	void								SetPosition(XMFLOAT3 _position);
	void								SetShaders(ShaderData _shaderData);
	void								SetMesh(char* filename, ID3D11Device* _pd3dDevice, bool invertTexCoords);
	void								SetAlbedoTexture(ID3D11ShaderResourceView* _resourceView);
	void								SetAlbedoTexture(const wchar_t* _filePath, ID3D11Device* _device);
	void								SetNormalTexture(ID3D11ShaderResourceView* _resourceView);
	void								SetNormalTexture(const wchar_t* _filePath, ID3D11Device* _device);
	void								SetOcclusionTexture(ID3D11ShaderResourceView* _resourceView);
	void								SetOcclusionTexture(const wchar_t* _filePath, ID3D11Device* _device);

	XMFLOAT3 GetPosition() { return m_position; }

	void								virtual Release();
protected:
	void								SetWorldMatrix(XMFLOAT4X4* _world);

	XMFLOAT4X4*	m_world = nullptr;

	ID3D11Buffer* m_constantBuffer = nullptr;
	ID3D11Buffer* m_materialConstantBuffer = nullptr;
	ID3D11Buffer* m_parallaxBuffer = nullptr;
	ID3D11Buffer* m_tesselationBuffer = nullptr;

	ID3D11ShaderResourceView*	 m_textureResourceView = nullptr;
	ID3D11ShaderResourceView*  m_albedoTexture = nullptr;
	ID3D11ShaderResourceView*  m_normalTexture = nullptr;
	ID3D11ShaderResourceView* m_parallaxTexture = nullptr;

	unique_ptr < MaterialPropertiesConstantBuffer	> m_material = nullptr;
	ID3D11SamplerState *	m_samplerLinear = nullptr;
	ID3D11InputLayout*  m_inputLayout = nullptr;

	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader*	m_pixelShader = nullptr;
	ID3D11HullShader* m_hullShader = nullptr;
	ID3D11DomainShader* m_domainShader = nullptr;

	unsigned int	NUM_VERTICES = 0;
	unsigned int	NUM_INDICES = 0;

	float	 m_parallaxBias;
	float m_parallaxScale;
};

