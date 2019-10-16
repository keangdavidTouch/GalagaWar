#include "stdafx.h"
#include "GameScene.h"
#include "PickupManager.h"
#include "TransformComponent.h"

void PickupManager::Update(const GameContext &)
{
}

void PickupManager::SetCoinPickupCount(GameScene* scene, int count)
{
	for (int i = 0; i <= count; i++) {
		auto coin = new CoinPrefab();
		coin->SetAsDropped(false);
		m_Coins.push_back(coin);
		scene->AddChild(coin);
	}
}

void PickupManager::RequestCoin(DirectX::XMFLOAT3& position)
{
	for (auto coin : m_Coins) {
		if (!coin->IsOccupy()) {
			coin->SetAsDropped(true);
			coin->GetTransform()->Translate(position);
			break;
		}
	}
}

void PickupManager::ResetCoins()
{
	m_CollectedCoin = 0;

	for (auto coin : m_Coins) {
		coin->SetAsDropped(false);
	}
}

