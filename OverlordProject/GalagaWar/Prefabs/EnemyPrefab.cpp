#include "stdafx.h"
#include "EnemyPrefab.h"
#include "GameScene.h"
#include "ModelAnimator.h"
//PhysX
#include "PhysxProxy.h"
#include "PhysxManager.h"
//Managers
#include "ContentManager.h"
#include "../Managers/PickupManager.h"
#include "../Managers/StateManager.h"
//Components
#include "ModelComponent.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"
//Movement
#include "../Paths/BezierPath.h"

EnemyPrefab::EnemyPrefab(bool active)
	: m_Followers()
	, m_BezierPath(new BezierPath())
	, m_PathTransform()
{
	SetDead(!active);
}

EnemyPrefab::~EnemyPrefab()
{
	SafeDelete(m_BezierPath);
}

void EnemyPrefab::SetScale(float scale)
 { 
	m_Scale = scale; 
}

void EnemyPrefab::SetTarget(GameObject * target)
{
	m_Target = target;
	auto random = randF(1, 3);
	m_FireRate = (float)random;
	m_FireElapsedSec = m_FireRate;
}

BezierPath * EnemyPrefab::GetBezier()
{
	return m_BezierPath;
}

void EnemyPrefab::AddFollower(EnemyPrefab * enemy)
{
	m_Followers.push_back(enemy);
	enemy->m_Parent = this;
	enemy->GetBezier()->SetParent(m_BezierPath);
	enemy->GetBezier()->SetOffset(8 * float(m_Followers.size()));
	GetScene()->AddChild(enemy);
}

void EnemyPrefab::SetFollowers(std::vector<EnemyPrefab*> followers)
{
	m_Followers = followers;
}

void EnemyPrefab::SetPath(std::vector<DirectX::XMFLOAT3>& paths)
{
	this->SetDead(false);
	GetBezier()->SetPath(paths);

	if(m_Bullet)
		m_Bullet->SetActive(false);

	for(auto follower : m_Followers)
		follower->SetPath(paths);
}

void EnemyPrefab::Initialize(const GameContext& /*gameContext*/)
{
	GetScene()->GetPhysxProxy()->EnablePhysxDebugRendering(true);
	auto pPhysx = PhysxManager::GetInstance()->GetPhysics();
	auto pMat = pPhysx->createMaterial(0.f, 0.f, 0.f);

	m_Model = new ModelComponent(L"./Resources/Meshes/bug2.ovm");
	m_Model->SetMaterial(0);
	AddComponent(m_Model);
	AddComponent(new RigidBodyComponent());
	std::shared_ptr<physx::PxGeometry> pEnemyGeometry(new physx::PxBoxGeometry(6,2, 2.5));
	AddComponent(new ColliderComponent(pEnemyGeometry, *pMat));
	SetTag(m_Tag);
} 

void EnemyPrefab::PostInitialize(const GameContext&)
{
	auto rigidBody = GetComponent<RigidBodyComponent>();
	rigidBody->GetPxRigidBody()->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	rigidBody->SetKinematic(true);

	m_Model->GetAnimator()->SetAnimation(1);
	m_Model->GetAnimator()->Play();
	m_Model->GetAnimator()->SetAnimationSpeed(1.5f);

	GetTransform()->ScaleUniform(m_Scale);
}
 
void EnemyPrefab::Update(const GameContext& gameContext)
{
	if (StateManager::GetInstance()->GetState() == State::Playing) {
		
		m_Model->GetAnimator()->Play();

		m_BezierPath->Update(gameContext.pGameTime->GetElapsed());

		if(!IsDead())
		{
			this->GetBezier()->GetCurrentPathTransform(m_PathTransform);
			GetTransform()->Translate(m_PathTransform.Position);
			GetTransform()->Rotate(0, m_PathTransform.Angle, 0, false);
			HandleShooting(gameContext);
		}
	}
	else {
		m_Model->GetAnimator()->Pause();
	}
}

void EnemyPrefab::Shoot(GameObject* object)
{
	if(!m_Target || m_FireElapsedSec < m_FireRate) 
		return;

	m_FireElapsedSec = 0;

	using namespace DirectX;
	auto fromPos = GetTransform()->GetPosition();
	auto toPos = object->GetTransform()->GetPosition();
	auto enemyPosVec = DirectX::XMLoadFloat3(&fromPos);
	auto shipPosVec = DirectX::XMLoadFloat3(&toPos);
	auto targetVec = shipPosVec - enemyPosVec;
	targetVec = XMVector3Normalize(targetVec);
	XMFLOAT3 target;
	XMStoreFloat3(&target, targetVec);

	if(!m_Bullet)
	{
		m_Bullet = new BulletPrefab(7);
		m_Bullet->SetTag(L"EnemyBullet");
		m_Bullet->SetMaxVelocity(100);
		GetScene()->AddChild(m_Bullet);
	}

	m_Bullet->SetActive(true);
	m_Bullet->SetDirection(target);
	auto angle = GetTransform()->GetAngleBetweenVector(fromPos, toPos);
	m_Bullet->GetTransform()->Rotate(0, angle, 0, false);
	m_Bullet->GetTransform()->Translate(fromPos.x,fromPos.y -  1.1f,fromPos.z - 12);
}

