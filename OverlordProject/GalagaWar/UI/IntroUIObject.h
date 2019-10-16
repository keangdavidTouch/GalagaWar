#pragma once
#include "UIObject.h"

class IntroUIObject : public UIObject
{
public:
	IntroUIObject();
	IntroUIObject(const IntroUIObject& other) = delete;
	IntroUIObject(IntroUIObject&& other) noexcept = delete;
	IntroUIObject& operator=(const IntroUIObject& other) = delete;
	IntroUIObject& operator=(IntroUIObject&& other) noexcept = delete;
	~IntroUIObject();

protected:
	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext&) override;
	void Update(const GameContext& gameContext) override;
private:

	float m_Speed;
	float m_AnimatedElapsedSec;

	TextObject* m_TextObject;
	GameObject* m_PlayButton;
	SpriteComponent* m_PlayButtonSprite;
	SpriteComponent* m_PlayButtonFrameSprite;
};