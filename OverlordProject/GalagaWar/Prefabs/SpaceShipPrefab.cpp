#include "stdafx.h"
#include "SpaceShipPrefab.h"
#include "GameScene.h"
#include "OverlordGame.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"
#include "TransformComponent.h"
#include "../Managers/StateManager.h"
#include "../../Materials/UberMaterial.h"
#include "../../Materials/DiffuseMaterial.h"
#include "ModelComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "BoxControllerComponent.h"
#include "ParticleEmitterComponent.h"
#include "../Components/SpaceShipComponent.h"
#include "BulletPrefab.h"
#include "CoinPrefab.h"

SpaceShipPrefab::SpaceShipPrefab()
	: m_pController(nullptr)
	, m_PreviousXPos(0)
	, m_RotationVelocity(0)
	, m_MaxVelocity(120.0f)
	, m_CurrentVelocity(0) 
	, m_MoveAccelerationTime(0.3f)
	, m_MoveAcceleration(m_MaxVelocity/m_MoveAccelerationTime)
	, m_Velocity(0,0,0)
{
}

void SpaceShipPrefab::Initialize(const GameContext& gameContext)
{
	GetScene()->GetPhysxProxy()->EnablePhysxDebugRendering(true);
	auto pPhysx = PhysxManager::GetInstance()->GetPhysics();
	auto pMat = pPhysx->createMaterial(0, 0, 1.0f);

	//Material
	auto mat = new DiffuseMaterial();
	mat->SetDiffuseTexture(L"Resources/Textures/SpaceShip/Tex_0017.png");
	gameContext.pMaterialManager->AddMaterial(mat, 99);

	//CharacterController
	m_pController = new BoxControllerComponent(pMat, 10, 5, 10);
	m_pController->SetCollisionGroup(CollisionGroupFlag::Group0);
	AddComponent(m_pController);
	
	//Model
	m_MeshObject = new GameObject();
	auto modelComponent = new ModelComponent(L"./Resources/Meshes/starship.ovm"); 
	modelComponent->SetMaterial(99);
	m_MeshObject->AddComponent(modelComponent);
	AddChild(m_MeshObject);

	//Rigidbody
	auto pRigidBody = new RigidBodyComponent();
	AddComponent(pRigidBody);
	std::shared_ptr<physx::PxGeometry> pSphereGeometry(new physx::PxCapsuleGeometry(5, 5));
	auto collider = new ColliderComponent(pSphereGeometry, *pMat, physx::PxTransform(0,0,0, physx::PxQuat(physx::PxPiDivTwo, physx::PxVec3(0,1,0))));
	collider->EnableTrigger(true);
	AddComponent(collider);

	//Shooting Component
	m_Component = new SpaceShipComponent();
	m_Component->SetActive(false);
	AddComponent(m_Component);

	//JetFlare Particle
	m_JetFlareParticle = GetFlareParticle(DirectX::XMFLOAT3(2, 0.5f, 6.75f), gameContext);
	m_JetFlareParticle2 = GetFlareParticle(DirectX::XMFLOAT3(-2, 0.5f, 6.75f), gameContext);

	RegisterInput(gameContext);
}

void SpaceShipPrefab::PostInitialize(const GameContext& /*gameContext*/)
{
	GameObject::PhysicsCallback callback = [&](GameObject* trigger, GameObject* other, GameObject::TriggerAction action)
	{
		this->IsTrigger(trigger, other, action);
	};

	SetOnTriggerCallBack(callback);
}

void SpaceShipPrefab::Update(const GameContext& gameContext)
{
	MoveWithKeyboardAndController(gameContext);

	//Billboard JetFlare Particle
	if (m_JetFlareParticle) {
		m_JetFlareParticle->GetBillboardRotation().z = gameContext.pCamera->GetTransform()->GetRotation().y;
	}

	if (m_JetFlareParticle2) {
		m_JetFlareParticle2->GetBillboardRotation().z = gameContext.pCamera->GetTransform()->GetRotation().y;
	}
}

void SpaceShipPrefab::MoveController(DirectX::XMFLOAT3& position)
{
	//Clamp Position to Boundary
	if(!m_Dead)
	{
		Clamp<float>(position.x, m_MaxBoundaryWidth, -m_MaxBoundaryWidth);
		Clamp<float>(position.z, m_MaxBoundaryHeight, -m_MaxBoundaryHeight);
	}

	m_pController->Translate(position);
	GetTransform()->Translate(m_pController->GetPosition());
}

