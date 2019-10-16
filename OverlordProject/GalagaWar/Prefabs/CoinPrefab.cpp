#include "stdafx.h"
#include "CoinPrefab.h"
#include "ModelComponent.h"
#include"TransformComponent.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"
#include "GameScene.h"
#include "ParticleEmitterComponent.h"
#include "../Managers/StateManager.h"
#include "../Managers/PickupManager.h"
#include "../Sound/SoundObject.h"

CoinPrefab::CoinPrefab() 
	: m_Particle(nullptr)
	, m_MeshGameObject(new GameObject())
{
}

void CoinPrefab::Initialize(const GameContext & )
{
	SetTag(m_Tag);

	m_SoundObject = SoundObject();
	m_SoundObject.AddSound("./Resources/Sounds/pickup_coin_1.wav");
	m_SoundObject.AddSound("./Resources/Sounds/pickup_coin_2.wav");

	//Add ModelComponent
	auto modelComponent = new ModelComponent(L"./Resources/Meshes/Coin.ovm");
	modelComponent->SetMaterial(33);
	m_MeshGameObject->AddComponent(modelComponent);
	m_MeshGameObject->GetTransform()->ScaleUniform(12);
	m_MeshGameObject->GetTransform()->Rotate(90, 0, 0);
	AddChild(m_MeshGameObject);

	//Add Collider & RigidBody
	GetScene()->GetPhysxProxy()->EnablePhysxDebugRendering(true);
	auto pPhysx = PhysxManager::GetInstance()->GetPhysics();
	auto pMat = pPhysx->createMaterial(0.f, 0.f, 0.f);
	AddComponent(new RigidBodyComponent());
	std::shared_ptr<physx::PxGeometry> pEnemyGeometry(new physx::PxSphereGeometry(3));
	AddComponent(new ColliderComponent(pEnemyGeometry, *pMat, physx::PxTransform(0,0,3)));
}

void CoinPrefab::PostInitialize(const GameContext &)
{
	//Disable Gravity
	auto rigidBody = GetComponent<RigidBodyComponent>();
	rigidBody->GetPxRigidBody()->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	rigidBody->SetKinematic(true);
}

void CoinPrefab::Update(const GameContext & gameContext)
{
	if(m_IsDropped)  {

		m_PickUpElaspedSec += gameContext.pGameTime->GetElapsed();

		if(m_PickUpElaspedSec >= m_PickupTime) {
			m_PickUpElaspedSec = 0;
			SetAsDropped(false);
		}

		GetTransform()->Rotate(0, 0, DirectX::XM_PI * gameContext.pGameTime->GetTotal(), false);

		if(StateManager::GetInstance()->GetState() == State::Playing) {
	
			auto pos = GetTransform()->GetPosition();
			GetTransform()->Translate(pos.x, pos.y, pos.z - (gameContext.pGameTime->GetElapsed() * m_MaxDropTime));
		}
	}
}

void CoinPrefab::GetCollected()
{
	AddParticle();

	SetAsDropped(false);
	m_SoundObject.Play(rand() % 1);
	PickupManager::GetInstance()->IncrementCoin();
}

void CoinPrefab::SetAsDropped(bool dropped)
{
	m_IsDropped = dropped;
	if(!m_IsDropped)
		GetTransform()->Translate(1000, 0, 0);
}

void CoinPrefab::AddParticle()
{
	if (m_Particle == nullptr) {

		auto rotation = DirectX::XMFLOAT3();
		DirectX::XMStoreFloat3(&rotation, DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(90), 0, 0));

		m_Particle = new GameObject();
		auto pEmitter = new ParticleEmitterComponent(L"./Resources/Textures/CoinSparkle.png", ParticleEmitterComponent::Type::Explode, 1);
		pEmitter->SetVelocity(DirectX::XMFLOAT3(0.f, 0.0f, .0f));
		pEmitter->SetMinSize(3);
		pEmitter->SetMaxSize(3);
		pEmitter->SetMinEnergy(0.6f);
		pEmitter->SetMaxEnergy(0.6f);
		pEmitter->SetMinSizeGrow(5);
		pEmitter->SetMaxSizeGrow(5);
		pEmitter->SetMinEmitterRange(0.f);
		pEmitter->SetMaxEmitterRange(0.f);
		pEmitter->SetLoop(false);
		pEmitter->AddColor(DirectX::XMFLOAT4(1, 0.85f, 0, 1));
		pEmitter->SetBillboardRotation(rotation);
		m_Particle->AddComponent(pEmitter);
		m_Particle->GetTransform()->Translate(GetTransform()->GetPosition());
		GetScene()->AddChild(m_Particle);
	}
	else {
		m_Particle->GetTransform()->Translate(GetTransform()->GetPosition());
		m_Particle->GetComponent<ParticleEmitterComponent>()->Restart();
	}
}
