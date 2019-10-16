#include "stdafx.h"
#include "SpaceShipComponent.h"
#include "TransformComponent.h"
#include "GameScene.h"

SpaceShipComponent::SpaceShipComponent()
	: m_SoundObject()
{}

void SpaceShipComponent::Draw(const GameContext&)
{}

void SpaceShipComponent::Initialize(const GameContext&)
{
	InitBullets();
	InitSounds();
}

void SpaceShipComponent::InitSounds()
{
	m_SoundObject.AddSound("./Resources/Sounds/sp_circle_barrage_shoot_2.wav");
}

void SpaceShipComponent::InitBullets()
{
	for (size_t i = 0; i < m_NumberOfBullets; i++)
	{
		auto bullet = new BulletPrefab();
		auto p = GetGameObject()->GetTransform()->GetPosition();
		bullet->GetTransform()->Translate(p.x, p.y - 1, p.z - 300);
		bullet->SetActive(m_Active);
		m_Bullets.push_back(bullet);

		GetGameObject()->GetScene()->AddChild(bullet);
	}
}

void SpaceShipComponent::Update(const GameContext& gameContext)
{
	if (m_Active) {

		m_FireRate += gameContext.pGameTime->GetElapsed();

		if (m_FireRate >= m_NextFire)
		{
			auto pos = GetGameObject()->GetTransform()->GetPosition();
			m_Bullets[m_CurrentBulletIndex]->GetTransform()->Translate(pos.x, pos.y - 1, pos.z + 9);
			m_FireRate -= m_NextFire;
			m_CurrentBulletIndex++;
			if (m_CurrentBulletIndex >= int(m_Bullets.size()))
				m_CurrentBulletIndex = 0;

			m_SoundObject.Play(0);
		}
	}
}

void SpaceShipComponent::SetActive(bool active)
{
	m_Active = active;
	for (auto bullet : m_Bullets)
		bullet->SetActive(active);
}

void SpaceShipComponent::ReloadBullets()
{
	m_FireRate = 0.f;
	m_CurrentBulletIndex = 0;

	for (auto bullet : m_Bullets)
	{
		auto p = GetGameObject()->GetTransform()->GetPosition();
		bullet->GetTransform()->Translate(p.x, p.y - 1, p.z - 300);
	}
}

