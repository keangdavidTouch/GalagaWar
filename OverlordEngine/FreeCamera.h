#pragma once
#include "GameObject.h"

struct GameContext;
class CameraComponent;

class FreeCamera final : public GameObject
{
	float m_TotalPitch, m_TotalYaw;
	float m_MoveSpeed, m_RotationSpeed, m_SpeedMultiplier;
	CameraComponent* m_pCamera;
public:
	FreeCamera();
	virtual ~FreeCamera() = default;
	FreeCamera(const FreeCamera& other) = delete;
	FreeCamera(FreeCamera&& other) noexcept = delete;
	FreeCamera& operator=(const FreeCamera& other) = delete;
	FreeCamera& operator=(FreeCamera&& other) noexcept = delete;

	void SetRotation(float pitch, float yaw);

protected:

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
};
