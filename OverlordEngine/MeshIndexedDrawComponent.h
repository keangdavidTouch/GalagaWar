#pragma once
#include <vector>
#include "BaseComponent.h"

class MeshIndexedDrawComponent : public BaseComponent
{
	static ID3DX11EffectMatrixVariable *m_pWorldVar, *m_pWvpVar;

	std::vector<VertexPosNormCol> m_vecVertices;
	std::vector<unsigned int> m_vecIndices;

	ID3D11Buffer *m_pVertexBuffer, *m_pIndexBuffer;
	unsigned int m_VertexCapacity, m_IndexCapacity;

	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3D11InputLayout* m_pInputLayout;

	void LoadEffect(const GameContext& gameContext);
	void InitializeVertexBuffer(const GameContext& gameContext);
	void InitializeIndexBuffer(const GameContext& gameContext);

public:
	MeshIndexedDrawComponent(const MeshIndexedDrawComponent& other) = delete;
	MeshIndexedDrawComponent(MeshIndexedDrawComponent&& other) noexcept = delete;
	MeshIndexedDrawComponent& operator=(const MeshIndexedDrawComponent& other) = delete;
	MeshIndexedDrawComponent& operator=(MeshIndexedDrawComponent&& other) noexcept = delete;
	MeshIndexedDrawComponent(unsigned int vertexCapacity = 50, unsigned int indexCapacity = 50);
	virtual ~MeshIndexedDrawComponent();

	void AddVertex(VertexPosNormCol vertex, bool updateBuffer = false);
	void AddIndex(unsigned int index, bool updateBuffer = false);

	void ClearVertexList();
	void ClearIndexList();

	void UpdateVertexBuffer();
	void UpdateIndexBuffer();
	void GenerateNormals();

protected:

	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void Initialize(const GameContext& gameContext) override;
};
