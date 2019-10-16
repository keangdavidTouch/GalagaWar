#pragma once
#include "GameObject.h"

class AsteroidPrefab : public GameObject
{
public:

	AsteroidPrefab(int type, float scale = 3) : m_Type(type), m_Scale(scale) {};
	AsteroidPrefab(const AsteroidPrefab& other) = delete;
	AsteroidPrefab(AsteroidPrefab&& other) noexcept = delete;
	AsteroidPrefab& operator=(const AsteroidPrefab& other) = delete;
	AsteroidPrefab& operator=(AsteroidPrefab&& other) noexcept = delete;
	virtual ~AsteroidPrefab() = default;

protected:
	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& ) override;
private:
	float m_Scale;
	int m_Type;
};

