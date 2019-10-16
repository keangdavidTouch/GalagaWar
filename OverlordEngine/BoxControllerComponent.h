#pragma once
#include "BaseComponent.h"

class BoxControllerComponent : public BaseComponent
{
public:
	BoxControllerComponent(const BoxControllerComponent& other) = delete;
	BoxControllerComponent(BoxControllerComponent&& other) noexcept = delete;
	BoxControllerComponent& operator=(const BoxControllerComponent& other) = delete;
	BoxControllerComponent& operator=(BoxControllerComponent&& other) noexcept = delete;

	BoxControllerComponent(physx::PxMaterial* material, float width = 5, float height = 5, float forward = 5, std::wstring name = L"BoxCharacter",
	                    physx::PxCapsuleClimbingMode::Enum climbingMode = physx::PxCapsuleClimbingMode::eEASY);
	virtual ~BoxControllerComponent() = default;

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

	float m_Height;
	float m_Width;
	float m_Forward;
	std::wstring m_Name;
	physx::PxController* m_Controller;
	physx::PxCapsuleClimbingMode::Enum m_ClimbingMode;
	physx::PxMaterial* m_pMaterial;
	physx::PxControllerCollisionFlags m_CollisionFlag;
	physx::PxFilterData m_CollisionGroups;
};
