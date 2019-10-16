#pragma once
#include "Particle.h"

class DropParticle final : public Particle
{
public:
	explicit DropParticle(const ParticleEmitterSettings& emitterSettings);
	~DropParticle() = default;

	DropParticle(const DropParticle& other) = delete;
	DropParticle(DropParticle&& other) noexcept = delete;
	DropParticle& operator=(const DropParticle& other) = delete;
	DropParticle& operator=(DropParticle&& other) noexcept = delete;

	void Update(const GameContext& context) override;
	void Init(DirectX::XMFLOAT3 initPosition, int index) override;
};

