#pragma once
#include "GameObject.h"

class TorusPrefab final : public GameObject
{
	float m_MajorRadius, m_MinorRadius;
	unsigned int m_MajorRadiusSteps, m_MinorRadiusSteps;
	DirectX::XMFLOAT4 m_Color;

public:
	TorusPrefab(float majorRadius = 1.f, unsigned int majorRadiusSteps = 10, float minorRadius = .2f, unsigned int minorRadiusSteps = 5,
	            DirectX::XMFLOAT4 color = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Green));
	virtual ~TorusPrefab() = default;
	TorusPrefab(const TorusPrefab& other) = delete;
	TorusPrefab(TorusPrefab&& other) noexcept = delete;
	TorusPrefab& operator=(const TorusPrefab& other) = delete;
	TorusPrefab& operator=(TorusPrefab&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& gameContext) override;
};