void SpaceShipPrefab::MoveWithKeyboardAndController(const GameContext& gameContext)
{
	if(StateManager::GetInstance()->GetState() != State::Playing)
	{
		auto pos = GetTransform()->GetPosition();
		MoveController(pos);
		m_Component->SetActive(false);
		return;
	}

	using namespace DirectX;
	auto movePos = XMFLOAT2(0,0);

	//Handle Thumbstick Input
	auto thumbstickPos = InputManager::GetThumbstickPosition();
	if (thumbstickPos.x != 0 || thumbstickPos.y != 0)
	{
		movePos = thumbstickPos;
		movePos.y = -movePos.y; //Inverse
		m_Component->SetActive(true);		
	}

	//Handle Keyboard Input
	movePos.y = gameContext.pInput->IsActionTriggered(SpaceshipMovement::BACKWARD) ? 1.0f : 0.0f;
	if (movePos.y == 0) movePos.y = -(gameContext.pInput->IsActionTriggered(SpaceshipMovement::FORWARD) ? 1.0f : 0.0f);
	movePos.x = gameContext.pInput->IsActionTriggered(SpaceshipMovement::RIGHT) ? 1.0f : 0.0f;
	if (movePos.x == 0) movePos.x = -(gameContext.pInput->IsActionTriggered(SpaceshipMovement::LEFT) ? 1.0f : 0.0f);

	//If any inputs trigger
	if (movePos.x != 0 || movePos.y != 0)
	{
		if (m_CurrentVelocity < m_MaxVelocity)
			m_CurrentVelocity += m_MoveAcceleration * gameContext.pGameTime->GetElapsed();

		if (movePos.x != 0 && m_RotationVelocity < m_MaxVelocity)
			m_RotationVelocity += m_MoveAcceleration * gameContext.pGameTime->GetElapsed();

		m_Component->SetActive(true); 
	}
	else
	{
		m_RotationVelocity = 0;
		m_CurrentVelocity = 0;
		m_Velocity.x = 0;
		m_Velocity.z = 0;
	}

	const auto forward = XMLoadFloat3(&GetTransform()->GetForward());
	const auto right = XMLoadFloat3(&GetTransform()->GetRight());

	XMVECTOR velocityVec{};
	velocityVec += -right * movePos.x * m_MaxVelocity * gameContext.pGameTime->GetElapsed();
	velocityVec += forward * movePos.y * m_MaxVelocity * gameContext.pGameTime->GetElapsed();
	XMStoreFloat3(&m_Velocity, velocityVec);

	auto pos = GetTransform()->GetPosition();
	pos.x += m_Velocity.x;
	pos.z += m_Velocity.z;

	MoveController(pos);
	m_MeshObject->GetTransform()->Rotate(0, 0, m_RotationVelocity * movePos.x * 0.25f);

	m_PreviousXPos = movePos.x;
}

void SpaceShipPrefab::RegisterInput(const GameContext& gameContext)
{
	const InputAction leftInput = InputAction{SpaceshipMovement::LEFT, InputTriggerState::Down, 'A', -1};
	const InputAction rightInput = InputAction{SpaceshipMovement::RIGHT, InputTriggerState::Down, 'D'};
	const InputAction forwardInput = InputAction{SpaceshipMovement::FORWARD, InputTriggerState::Down, 'W'};
	const InputAction backwardInput = InputAction{SpaceshipMovement::BACKWARD, InputTriggerState::Down, 'S'};
	const InputAction shootInput = InputAction{4, InputTriggerState::Down, 'S', VK_LBUTTON};

	gameContext.pInput->AddInputAction(leftInput);
	gameContext.pInput->AddInputAction(rightInput);
	gameContext.pInput->AddInputAction(forwardInput);
	gameContext.pInput->AddInputAction(backwardInput);
	gameContext.pInput->AddInputAction(shootInput);
}

void SpaceShipPrefab::IsTrigger(GameObject* /*triggerobject*/, GameObject* otherobject, TriggerAction action)
{
	if (action == GameObject::TriggerAction::ENTER) {
		if(otherobject->GetTag() == m_EnemyTag || otherobject->GetTag() == m_EnemyBulletTag)
		{
			m_Dead = true;
			AddExplodeParticle();
			GetTransform()->Translate(0,0,-500);
			otherobject->GetTransform()->Translate(0,0,-1000);
			StateManager::GetInstance()->SetState(State::Stop);
			StateManager::GetInstance()->SetWinState(WinState::Lose);
		}
		else if(otherobject->GetTag() == m_CoinPickupTag)
		{
			auto coin = dynamic_cast<CoinPrefab*>(otherobject);
			if(coin)
				coin->GetCollected();
		}
	}
}

