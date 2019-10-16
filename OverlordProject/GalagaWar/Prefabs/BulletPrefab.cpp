#include "stdafx.h"
#include "BulletPrefab.h"
#include "EnemyPrefab.h"
#include "GameScene.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "ModelComponent.h"
#include "TransformComponent.h"    
#include "../Managers/StateManager.h"
#include "../../Materials/DiffuseMaterial.h"
#include "../../Materials/UberMaterial.h"

BulletPrefab::BulletPrefab(int materialId)
	: m_MatID(materialId)
	, m_Active(false)
	, m_MaxVelocity(200.0f)
	, m_CurrentVelocity(0)
	, m_AccelerationTime(0.1f)
	, m_Acceleration(m_MaxVelocity/m_AccelerationTime)
	, m_Velocity(DirectX::XMFLOAT3(0,0,0))
	, m_Direction(DirectX::XMFLOAT3(0,0,1))
{
}

void BulletPrefab::Initialize(const GameContext & )
{
	GetScene()->GetPhysxProxy()->EnablePhysxDebugRendering(true);
	auto pPhysx = PhysxManager::GetInstance()->GetPhysics();
	auto pMat = pPhysx->createMaterial(0.f, 0.f, 0.f);

	//RigidBody & Collider
	auto pRigidBody = new RigidBodyComponent();
	AddComponent(pRigidBody);
	std::shared_ptr<physx::PxGeometry> pSphereGeometry(new physx::PxCapsuleGeometry(1.5f, 3.f));//(0.5f, 2.25f));
	auto collider = new ColliderComponent(pSphereGeometry, *pMat, physx::PxTransform(0,0,0, physx::PxQuat(physx::PxPiDivTwo, physx::PxVec3(0,1,0))));
	collider->EnableTrigger(true);
	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group0);
	AddComponent(collider);

	//Model
	auto modelComponent = new ModelComponent(L"./Resources/Meshes/UnitPlane.ovm"); 
	modelComponent->SetMaterial(m_MatID);
	AddComponent(modelComponent);
}

void BulletPrefab::PostInitialize(const GameContext&)
{
	GetTransform()->Scale(1.f, 2.5f, 2.5f);
	GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);

	//Set Collision Callback
	GameObject::PhysicsCallback callback = [&](GameObject* trigger, GameObject* other, GameObject::TriggerAction action)
	{
		this->IsTrigger(trigger, other, action);
	};

	SetOnTriggerCallBack(callback);
}

void BulletPrefab::Update(const GameContext & gameContext)
{
	if (!m_Active || StateManager::GetInstance()->GetState() != State::Playing) return;

	using namespace DirectX;

	if (m_CurrentVelocity < m_MaxVelocity)
		m_CurrentVelocity += m_Acceleration * gameContext.pGameTime->GetElapsed();

	XMVECTOR dirVec = XMLoadFloat3(&m_Direction);
	XMVECTOR velocityVec = dirVec * m_CurrentVelocity * gameContext.pGameTime->GetElapsed();
	XMStoreFloat3(&m_Velocity, velocityVec);

	auto pos = GetTransform()->GetPosition();
	pos.x += m_Velocity.x;
	pos.z += m_Velocity.z;

	GetTransform()->Translate(pos);
}

void BulletPrefab::IsTrigger(GameObject* /*triggerobject*/, GameObject* otherobject, TriggerAction action)
{
	if (action == GameObject::TriggerAction::ENTER) {
		if(otherobject->GetTag() == m_EnemyTag && GetTag() != m_EnemyBulletTag)
		{
			auto enemy = dynamic_cast<EnemyPrefab*>(otherobject);
			if(enemy)
				enemy->SetDead(true);

			SetActive(false);
		}
	}
}

void BulletPrefab::SetActive(bool pause)
{
	m_Active = pause; 
	if(m_Active == false)
		GetTransform()->Translate(0,0,-2000);
}