void EnemyPrefab::SetDead(bool dead)
{
	m_Dead = dead;
	if(m_Dead) {
		if(!m_Parent) 
			SpawnPickupCoin();

		AddExplodeParticle();
	}
	//move to dead position
	GetTransform()->Translate(500, 0,0);
}

bool EnemyPrefab::IsDead() const
{
	return m_Dead;
};

bool EnemyPrefab::IsFinish() 
{
	for(auto follower : m_Followers)
		if(!follower->IsDead())
			if(follower->GetBezier()->IsFinish() == false)
				return false;

	return GetBezier()->IsFinish();
}

bool EnemyPrefab::ShouldRestart() const
{
	for (auto follower : m_Followers) {
		if (!follower->IsDead())
			return false;
	}

	return m_Dead;
}

bool EnemyPrefab::IsInShootingBoundary()
{
	if(m_Bullet)
	{
		auto boundary = 100;
		auto bulletPos = m_Bullet->GetTransform()->GetPosition();
		if(bulletPos.x < -boundary && bulletPos.x > boundary && bulletPos.y < -boundary && bulletPos.y > boundary)
			m_Bullet->SetActive(false);
	}

	auto pos = GetTransform()->GetPosition();
	return (pos.z <= 60 && pos.z > 20 && pos.x > -50 && pos.x < 50) && !GetBezier()->IsFinish();
}

void EnemyPrefab::HandleShooting(const GameContext& gameContext)
{
	if (IsInShootingBoundary())
	{
		Shoot(m_Target);
		m_FireElapsedSec += gameContext.pGameTime->GetElapsed();
	}
	else if (m_FireElapsedSec == 0)
	{
		m_FireElapsedSec = m_FireRate;
	}
}

void EnemyPrefab::SpawnPickupCoin()
{
	auto pos = GetTransform()->GetPosition();
	PickupManager().GetInstance()->RequestCoin(pos);
}

void EnemyPrefab::AddExplodeParticle()
{
	if(m_ExplodeParticle == nullptr) {

		auto rotation = DirectX::XMFLOAT3();
		DirectX::XMStoreFloat3(&rotation, DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(90), 0, 0));

		m_ExplodeParticle = new GameObject();
		m_ParticleEmitter1 = new ParticleEmitterComponent(L"./Resources/Textures/square.png", ParticleEmitterComponent::Type::Explode, 24);
		m_ParticleEmitter1->SetVelocity(DirectX::XMFLOAT3(15.f, 0.f, 15.0f));
		m_ParticleEmitter1->SetMinSize(0.3f);
		m_ParticleEmitter1->SetMaxSize(0.3f);
		m_ParticleEmitter1->SetMinEnergy(1);
		m_ParticleEmitter1->SetMaxEnergy(1);
		m_ParticleEmitter1->SetMinSizeGrow(10.f);
		m_ParticleEmitter1->SetMaxSizeGrow(3.f);
		m_ParticleEmitter1->SetMinEmitterRange(-2.f);
		m_ParticleEmitter1->SetMaxEmitterRange(3.f);
		m_ParticleEmitter1->AddColor(DirectX::XMFLOAT4(0, 1, 1, 1));
		m_ParticleEmitter1->AddColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		m_ParticleEmitter1->SetLoop(false);
		m_ParticleEmitter1->SetBillboardRotation(rotation);

		m_ParticleEmitter2 = new ParticleEmitterComponent(L"./Resources/Textures/Glare.png", ParticleEmitterComponent::Type::Explode, 1);
		m_ParticleEmitter2->SetVelocity(DirectX::XMFLOAT3(0.f, 0.0f, .0f));
		m_ParticleEmitter2->SetMinSize(4.5f);
		m_ParticleEmitter2->SetMaxSize(4.5f);
		m_ParticleEmitter2->SetMinEnergy(1);
		m_ParticleEmitter2->SetMaxEnergy(1);
		m_ParticleEmitter2->SetMinSizeGrow(9.f);
		m_ParticleEmitter2->SetMaxSizeGrow(9.f);
		m_ParticleEmitter2->SetMinEmitterRange(0.f);
		m_ParticleEmitter2->SetMaxEmitterRange(0.f);
		m_ParticleEmitter2->SetLoop(false);
		m_ParticleEmitter2->AddColor(DirectX::XMFLOAT4(0, 1, 1, 1));
		m_ParticleEmitter2->SetBillboardRotation(rotation);

		m_ExplodeParticle->AddComponent(m_ParticleEmitter1);
		m_ExplodeParticle->AddComponent(m_ParticleEmitter2);
		m_ExplodeParticle->GetTransform()->Translate(GetTransform()->GetPosition());
		GetScene()->AddChild(m_ExplodeParticle);
	}
	else {
		m_ExplodeParticle->GetTransform()->Translate(GetTransform()->GetPosition());
		m_ParticleEmitter1->Restart();
		m_ParticleEmitter2->Restart();
	}
}