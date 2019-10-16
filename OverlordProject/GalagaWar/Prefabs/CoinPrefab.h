#pragma once
#include "GameObject.h"
#include "../Sound/SoundObject.h" 

class CoinPrefab : public GameObject
{
public:
	CoinPrefab();
	CoinPrefab(const CoinPrefab& other) = delete;
	CoinPrefab(CoinPrefab&& other) noexcept = delete;
	CoinPrefab& operator=(const CoinPrefab& other) = delete;
	CoinPrefab& operator=(CoinPrefab&& other) noexcept = delete;
	~CoinPrefab() = default;

	void GetCollected();
	bool IsOccupy() { return m_IsDropped;}
	void SetAsDropped(bool occupy);

protected:
	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext&) override;
	void Update(const GameContext& gameContext) override;
private:
	bool m_IsDropped = false;
	float m_PickUpElaspedSec = 0.f;
	const float m_MaxDropTime = 5.f;
	const float m_PickupTime = 10.f;
	const std::wstring m_Tag = L"Coin";
	SoundObject m_SoundObject;
	GameObject* m_MeshGameObject;
	GameObject* m_Particle;

	void AddParticle();
};