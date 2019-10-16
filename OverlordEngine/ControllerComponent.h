#pragma once
#include "BaseComponent.h"

class ControllerComponent : public BaseComponent
{
public:
	ControllerComponent(const ControllerComponent& other) = delete;
	ControllerComponent(ControllerComponent&& other) noexcept = delete;
	ControllerComponent& operator=(const ControllerComponent& other) = delete;
	ControllerComponent& operator=(ControllerComponent&& other) noexcept = delete;

	ControllerComponent(physx::PxMaterial* material, float radius = 2, float height = 5, std::wstring name = L"Character",
	                    physx::PxCapsuleClimbingMode::Enum climbingMode = physx::PxCapsuleClimbingMode::eEASY);
	virtual ~ControllerComponent() = default;

	void Translate(const DirectX::XMFLOAT3& position) const;
	void Translate(float x, float y, float z) const;
	void Move(DirectX::XMFLOAT3 displacement, float minDist = 0);
	const physx::PxControllerCollisionFlags& GetCollisionFlags() const { return m_CollisionFlag; }
	void SetCollisionGroup(CollisionGroupFlag group);
	void SetCollisionIgnoreGroups(CollisionGroupFlag ignoreGroups);

	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMFLOAT3 GetFootPosition() const;

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

private:

	float m_Radius, m_Height;
	std::wstring m_Name;
	physx::PxController* m_Controller;
	physx::PxCapsuleClimbingMode::Enum m_ClimbingMode;
	physx::PxMaterial* m_pMaterial;
	physx::PxControllerCollisionFlags m_CollisionFlag;
	physx::PxFilterData m_CollisionGroups;
};
