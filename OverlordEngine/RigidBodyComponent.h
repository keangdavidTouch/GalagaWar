#pragma once
#include "BaseComponent.h"

class ColliderComponent;

enum RigidBodyConstraintFlag
{
	RotX = 1 << 0,
	RotY = 1 << 1,
	RotZ = 1 << 2,
	TransX = 1 << 3,
	TransY = 1 << 4,
	TransZ = 1 << 5
};

class RigidBodyComponent : public BaseComponent
{
	// TODO: get rid of this ugly dependency
	friend class TransformComponent;
	friend class ColliderComponent;

	physx::PxRigidActor* m_pActor;
	std::vector<ColliderComponent*> m_Colliders;

	bool m_isStatic;
	bool m_isKinematic;

	physx::PxD6Joint* m_pConstraintJoint;
	physx::PxFilterData m_CollisionGroups;

	unsigned int m_InitialConstraints;

public:
	RigidBodyComponent(const RigidBodyComponent& other) = delete;
	RigidBodyComponent(RigidBodyComponent&& other) noexcept = delete;
	RigidBodyComponent& operator=(const RigidBodyComponent& other) = delete;
	RigidBodyComponent& operator=(RigidBodyComponent&& other) noexcept = delete;
	RigidBodyComponent(bool isStatic = false);
	virtual ~RigidBodyComponent();

	virtual bool IsStatic()  const { return m_isStatic; }
	virtual bool IsDynamic()  const { return !m_isStatic; }
	virtual bool IsKinematic() const { return m_isKinematic; }

	void SetKinematic(bool isKinematic);
	void PutToSleep() const;

	physx::PxRigidBody* GetPxRigidBody() const;
	void AddForce(const physx::PxVec3& force, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE,
	              bool autowake = true) const;
	void AddTorque(const physx::PxVec3& torque, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE,
	               bool autowake = true) const;
	void ClearForce(physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE) const;
	void ClearTorque(physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE) const;

	void SetConstraint(RigidBodyConstraintFlag flag, bool enable);
	void SetCollisionGroup(CollisionGroupFlag group);
	void SetCollisionIgnoreGroups(CollisionGroupFlag ignoreGroups);
	void SetDensity(float density) const;

protected:

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

	virtual void CreateActor();
	void AddCollider(ColliderComponent* collider);

	void Translate(DirectX::XMFLOAT3 position) const;
	void Rotate(DirectX::XMFLOAT4 rotation) const;

	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMFLOAT4 GetRotation() const;
};
