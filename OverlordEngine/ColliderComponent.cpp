#include "stdafx.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "GameObject.h"

ColliderComponent::ColliderComponent(std::shared_ptr<physx::PxGeometry>& geometry, const physx::PxMaterial& material,
                                     physx::PxTransform localPose) :
	m_Geometry(geometry),
	m_Material(material),
	m_LocalPose(std::move(localPose)),
	m_pShape(nullptr),
	m_isTrigger(false)
{
}

void ColliderComponent::Initialize(const GameContext&)
{
	auto rigidBody = GetGameObject()->GetComponent<RigidBodyComponent>();

	if (rigidBody == nullptr)
	{
		Logger::LogError(L"[ColliderComponent] Cannot add a Collider to an object that does not have a rigid body");
		return;
	}

	rigidBody->AddCollider(this);
}

void ColliderComponent::Update(const GameContext&)
{
}

void ColliderComponent::Draw(const GameContext&)
{
}

void ColliderComponent::EnableTrigger(const bool isTrigger)
{
	m_isTrigger = isTrigger;
	UpdateTriggerState();
}

void ColliderComponent::SetShape(physx::PxShape* shape)
{
	m_pShape = shape;
	UpdateTriggerState();
}

void ColliderComponent::UpdateTriggerState() const
{
	if (m_pShape != nullptr)
	{
		m_pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
		m_pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
	}
}
