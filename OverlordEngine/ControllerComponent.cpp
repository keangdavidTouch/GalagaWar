#include "stdafx.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"
#include "ControllerComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "GameScene.h"

ControllerComponent::ControllerComponent(physx::PxMaterial* material, float radius, float height, std::wstring name,
                                         physx::PxCapsuleClimbingMode::Enum climbingMode)
	: m_Radius(radius),
	  m_Height(height),
	  m_Name(std::move(name)),
	  m_Controller(nullptr),
	  m_ClimbingMode(climbingMode),
	  m_pMaterial(material),
	  m_CollisionFlag(physx::PxControllerCollisionFlags()),
	  m_CollisionGroups(physx::PxFilterData(CollisionGroupFlag::Group0, 0, 0, 0))
{
}

void ControllerComponent::Initialize(const GameContext&)
{
	if (m_Controller != nullptr)
	{
		Logger::LogError(L"[ControllerComponent] Cannot initialize a controller twice");
		return;
	}

	//TODO: 1. Retrieve the ControllerManager from the PhysX Proxy (PhysxProxy::GetControllerManager();)
	physx::PxControllerManager* manager{  GetGameObject()->GetScene()->GetPhysxProxy()->GetControllerManager() };
	//TODO: 2. Create a PxCapsuleControllerDesc (Struct)
	physx::PxCapsuleControllerDesc capsuleDesc{};
	//  > Call the "setToDefault()" method of the PxCapsuleControllerDesc
	capsuleDesc.setToDefault();
	//	> Fill in all the required fields
	
	//  > Radius, Height, ClimbingMode, UpDirection (PxVec3(0,1,0)), ContactOffset (0.1f), Material [See Initializer List]
	capsuleDesc.radius = m_Radius;
	capsuleDesc.height = m_Height;
	capsuleDesc.climbingMode = m_ClimbingMode;
	capsuleDesc.upDirection = physx::PxVec3(0,1,0);
	capsuleDesc.contactOffset = 0.1f;
	capsuleDesc.material = m_pMaterial;
	//  > Position -> Use the position of the parent GameObject
	auto position = GetGameObject()->GetTransform()->GetPosition();
	capsuleDesc.position = physx::PxExtendedVec3(position.x, position.y, position.z);
	//  > UserData -> This component
	capsuleDesc.userData = this;

	//3. Create the controller object (m_pController), use the ControllerManager to do that (CHECK IF VALID!!)
	m_Controller = manager->createController(capsuleDesc);


	if (m_Controller == nullptr)
	{
		Logger::LogError(L"[ControllerComponent] Failed to create controller");
		return;
	}

	//TODO: 4. Set the controller's name (use the value of m_Name) [PxController::setName]
	//   > Converting 'wstring' to 'string' > Use one of the constructor's of the string class
	//	 > Converting 'string' to 'char *' > Use one of the string's methods ;)
	m_Controller->getActor()->setName(std::string(m_Name.begin(), m_Name.end()).c_str());
	
	//TODO: 5. Set the controller's actor's userdata > This Component
	m_Controller->setUserData(this);

	SetCollisionGroup(static_cast<CollisionGroupFlag>(m_CollisionGroups.word0));
	SetCollisionIgnoreGroups(static_cast<CollisionGroupFlag>(m_CollisionGroups.word1));
}

void ControllerComponent::Update(const GameContext&)
{
}

void ControllerComponent::Draw(const GameContext&)
{
}

void ControllerComponent::Translate(const DirectX::XMFLOAT3& position) const
{
	Translate(position.x, position.y, position.z);
}

void ControllerComponent::Translate(const float x, const float y, const float z) const
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot Translate. No controller present");
	else
		m_Controller->setPosition(physx::PxExtendedVec3(x, y, z));
}

void ControllerComponent::Move(const DirectX::XMFLOAT3 displacement, const float minDist)
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot Move. No controller present");
	else
		m_CollisionFlag = m_Controller->move(ToPxVec3(displacement), minDist, 0, nullptr, nullptr);
}

DirectX::XMFLOAT3 ControllerComponent::GetPosition() const
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot get position. No controller present");
	else
		return ToXMFLOAT3(m_Controller->getPosition());

	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 ControllerComponent::GetFootPosition() const
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot get footposition. No controller present");
	else
		return ToXMFLOAT3(m_Controller->getFootPosition());

	return DirectX::XMFLOAT3();
}

void ControllerComponent::SetCollisionIgnoreGroups(const CollisionGroupFlag ignoreGroups)
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

void ControllerComponent::SetCollisionGroup(const CollisionGroupFlag group)
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
