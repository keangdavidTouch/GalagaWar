#include "stdafx.h"
#include "CubePrefab.h"
#include "MeshDrawComponent.h"

CubePrefab::CubePrefab(float width, float height, float depth, DirectX::XMFLOAT4 color):
	m_Width(width),
	m_Height(height),
	m_Depth(depth),
	m_Color(color)
{
}

void CubePrefab::Initialize(const GameContext&)
{
	//Create Cube
	auto mesh = new MeshDrawComponent(12);

	const float halfWidth = m_Width / 2.f;
	const float halfHeight = m_Height / 2.f;
	const float halfDepth = m_Depth / 2.f;

	//FRONT
	mesh->AddQuad(
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, halfHeight, -halfDepth), DirectX::XMFLOAT3(0, 0, -1), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, halfHeight, -halfDepth), DirectX::XMFLOAT3(0, 0, -1), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, -halfHeight, -halfDepth), DirectX::XMFLOAT3(0, 0, -1), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, -halfHeight, -halfDepth), DirectX::XMFLOAT3(0, 0, -1), m_Color)
	);

	//BACK
	mesh->AddQuad(
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, halfHeight, halfDepth), DirectX::XMFLOAT3(0, 0, 1), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, halfHeight, halfDepth), DirectX::XMFLOAT3(0, 0, 1), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, -halfHeight, halfDepth), DirectX::XMFLOAT3(0, 0, 1), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, -halfHeight, halfDepth), DirectX::XMFLOAT3(0, 0, 1), m_Color)
	);

	//LEFT
	mesh->AddQuad(
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, halfHeight, halfDepth), DirectX::XMFLOAT3(-1, 0, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, halfHeight, -halfDepth), DirectX::XMFLOAT3(-1, 0, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, -halfHeight, -halfDepth), DirectX::XMFLOAT3(-1, 0, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, -halfHeight, halfDepth), DirectX::XMFLOAT3(-1, 0, 0), m_Color)
	);

	//RIGHT
	mesh->AddQuad(
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, halfHeight, -halfDepth), DirectX::XMFLOAT3(1, 0, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, halfHeight, halfDepth), DirectX::XMFLOAT3(1, 0, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, -halfHeight, halfDepth), DirectX::XMFLOAT3(1, 0, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, -halfHeight, -halfDepth), DirectX::XMFLOAT3(1, 0, 0), m_Color)
	);

	//TOP
	mesh->AddQuad(
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, halfHeight, halfDepth), DirectX::XMFLOAT3(0, 1, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, halfHeight, halfDepth), DirectX::XMFLOAT3(0, 1, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, halfHeight, -halfDepth), DirectX::XMFLOAT3(0, 1, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, halfHeight, -halfDepth), DirectX::XMFLOAT3(0, 1, 0), m_Color)
	);

	//BOTTOM
	mesh->AddQuad(
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, -halfHeight, -halfDepth), DirectX::XMFLOAT3(0, -1, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, -halfHeight, -halfDepth), DirectX::XMFLOAT3(0, -1, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(halfWidth, -halfHeight, halfDepth), DirectX::XMFLOAT3(0, -1, 0), m_Color),
		VertexPosNormCol(DirectX::XMFLOAT3(-halfWidth, -halfHeight, halfDepth), DirectX::XMFLOAT3(0, -1, 0), m_Color)
	);

	AddComponent(mesh);
}
