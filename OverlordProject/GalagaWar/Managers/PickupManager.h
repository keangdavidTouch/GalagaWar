#pragma once
#include "Singleton.h"
#include "../Prefabs/CoinPrefab.h"

class PickupManager : public Singleton<PickupManager>
{

public:
	PickupManager(const PickupManager& other) = delete;
	PickupManager(PickupManager&& other) noexcept = delete;
	PickupManager& operator=(const PickupManager& other) = delete;
	PickupManager& operator=(PickupManager&& other) noexcept = delete;
	PickupManager() = default;
	virtual ~PickupManager() = default;

	//void Update(const GameContext& context);
	void IncrementCoin() { m_CollectedCoin += 1; };
	int GetCollectedCoin() { return m_CollectedCoin; };
	void SetCoinPickupCount(GameScene* scene, int count);
	void RequestCoin(DirectX::XMFLOAT3& position);
	void ResetCoins();

private:
	//Pickup
	int m_CollectedCoin = 0;
	std::vector<CoinPrefab*> m_Coins{};
};