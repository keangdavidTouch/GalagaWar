#pragma once
#include "UIObject.h"

class SettingUIObject : public UIObject
{
public:
	SettingUIObject(SpriteFont* font);
	SettingUIObject(const SettingUIObject& other) = delete;
	SettingUIObject(SettingUIObject&& other) noexcept = delete;
	SettingUIObject& operator=(const SettingUIObject& other) = delete;
	SettingUIObject& operator=(SettingUIObject&& other) noexcept = delete;
	~SettingUIObject() = default;

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
private:

	int m_SelectedButtonIndex = 0;
	SpriteFont* m_pFont;

	std::vector<SpriteComponent*> m_Sprites;
	std::vector<GameObject*> m_Buttons;
};