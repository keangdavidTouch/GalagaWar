#pragma once
#include "GameObject.h"

class GalagaCamera final : public GameObject
{
public:
	void Shake(const GameContext& gameContext);
	void Animate(std::function<void()> callback);
	void SetTarget(GameObject* target);
protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
private:
	bool m_ShouldAnimated = false;
	float m_CameraLerp = 0.0f;
	std::function<void()> m_AnimationCallback;
	GameObject* m_Target = nullptr;
};
