#include "stdafx.h"
#include "BoxControllerComponent.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"
#include "ControllerComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "GameScene.h"

BoxControllerComponent::BoxControllerComponent(physx::PxMaterial* material, float width, float height, float forward,
	std::wstring name, physx::PxCapsuleClimbingMode::Enum climbingMode)
	: m_Width(width),
	  m_Height(height),
	  m_Forward(forward),
	  m_Name(std::move(name)),
	  m_Controller(nullptr),
	  m_ClimbingMode(climbingMode),
	  m_pMaterial(material),
	  m_CollisionFlag(physx::PxControllerCollisionFlags()),
	  m_CollisionGroups(physx::PxFilterData(CollisionGroupFlag::Group0, 0, 0, 0))
{
}


void BoxControllerComponent::Initialize(const GameContext&)
{
	if (m_Controller != nullptr)
	{
		Logger::LogError(L"[ControllerComponent] Cannot initialize a controller twice");
		return;
	}

	//TODO: 1. Retrieve the ControllerManager from the PhysX Proxy (PhysxProxy::GetControllerManager();)
	physx::PxControllerManager* manager{  GetGameObject()->GetScene()->GetPhysxProxy()->GetControllerManager() };
	//TODO: 2. Create a PxCapsuleControllerDesc (Struct)
	physx::PxBoxControllerDesc capsuleDesc{};
	capsuleDesc.setToDefault();
	capsuleDesc.halfSideExtent = m_Width;
	capsuleDesc.halfForwardExtent = m_Forward;
	capsuleDesc.halfHeight = m_Height;
	capsuleDesc.upDirection = physx::PxVec3(0,1,0);
	capsuleDesc.contactOffset = 0.1f;
	capsuleDesc.material = m_pMaterial;
	const auto position = GetGameObject()->GetTransform()->GetPosition();
	capsuleDesc.position = physx::PxExtendedVec3(position.x, position.y, position.z - m_Forward/2.f);
	capsuleDesc.userData = this;

	m_Controller = manager->createController(capsuleDesc);

	if (m_Controller == nullptr)
	{
		Logger::LogError(L"[ControllerComponent] Failed to create controller");
		return;
	}

	m_Controller->getActor()->setName(std::string(m_Name.begin(), m_Name.end()).c_str());
	m_Controller->setUserData(this);

	SetCollisionGroup(static_cast<CollisionGroupFlag>(m_CollisionGroups.word0));
	SetCollisionIgnoreGroups(static_cast<CollisionGroupFlag>(m_CollisionGroups.word1));
}

void BoxControllerComponent::Update(const GameContext&)
{
}

void BoxControllerComponent::Draw(const GameContext&)
{
}

void BoxControllerComponent::Translate(const DirectX::XMFLOAT3& position) const
{
	Translate(position.x, position.y, position.z);
}

void BoxControllerComponent::Translate(const float x, const float y, const float z) const
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot Translate. No controller present");
	else
		m_Controller->setPosition(physx::PxExtendedVec3(x, y, z));
}

void BoxControllerComponent::Move(const DirectX::XMFLOAT3 displacement, const float minDist)
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot Move. No controller present");
	else
		m_CollisionFlag = m_Controller->move(ToPxVec3(displacement), minDist, 0, nullptr, nullptr);
}

DirectX::XMFLOAT3 BoxControllerComponent::GetPosition() const
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot get position. No controller present");
	else
		return ToXMFLOAT3(m_Controller->getPosition());

	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 BoxControllerComponent::GetFootPosition() const
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot get footposition. No controller present");
	else
		return ToXMFLOAT3(m_Controller->getFootPosition());

	return DirectX::XMFLOAT3();
}

void BoxControllerComponent::SetCollisionIgnoreGroups(const CollisionGroupFlag ignoreGroups)
{
	using namespace physx;

	m_CollisionGroups.word1 = ignoreGroups;

	if (m_Controller != nullptr)
	{
		const auto actor = m_Controller->getActor();
		const auto numShapes = actor->getNbShapes();
		const auto shapes = new PxShape*[numShapes];

		const auto numPointers = actor->getShapes(shapes, numShapes);
		for (PxU32 i = 0; i < numPointers; i++)
		{
			auto shape = shapes[i];
			shape->setSimulationFilterData(m_CollisionGroups);
			//TODO: shouldn't the query filter data be set as well?
		}
		delete[] shapes;
	}
}

void BoxControllerComponent::SetCollisionGroup(const CollisionGroupFlag group)
{
	using namespace physx;

	m_CollisionGroups.word0 = group;

	if (m_Controller != nullptr)
	{
		const auto actor = m_Controller->getActor();
		const auto numShapes = actor->getNbShapes();
		const auto shapes = new PxShape*[numShapes];

		const auto numPointers = actor->getShapes(shapes, numShapes);
		for (PxU32 i = 0; i < numPointers; i++)
		{
			auto shape = shapes[i];
			shape->setSimulationFilterData(m_CollisionGroups);
			shape->setQueryFilterData(m_CollisionGroups);
		}
		delete[] shapes;
	}
}
