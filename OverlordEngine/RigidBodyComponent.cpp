#include "stdafx.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"
#include "GameObject.h"
#include "GameScene.h"

RigidBodyComponent::RigidBodyComponent(bool isStatic) :
	m_pActor(nullptr),
	m_isStatic(isStatic),
	m_isKinematic(false),
	m_pConstraintJoint(nullptr),
	m_CollisionGroups(physx::PxFilterData(CollisionGroupFlag::Group0, 0, 0, 0)),
	m_InitialConstraints(0)
{
}

RigidBodyComponent::~RigidBodyComponent()
{
	if (m_pActor != nullptr)
	{
		auto physxScene = GetGameObject()->GetScene()->GetPhysxProxy()->GetPhysxScene();
		physxScene->removeActor(*m_pActor);
		m_pActor->release();
	}
}

void RigidBodyComponent::Initialize(const GameContext& )
{
}

void RigidBodyComponent::Update(const GameContext& )
{
}

void RigidBodyComponent::Draw(const GameContext& )
{
}

void RigidBodyComponent::AddCollider(ColliderComponent* collider)
{
	m_Colliders.push_back(collider);
	if (m_pActor == nullptr)
		CreateActor();
	else
		collider->SetShape(m_pActor->createShape(*collider->m_Geometry, collider->m_Material, collider->m_LocalPose));

	collider->GetShape()->setSimulationFilterData(m_CollisionGroups);
	collider->GetShape()->setQueryFilterData(m_CollisionGroups);
}

void RigidBodyComponent::SetCollisionIgnoreGroups(CollisionGroupFlag ignoreGroups)
{
	m_CollisionGroups.word1 = ignoreGroups;
	for each (auto shape in m_Colliders)
	{
		shape->GetShape()->setSimulationFilterData(m_CollisionGroups);
	}
}

void RigidBodyComponent::SetCollisionGroup(CollisionGroupFlag group)
{
	m_CollisionGroups.word0 = group;
	for each (auto shape in m_Colliders)
	{
		shape->GetShape()->setSimulationFilterData(m_CollisionGroups);
		shape->GetShape()->setQueryFilterData(m_CollisionGroups);
	}
}

void RigidBodyComponent::SetKinematic(bool isKinematic)
{
	if (m_isStatic && isKinematic)
		Logger::LogError(L"[RigidBodyComponent] A static RigidBody can never be kinematic");

	m_isKinematic = isKinematic;

	if (m_pActor != nullptr && !m_isStatic)
		reinterpret_cast<physx::PxRigidDynamic*>(m_pActor)->setRigidDynamicFlag(
			physx::PxRigidDynamicFlag::eKINEMATIC, m_isKinematic);
}

