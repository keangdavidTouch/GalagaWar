#pragma once
#include "BaseComponent.h"
#include "../Prefabs/BulletPrefab.h"
#include "../Sound/SoundObject.h"

class SpaceShipComponent : public BaseComponent
{
public:
	SpaceShipComponent();
	virtual ~SpaceShipComponent() = default;
	SpaceShipComponent(const SpaceShipComponent& other) = delete;
	SpaceShipComponent(SpaceShipComponent&& other) noexcept = delete;
	SpaceShipComponent& operator=(const SpaceShipComponent& other) = delete;
	SpaceShipComponent& operator=(SpaceShipComponent&& other) noexcept = delete;

	void SetActive(bool active);
	void ReloadBullets();
protected:
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void Initialize(const GameContext& gameContext) override;
private:

	bool m_Active = false;
	int m_CurrentBulletIndex = 0;
	const size_t m_NumberOfBullets = 9;
	float m_FireRate = 0.0f;
	const float m_NextFire = 0.25f;

	SoundObject m_SoundObject;
	std::vector<BulletPrefab*> m_Bullets;

	void InitSounds();
	void InitBullets();
};
