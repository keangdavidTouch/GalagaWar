#pragma once
#include "GameObject.h"

class CubePrefab final : public GameObject
{
	float m_Width, m_Height, m_Depth;
	DirectX::XMFLOAT4 m_Color;

public:
	CubePrefab(float width = 1.f, float height = 1.f, float depth = 1.f,
	           DirectX::XMFLOAT4 color = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Red));

	CubePrefab(const CubePrefab& other) = delete;
	CubePrefab(CubePrefab&& other) noexcept = delete;
	CubePrefab& operator=(const CubePrefab& other) = delete;
	CubePrefab& operator=(CubePrefab&& other) noexcept = delete;
	virtual ~CubePrefab() = default;

protected:
	void Initialize(const GameContext& gameContext) override;
};