void RigidBodyComponent::CreateActor()
{
	if (m_pActor != nullptr)
		Logger::LogError(L"[RigidBodyComponent] CreateActor cannot be called twice");

	if (m_Colliders.empty())
		return;

	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto physxScene = GetGameObject()->GetScene()->GetPhysxProxy()->GetPhysxScene();

	const auto transform = GetTransform();

	if (m_isStatic)
		m_pActor = physX->createRigidStatic(physx::PxTransform(ToPxVec3(transform->GetPosition()), ToPxQuat(transform->GetRotation()))); //fix rotations!!!
	else
	{
		m_pActor = physX->createRigidDynamic(physx::PxTransform(ToPxVec3(transform->GetPosition()), ToPxQuat(transform->GetRotation())));

		reinterpret_cast<physx::PxRigidDynamic*>(m_pActor)->setRigidDynamicFlag(
			physx::PxRigidDynamicFlag::eKINEMATIC, m_isKinematic);

		if (m_InitialConstraints != 0)
		{
			SetConstraint(RigidBodyConstraintFlag::RotX,
			              static_cast<bool>((m_InitialConstraints & RigidBodyConstraintFlag::RotX) == RigidBodyConstraintFlag::RotX));
			SetConstraint(RigidBodyConstraintFlag::RotY,
			              static_cast<bool>((m_InitialConstraints & RigidBodyConstraintFlag::RotY) == RigidBodyConstraintFlag::RotY));
			SetConstraint(RigidBodyConstraintFlag::RotZ,
			              static_cast<bool>((m_InitialConstraints & RigidBodyConstraintFlag::RotZ) == RigidBodyConstraintFlag::RotZ));
			SetConstraint(RigidBodyConstraintFlag::TransX,
			              static_cast<bool>((m_InitialConstraints & RigidBodyConstraintFlag::TransX) == RigidBodyConstraintFlag::TransX));
			SetConstraint(RigidBodyConstraintFlag::TransY,
			              static_cast<bool>((m_InitialConstraints & RigidBodyConstraintFlag::TransY) == RigidBodyConstraintFlag::TransY));
			SetConstraint(RigidBodyConstraintFlag::TransZ,
			              static_cast<bool>((m_InitialConstraints & RigidBodyConstraintFlag::TransZ) == RigidBodyConstraintFlag::TransZ));
		}
	}

	for (auto collider : m_Colliders)
		collider->SetShape(m_pActor->createShape(*collider->m_Geometry, collider->m_Material, collider->m_LocalPose));

	physxScene->addActor(*m_pActor);
	m_pActor->userData = this;
}

void RigidBodyComponent::SetDensity(float density) const
{
	if (m_pActor && m_pActor->isRigidBody())
	{
		physx::PxRigidBodyExt::updateMassAndInertia(*m_pActor->isRigidBody(), density);
	}
}

void RigidBodyComponent::Translate(DirectX::XMFLOAT3 position) const
{
	if (m_pActor != nullptr && !m_isStatic)
	{
		if (!m_isKinematic)
			m_pActor->setGlobalPose(physx::PxTransform(ToPxVec3(position), ToPxQuat(GetRotation())));
		else
			reinterpret_cast<physx::PxRigidDynamic*>(m_pActor)->setKinematicTarget(
				physx::PxTransform(ToPxVec3(position), ToPxQuat(GetRotation())));
	}
}

void RigidBodyComponent::Rotate(DirectX::XMFLOAT4 rotation) const
{
	if (m_pActor != nullptr && !m_isStatic)
	{
		if (!m_isKinematic)
			m_pActor->setGlobalPose(physx::PxTransform(ToPxVec3(GetPosition()), ToPxQuat(rotation)));
		else
			reinterpret_cast<physx::PxRigidDynamic*>(m_pActor)->setKinematicTarget(
				physx::PxTransform(ToPxVec3(GetPosition()), ToPxQuat(rotation)));
	}
}

