#include "stdafx.h"
#include "TorusPrefab.h"
#include "Components.h"

TorusPrefab::TorusPrefab(float majorRadius, unsigned int majorRadiusSteps, float minorRadius, unsigned int minorRadiusSteps,
                         DirectX::XMFLOAT4 color):
	m_MajorRadius(majorRadius),
	m_MinorRadius(minorRadius),
	m_MajorRadiusSteps(majorRadiusSteps),
	m_MinorRadiusSteps(minorRadiusSteps),
	m_Color(color)
{
}

void TorusPrefab::Initialize(const GameContext&)
{
	const auto vertCount = m_MajorRadiusSteps * m_MinorRadiusSteps;
	auto mesh = new MeshIndexedDrawComponent(vertCount, vertCount * 6);

	//Vertices
	const float majorInterval = DirectX::XM_2PI / m_MajorRadiusSteps;
	const float minorInterval = DirectX::XM_2PI / m_MinorRadiusSteps;
	for (unsigned int majorStep = 0; majorStep < m_MajorRadiusSteps; ++majorStep)
	{
		const float torusRadius = majorStep * majorInterval;

		for (unsigned int minorStep = 0; minorStep < m_MinorRadiusSteps; ++minorStep)
		{
			const float circleRadius = minorStep * minorInterval;
			DirectX::XMFLOAT3 pos;
			pos.x = (m_MajorRadius + (m_MinorRadius * cos(circleRadius))) * cos(torusRadius);
			pos.y = (m_MajorRadius + (m_MinorRadius * cos(circleRadius))) * sin(torusRadius);
			pos.z = m_MinorRadius * sin(circleRadius);

			//Normal
			DirectX::XMFLOAT3 middle;
			middle.x = m_MajorRadius * cos(torusRadius);
			middle.y = m_MajorRadius * sin(torusRadius);
			middle.z = 0;

			const DirectX::XMVECTOR vecPos = XMLoadFloat3(&pos);
			const DirectX::XMVECTOR vecMiddle = XMLoadFloat3(&middle);
			const DirectX::XMVECTOR vecNormal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(vecPos, vecMiddle));
			DirectX::XMFLOAT3 normal;
			XMStoreFloat3(&normal, vecNormal);

			mesh->AddVertex(VertexPosNormCol(pos, normal, m_Color));
		}
	}

	//Indices
	for (unsigned int i = 0; i < vertCount; ++i)
	{
		const auto v0 = i;
		const auto v1 = (v0 + m_MinorRadiusSteps) % vertCount;
		auto v2 = v1 + 1;
		auto v3 = i + 1;

		if ((i + 1) % m_MinorRadiusSteps == 0)
		{
			v2 -= m_MinorRadiusSteps;
			v3 -= m_MinorRadiusSteps;
		}

		mesh->AddIndex(v0);
		mesh->AddIndex(v1);
		mesh->AddIndex(v2);
		mesh->AddIndex(v2);
		mesh->AddIndex(v3);
		mesh->AddIndex(v0);
	}

	AddComponent(mesh);
}
