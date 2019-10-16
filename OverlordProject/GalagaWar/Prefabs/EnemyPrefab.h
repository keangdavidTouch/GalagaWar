#pragma once
#include "GameObject.h"
#include "BulletPrefab.h"
#include "../Paths/BezierPath.h"
class ModelComponent;
class ParticleEmitterComponent;

class EnemyPrefab : public GameObject
{
public:

	EnemyPrefab(bool active = true);
	EnemyPrefab(const EnemyPrefab& other) = delete;
	EnemyPrefab(EnemyPrefab&& other) noexcept = delete;
	EnemyPrefab& operator=(const EnemyPrefab& other) = delete;
	EnemyPrefab& operator=(EnemyPrefab&& other) noexcept = delete;
	virtual ~EnemyPrefab();

	void SetScale(float scale);
	void SetDead(bool dead);
	bool IsDead() const;
	bool IsFinish();
	bool ShouldRestart() const;
	void SetTarget(GameObject* target);
	BezierPath* GetBezier();

	void AddFollower(EnemyPrefab* enemy);
	void SetFollowers(std::vector<EnemyPrefab*> followers);
	void SetPath(std::vector<DirectX::XMFLOAT3>& paths);

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& ) override;
private:

	float m_Offset = 8.f;
	bool m_Dead = false;
	float m_Scale = 2.f;
	float m_FireRate = 3.f;
	float m_FireElapsedSec = 3.f;
	const std::wstring m_Tag = L"Enemy";
	PathTransform m_PathTransform;
	BezierPath* m_BezierPath;
	ModelComponent* m_Model;
	ParticleEmitterComponent* m_ParticleEmitter1 = nullptr;
	ParticleEmitterComponent* m_ParticleEmitter2 = nullptr;
	GameObject* m_Target = nullptr;
	GameObject* m_ExplodeParticle = nullptr;
	BulletPrefab* m_Bullet = nullptr;
	EnemyPrefab* m_Parent = nullptr;
	std::vector<EnemyPrefab*> m_Followers;

	// Functions
	void AddExplodeParticle();
	void SpawnPickupCoin();
	bool IsInShootingBoundary();
	void HandleShooting(const GameContext& gameContext);
	void Shoot(GameObject* object);
};

