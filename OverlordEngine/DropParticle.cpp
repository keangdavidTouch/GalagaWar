#include "stdafx.h"
#include "DropParticle.h"

using namespace DirectX;

DropParticle::DropParticle(const ParticleEmitterSettings& emitterSettings)
	: Particle(emitterSettings)
{
	m_Finish = true;
}

void DropParticle::Update(const GameContext& gameContext)
{
	if (!m_IsActive)
		return;

	m_CurrentEnergy -= gameContext.pGameTime->GetElapsed();

	/*if (m_CurrentEnergy < 0) {
		m_IsActive = false;
		return;
	}*/

	auto vel = DirectX::XMLoadFloat3(&m_EmitterSettings.Velocity);
	auto pos = DirectX::XMLoadFloat3(&m_VertexInfo.Position);
	if(m_VertexInfo.Position.z < -150.f)
	{
		m_Finish = true; 
		m_IsActive = false;
		return;
	}
	pos += vel * gameContext.pGameTime->GetElapsed();
	DirectX::XMStoreFloat3(&m_VertexInfo.Position, pos);
	if (!m_EmitterSettings.Colors.empty()) {
		m_VertexInfo.Color = m_EmitterSettings.Colors[0];
	}
}

void DropParticle::Init(DirectX::XMFLOAT3 initPosition, int index)
{
	m_Finish = false;
	m_IsActive = true;

	//Random Energy
	m_TotalEnergy = m_CurrentEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	
	//Random Position in given range[]
	float xDistance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);
	float yDistance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);
	float zDistance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);

	bool isReset = m_VertexInfo.Position.z < -150.f;
	m_VertexInfo.Position = initPosition;

	//m_VertexInfo.Position.x += (15 * (index % 8) + xDistance);
	m_VertexInfo.Position.x += (20 * (index % 7) + xDistance);
	m_VertexInfo.Position.y += yDistance;
	//m_VertexInfo.Position.x += (15 * (index % 8));
	//m_VertexInfo.Position.y = 0;

	if(isReset)
	{
		m_VertexInfo.Position.z = 80 + zDistance;
		//m_VertexInfo.Position.z = 70;
	}
	else
	{
		//m_VertexInfo.Position.z -= (20 * (index / 10) + zDistance);
		m_VertexInfo.Position.z -= (30 * (index / 10) + zDistance);
		//m_VertexInfo.Position.z -= 20 * (index / 10);
	}

	//Random Size
	m_VertexInfo.Size = m_InitSize = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	//Random Growing Size Multiplier
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);

	//Random Rotation
	m_VertexInfo.Rotation = m_EmitterSettings.Rotation;
}
