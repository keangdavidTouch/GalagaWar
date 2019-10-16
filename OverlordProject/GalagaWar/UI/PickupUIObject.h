#pragma once
#include "UIObject.h" 
#include "GameScene.h"

class PickupUIObject : public UIObject
{
public:
	PickupUIObject(SpriteFont* font);
	PickupUIObject(const PickupUIObject& other) = delete;
	PickupUIObject(PickupUIObject&& other) noexcept = delete;
	PickupUIObject& operator=(const PickupUIObject& other) = delete;
	PickupUIObject& operator=(PickupUIObject&& other) noexcept = delete;
	~PickupUIObject();

protected:
	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext&) override;
	void Update(const GameContext& gameContext) override;
private:
	TextObject* m_TextObject = nullptr;
};