void SpaceShipPrefab::Reset()
{
	m_Dead = false;
	auto deadPos = DirectX::XMFLOAT3(0,0,-100);
	MoveController(deadPos);
	m_Component->ReloadBullets();
}

#pragma region
void SpaceShipPrefab::AddExplodeParticle()
{
	auto rotation = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&rotation, DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(90), 0, 0));
	
	if (!m_ExplosionParticle) {

		m_ExplosionParticle = new GameObject();
		m_ExplodeEmiiter = new ParticleEmitterComponent(L"./Resources/Textures/square.png", ParticleEmitterComponent::Type::Explode, 30);
		m_ExplodeEmiiter->SetVelocity(DirectX::XMFLOAT3(35, 0.f, 35));
		m_ExplodeEmiiter->SetMinSize(0.35f);
		m_ExplodeEmiiter->SetMaxSize(0.35f);
		m_ExplodeEmiiter->SetMinEnergy(1);
		m_ExplodeEmiiter->SetMaxEnergy(1);
		m_ExplodeEmiiter->SetMinSizeGrow(10.f);
		m_ExplodeEmiiter->SetMaxSizeGrow(3.f);
		m_ExplodeEmiiter->SetMinEmitterRange(1.f);
		m_ExplodeEmiiter->SetMaxEmitterRange(6.f);
		m_ExplodeEmiiter->AddColor(DirectX::XMFLOAT4(1, 0, 0, 1));
		m_ExplodeEmiiter->AddColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		m_ExplodeEmiiter->SetLoop(false);
		m_ExplodeEmiiter->SetBillboardRotation(rotation);

		m_ExplodeEmiiter2 = new ParticleEmitterComponent(L"./Resources/Textures/Glare.png", ParticleEmitterComponent::Type::Explode, 1);
		m_ExplodeEmiiter2->SetVelocity(DirectX::XMFLOAT3(0.f, 0.0f, .0f));
		m_ExplodeEmiiter2->SetMinSize(13.5f);
		m_ExplodeEmiiter2->SetMaxSize(13.5f);
		m_ExplodeEmiiter2->SetMinEnergy(1);
		m_ExplodeEmiiter2->SetMaxEnergy(1);
		m_ExplodeEmiiter2->SetMinSizeGrow(9.f);
		m_ExplodeEmiiter2->SetMaxSizeGrow(9.f);
		m_ExplodeEmiiter2->SetMinEmitterRange(0.f);
		m_ExplodeEmiiter2->SetMaxEmitterRange(0.f);
		m_ExplodeEmiiter2->SetLoop(false);
		m_ExplodeEmiiter2->AddColor(DirectX::XMFLOAT4(1, 0, 0, 1));
		m_ExplodeEmiiter2->SetBillboardRotation(rotation);

		m_ExplosionParticle->AddComponent(m_ExplodeEmiiter);
		m_ExplosionParticle->AddComponent(m_ExplodeEmiiter2);
		m_ExplosionParticle->GetTransform()->Translate(GetTransform()->GetPosition());
		GetScene()->AddChild(m_ExplosionParticle);
	}
	else {
		m_ExplosionParticle->GetTransform()->Translate(GetTransform()->GetPosition());
		m_ExplodeEmiiter->Restart();
		m_ExplodeEmiiter2->Restart();
	}
	
}

ParticleEmitterComponent* SpaceShipPrefab::GetFlareParticle(DirectX::XMFLOAT3 position, const GameContext& gameContext)
{
	auto rotation = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&rotation, DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(90), 0, 0));
	auto jetFlareRotation = DirectX::XMFLOAT3(rotation.x, 0, gameContext.pCamera->GetTransform()->GetRotation().y);

	auto jetFlare = new GameObject();
	auto particle = new ParticleEmitterComponent(L"./Resources/Textures/part_jet_core_dff.tif", ParticleEmitterComponent::Type::Explode, 5);
	particle->SetMinSize(4);
	particle->SetMaxSize(5);
	particle->SetMinEnergy(1);
	particle->SetMaxEnergy(1);
	particle->SetMinSizeGrow(1);
	particle->SetMaxSizeGrow(1);
	particle->SetLoop(true);
	particle->AddColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	particle->SetBillboardRotation(jetFlareRotation);
	particle->SetBillboardScale(4, 2, 1);
	jetFlare->AddComponent(particle);
	jetFlare->GetTransform()->Translate(position);
	jetFlare->GetTransform()->Rotate(0, 180, 0);
	
	AddChild(jetFlare);

	return particle;
}

#pragma endregion Particles