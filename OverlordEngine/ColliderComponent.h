#pragma once
#include "BaseComponent.h"

class ColliderComponent : public BaseComponent
{
	friend class RigidBodyComponent; // TODO: get rid of this ugly dependency

	std::shared_ptr<physx::PxGeometry> m_Geometry;
	const physx::PxMaterial& m_Material;
	const physx::PxTransform m_LocalPose;
	physx::PxShape* m_pShape;
	bool m_isTrigger;

	void SetShape(physx::PxShape* shape);
	void UpdateTriggerState() const;

public:
	ColliderComponent(const ColliderComponent& other) = delete;
	ColliderComponent(ColliderComponent&& other) noexcept = delete;
	ColliderComponent& operator=(const ColliderComponent& other) = delete;
	ColliderComponent& operator=(ColliderComponent&& other) noexcept = delete;
	ColliderComponent(std::shared_ptr<physx::PxGeometry>& geometry, const physx::PxMaterial& material, physx::PxTransform localPose = physx::PxTransform::createIdentity());
	~ColliderComponent() = default;

	physx::PxShape* GetShape() const { return m_pShape; }
	bool IsTrigger() const { return m_isTrigger; }
	void EnableTrigger(bool isTrigger);

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
};

