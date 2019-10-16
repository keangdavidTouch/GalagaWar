#pragma once
#include "GameObject.h"

class SpherePrefab final : public GameObject
{
	float m_Radius;
	int m_Steps;
	DirectX::XMFLOAT4 m_Color;

public:
	SpherePrefab(float radius = 1.f, int steps = 10, DirectX::XMFLOAT4 color = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Blue));
	virtual ~SpherePrefab() = default;

	SpherePrefab(const SpherePrefab& other) = delete;
	SpherePrefab(SpherePrefab&& other) noexcept = delete;
	SpherePrefab& operator=(const SpherePrefab& other) = delete;
	SpherePrefab& operator=(SpherePrefab&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& gameContext) override;
};
