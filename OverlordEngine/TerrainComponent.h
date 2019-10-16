#pragma once
#include "BaseComponent.h"

class TextureData;

class TerrainComponent : public BaseComponent
{
public:
	TerrainComponent(std::wstring  heightMapFile, std::wstring  textureFile, unsigned int nrOfRows,
		unsigned int nrOfColumns, float width, float depth, float maxHeight);
	virtual ~TerrainComponent();

	TerrainComponent(const TerrainComponent& other) = delete;
	TerrainComponent(TerrainComponent&& other) noexcept = delete;
	TerrainComponent& operator=(const TerrainComponent& other) = delete;
	TerrainComponent& operator=(TerrainComponent&& other) noexcept = delete;

protected:
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void Initialize(const GameContext& gameContext) override;

private:
	void ReadHeightMap();
	void CalculateVerticesAndIndices();
	void AddTriangle(unsigned a, unsigned b, unsigned c);
	void AddQuad(unsigned a, unsigned b, unsigned c, unsigned d);
	void BuildVertexBuffer(const GameContext& gameContext);
	void BuildIndexBuffer(const GameContext& gameContext);
	void CreatePxHeightField();

	//Resources
	const std::wstring m_HeightMapFile;
	const std::wstring m_TextureFile;

	//Terrain Properties
	unsigned int m_NrOfRows = 0;
	unsigned int m_NrOfColumns = 0;
	unsigned int m_NrOfVertices = 0;
	float m_Width = 0.0f;
	float m_Depth = 0.0f;
	float m_MaxHeight = 0.0f;

	//Buffers
	std::vector<VertexPosNormTex> m_VecVertices;
	std::vector<DWORD> m_VecIndices;
	std::vector<unsigned short> m_VecHeightValues;
	std::vector<DirectX::XMFLOAT3> m_VecFaceNormals;

	//Rendering
	TextureData* m_pTextureData{};
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
	unsigned int m_NumIndices = 0;

	ID3D11InputLayout* m_pInputLayout = nullptr;
	ID3DX11Effect* m_pEffect = nullptr;
	ID3DX11EffectTechnique* m_pTechnique = nullptr;
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pMatWorldVariable = nullptr;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseVariable = nullptr;
};
