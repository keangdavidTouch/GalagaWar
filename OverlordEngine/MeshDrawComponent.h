#pragma once
#include <vector>
#include "BaseComponent.h"

class MeshDrawComponent : public BaseComponent
{
	static ID3DX11EffectMatrixVariable *m_pWorldVar, *m_pWvpVar;

	std::vector<TrianglePosNormCol> m_vecTriangles;
	ID3D11Buffer* m_pVertexBuffer;
	unsigned int m_TriangleCapacity;

	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3D11InputLayout* m_pInputLayout;

	void LoadEffect(const GameContext& gameContext);
	void InitializeBuffer(const GameContext& gameContext);

public:
	MeshDrawComponent(const MeshDrawComponent& other) = delete;
	MeshDrawComponent(MeshDrawComponent&& other) noexcept = delete;
	MeshDrawComponent& operator=(const MeshDrawComponent& other) = delete;
	MeshDrawComponent& operator=(MeshDrawComponent&& other) noexcept = delete;
	MeshDrawComponent(unsigned int triangleCapacity = 50);
	virtual ~MeshDrawComponent();

	void AddTriangle(TrianglePosNormCol triangle, bool updateBuffer = false);
	void AddTriangle(VertexPosNormCol vertex1, VertexPosNormCol vertex2, VertexPosNormCol vertex3,
	                 bool updateBuffer = false);
	void AddQuad(VertexPosNormCol vertex1, VertexPosNormCol vertex2, VertexPosNormCol vertex3, VertexPosNormCol vertex4,
	             bool updateBuffer = false);
	void AddQuad(QuadPosNormCol quad, bool updateBuffer = false);
	void RemoveTriangles();

	void UpdateBuffer();

	unsigned int GetTriangleCapacity() const { return m_TriangleCapacity; }
	void SetTriangleCapacity(unsigned int capacity) { m_TriangleCapacity = capacity; }

protected:

	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void Initialize(const GameContext& gameContext) override;
};
