#pragma once
#include "GameObject.h"

struct GameContext;
class CameraComponent;

class FixedCamera final : public GameObject
{
protected:
	void Initialize(const GameContext& gameContext) override;
};