DirectX::XMFLOAT3 RigidBodyComponent::GetPosition() const
{
	if (m_pActor != nullptr)
	{
		physx::PxTransform pose;
		if (!m_isKinematic || !reinterpret_cast<physx::PxRigidDynamic*>(m_pActor)->getKinematicTarget(pose))
			pose = m_pActor->getGlobalPose();

		return DirectX::XMFLOAT3(pose.p.x, pose.p.y, pose.p.z);
	}
	Logger::LogError(
		L"[RigidBodyComponent] Calling GetPosition on a rigid body without actor. Did you forget to add a collider?");
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT4 RigidBodyComponent::GetRotation() const
{
	if (m_pActor != nullptr)
	{
		physx::PxTransform pose;
		if (!m_isKinematic || !reinterpret_cast<physx::PxRigidDynamic*>(m_pActor)->getKinematicTarget(pose))
			pose = m_pActor->getGlobalPose();

		return DirectX::XMFLOAT4(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
	}

	Logger::LogError(L"[RigidBodyComponent] Calling GetRotation on a rigid body without actor");
	return DirectX::XMFLOAT4();
}

physx::PxRigidBody* RigidBodyComponent::GetPxRigidBody() const
{
	return reinterpret_cast<physx::PxRigidBody*>(m_pActor);
}

void RigidBodyComponent::PutToSleep() const
{
	if (m_pActor != nullptr && !m_isStatic)
		reinterpret_cast<physx::PxRigidDynamic*>(m_pActor)->putToSleep();
	else
		Logger::LogError(L"[RigidBodyComponent] Cannot put a static actor to sleep");
}

void RigidBodyComponent::AddForce(const physx::PxVec3& force, physx::PxForceMode::Enum mode, bool autowake) const
{
	if (m_pActor != nullptr && !m_isStatic && !m_isKinematic)
		reinterpret_cast<physx::PxRigidDynamic*>(m_pActor)->addForce(force, mode, autowake);
	else
		Logger::LogError(L"[RigidBodyComponent] Cannot add a force to a static or kinematic actor");
}

void RigidBodyComponent::AddTorque(const physx::PxVec3& torque, physx::PxForceMode::Enum mode, bool autowake) const
{
	if (m_pActor != nullptr && !m_isStatic && !m_isKinematic)
		reinterpret_cast<physx::PxRigidDynamic*>(m_pActor)->addTorque(torque, mode, autowake);
	else
		Logger::LogError(L"[RigidBodyComponent] Cannot add a torque to a static or kinematic actor");
}

void RigidBodyComponent::ClearForce(physx::PxForceMode::Enum mode) const
{
	if (m_pActor != nullptr && !m_isStatic && !m_isKinematic)
		reinterpret_cast<physx::PxRigidDynamic*>(m_pActor)->clearForce(mode);
	else
		Logger::LogError(L"[RigidBodyComponent] Cannot clear a force on a static or kinematic actor");
}

void RigidBodyComponent::ClearTorque(physx::PxForceMode::Enum mode) const
{
	if (m_pActor != nullptr && !m_isStatic && !m_isKinematic)
		reinterpret_cast<physx::PxRigidDynamic*>(m_pActor)->clearTorque(mode);
	else
		Logger::LogError(L"[RigidBodyComponent] Cannot clear a torque on a static or kinematic actor");
}

void RigidBodyComponent::SetConstraint(RigidBodyConstraintFlag flag, bool enable)
{
	if (m_isStatic)
	{
		Logger::LogWarning(L"RigidBodyComponent::SetConstraint() > Can't contrain a static actor!");
		return;
	}

	if (m_pActor && !m_pConstraintJoint)
	{
		m_pConstraintJoint = PxD6JointCreate(*PhysxManager::GetInstance()->GetPhysics(), nullptr, m_pActor->getGlobalPose(),
		                                     m_pActor, physx::PxTransform(physx::PxVec3(0, 0, 0)));
	}

	if (m_pConstraintJoint)
	{
		switch (flag)
		{
		case RotX:
			m_pConstraintJoint->setMotion(physx::PxD6Axis::eTWIST,
			                              enable ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
			break;
		case RotY:
			m_pConstraintJoint->setMotion(physx::PxD6Axis::eSWING1,
			                              enable ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
			break;
		case RotZ:
			m_pConstraintJoint->setMotion(physx::PxD6Axis::eSWING2,
			                              enable ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
			break;
		case TransX:
			m_pConstraintJoint->setMotion(physx::PxD6Axis::eX, enable ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
			break;
		case TransY:
			m_pConstraintJoint->setMotion(physx::PxD6Axis::eY, enable ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
			break;
		case TransZ:
			m_pConstraintJoint->setMotion(physx::PxD6Axis::eZ, enable ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
			break;
		default:
			break;
		}

		m_pConstraintJoint->setLocalPose(physx::PxJointActorIndex::eACTOR0, m_pActor->getGlobalPose());
	}
	else
	{
		m_InitialConstraints |= enable ? flag : ~flag;
	}
}
