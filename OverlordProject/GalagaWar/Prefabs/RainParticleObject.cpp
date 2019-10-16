#include "stdafx.h"
#include "RainParticleObject.h"
#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"

void RainParticleObject::Initialize(const GameContext &)
{
	auto rotation = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&rotation, DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(90), 0, 0));
	auto pEmitter = new ParticleEmitterComponent(L"./Resources/Textures/Rain.png", ParticleEmitterComponent::Type::Rain, 70);
	pEmitter->SetVelocity(DirectX::XMFLOAT3(0, 0.0f, -70.f));
	pEmitter->SetMinSize(5);
	pEmitter->SetMaxSize(15);
	pEmitter->SetMinEnergy(6);
	pEmitter->SetMaxEnergy(6);
	pEmitter->SetMinSizeGrow(1.f);
	pEmitter->SetMaxSizeGrow(1.f);
	pEmitter->SetMinEmitterRange(-15);
	pEmitter->SetMaxEmitterRange(15);
	pEmitter->AddColor(DirectX::XMFLOAT4(1, 1, 1, 0.3f));
	pEmitter->SetBillboardRotation(rotation);
	pEmitter->SetBillboardScale(0.65f, 1.8f, 1);
	pEmitter->EnableImediateParticle(true);
	AddComponent(pEmitter);
	GetTransform()->Translate(-60.f, 0, 70);
}