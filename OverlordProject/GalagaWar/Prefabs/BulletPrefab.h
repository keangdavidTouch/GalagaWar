#pragma once
#include "GameObject.h"
#include "SpriteFont.h"

class BulletPrefab : public GameObject
{
public:

	BulletPrefab(int materialId = 6);
	BulletPrefab(const BulletPrefab& other) = delete;
	BulletPrefab(BulletPrefab&& other) noexcept = delete;
	BulletPrefab& operator=(const BulletPrefab& other) = delete;
	BulletPrefab& operator=(BulletPrefab&& other) noexcept = delete;
	virtual ~BulletPrefab() = default;
	void SetActive(bool pause);
	void SetDirection(DirectX::XMFLOAT3& direction) { m_Direction = direction; };
	void SetMaxVelocity(float vel) { m_MaxVelocity = vel; };

protected :
	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& ) override;
	void Update(const GameContext& gameContext) override;
	void IsTrigger(GameObject* triggerobject, GameObject* otherobject, TriggerAction action);
private:

	bool m_Active;
	int m_MatID;
	float m_MaxVelocity,
		m_AccelerationTime,
		m_Acceleration,
		m_CurrentVelocity;
	DirectX::XMFLOAT3 m_Velocity;
	DirectX::XMFLOAT3 m_Direction;
	const std::wstring m_EnemyTag = L"Enemy";
	const std::wstring m_EnemyBulletTag = L"EnemyBullet";
	
	GameObject* m_Particle = nullptr;
};



