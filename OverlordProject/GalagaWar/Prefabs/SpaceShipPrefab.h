#pragma once
#include "GameObject.h"

class BoxControllerComponent;
class SpaceShipComponent;
class ParticleEmitterComponent;

class SpaceShipPrefab : public GameObject
{
public:

	enum SpaceshipMovement : UINT
	{
		LEFT = 0, RIGHT, FORWARD, BACKWARD
	};

	SpaceShipPrefab();
	SpaceShipPrefab(const SpaceShipPrefab& other) = delete;
	SpaceShipPrefab(SpaceShipPrefab&& other) noexcept = delete;
	SpaceShipPrefab& operator=(const SpaceShipPrefab& other) = delete;
	SpaceShipPrefab& operator=(SpaceShipPrefab&& other) noexcept = delete;
	virtual ~SpaceShipPrefab() = default;
	void Reset();

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& ) override;
	void IsTrigger(GameObject* triggerobject, GameObject* otherobject, TriggerAction action);
private:

	bool m_Dead = false;
	const float m_MaxBoundaryWidth = 65;
	const float m_MaxBoundaryHeight = 70;
	float m_MaxVelocity,
		m_MoveAccelerationTime,
		m_MoveAcceleration,
		m_CurrentVelocity,
		m_RotationVelocity,
		m_PreviousXPos;
	DirectX::XMFLOAT3 m_Velocity;
	const std::wstring m_EnemyTag = L"Enemy";
	const std::wstring m_EnemyBulletTag = L"EnemyBullet";
	const std::wstring m_CoinPickupTag = L"Coin";

	GameObject* m_MeshObject = nullptr;
	GameObject* m_ExplosionParticle = nullptr;
	SpaceShipComponent* m_Component = nullptr;
	BoxControllerComponent* m_pController = nullptr;
	ParticleEmitterComponent* m_ExplodeEmiiter = nullptr;
	ParticleEmitterComponent* m_ExplodeEmiiter2 = nullptr;
	ParticleEmitterComponent* m_JetFlareParticle = nullptr;
	ParticleEmitterComponent* m_JetFlareParticle2 = nullptr;
	
	ParticleEmitterComponent* GetFlareParticle(DirectX::XMFLOAT3 position, const GameContext& gameContexts);
	void MoveController(DirectX::XMFLOAT3& position);
	void AddExplodeParticle();
	void RegisterInput(const GameContext& gameContext);
	void MoveWithKeyboardAndController(const GameContext& gameContext);
};

