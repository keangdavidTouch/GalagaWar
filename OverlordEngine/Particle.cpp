#include "stdafx.h"
#include "Particle.h"

// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
using namespace DirectX;

Particle::Particle(const ParticleEmitterSettings& emitterSettings):
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_IsActive(false),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0)
{}

void Particle::Update(const GameContext& gameContext)
{
	if (!m_IsActive)
		return;

	m_CurrentEnergy -= gameContext.pGameTime->GetElapsed();

	if (m_CurrentEnergy < 0) {
		m_IsActive = false;
		m_Finish = true;
		return;
	}

	auto vel = DirectX::XMLoadFloat3(&m_EmitterSettings.Velocity);
	auto dir = DirectX::XMLoadFloat3(&m_Dir);
	auto pos = DirectX::XMLoadFloat3(&m_VertexInfo.Position);
	pos += vel * gameContext.pGameTime->GetElapsed() * dir; //uncomment for explosion particle
	DirectX::XMStoreFloat3(&m_VertexInfo.Position, pos);
	float particleLifePerecent = m_CurrentEnergy / m_TotalEnergy;
	m_VertexInfo.Color.w = particleLifePerecent;
	m_VertexInfo.Size = (m_InitSize * m_SizeGrow) * (1 - particleLifePerecent);
	//if (particleLifePerecent <= 0.2f) {
	//	m_Finish = true; 
	//	//std::cout << "Finish" << std::endl;
	//}
}

void Particle::Init(XMFLOAT3 initPosition, int index)
{
	m_IsActive = true;

	//Energy Initialization
	m_TotalEnergy = m_CurrentEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	
	//Random Direction
	DirectX::XMFLOAT3 randomDir = XMFLOAT3(1, 0, 0);
	DirectX::XMVECTOR randomDirection = DirectX::XMLoadFloat3(&randomDir);
	
	//Random Angle For Explosion Effect
	float angle = 360 / 8;
	float minAngle = (index / 3) * angle;
	float maxAngle = minAngle + angle;
	minAngle = XMConvertToRadians(minAngle);
	maxAngle = XMConvertToRadians(maxAngle);

	XMMATRIX randomRotationMatrix = XMMatrixRotationRollPitchYaw(randF(minAngle, maxAngle), randF(minAngle, maxAngle), 0);
	auto randomNormalizedVector = XMVector3TransformNormal(randomDirection, randomRotationMatrix);
	XMStoreFloat3(&m_Dir, randomNormalizedVector);
	
	//Random Emitting Range
	float particleDistance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);
	//Position
	m_VertexInfo.Position = initPosition;
	auto pos = XMLoadFloat3(&m_VertexInfo.Position);
	pos += randomNormalizedVector * particleDistance;
	XMStoreFloat3(&m_VertexInfo.Position, pos);

	//Random Initial Size
	//m_VertexInfo.Size = m_InitSize = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_InitSize = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_VertexInfo.Size = 0; // Set initial size to 0 since animation is from 0 to 1
	//Random Growing Size Multiplier
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow); 

	//Random Color
	int random = rand() % (m_EmitterSettings.Colors.size()) + 0;
	m_VertexInfo.Color = m_EmitterSettings.Colors[random];
	
	//Random Rotation
	//m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);
}

void Particle::Reset()
{
	m_Finish = false;
}
