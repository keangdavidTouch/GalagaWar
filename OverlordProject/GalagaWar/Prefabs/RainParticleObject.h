#pragma once
#include "GameObject.h"

class RainParticleObject : public GameObject
{
public:
	RainParticleObject() = default;
	~RainParticleObject() = default;
	RainParticleObject(const RainParticleObject& other) = delete;
	RainParticleObject(RainParticleObject&& other) noexcept = delete;
	RainParticleObject& operator=(const RainParticleObject& other) = delete;
	RainParticleObject& operator=(RainParticleObject&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& gameContext) override;
private:
};