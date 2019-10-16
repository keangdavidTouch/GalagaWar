#pragma once
#include "UIObject.h"
#include "OverlordGame.h"
#include <functional>

class PauseUIObject : public UIObject
{
public:
	PauseUIObject(SpriteFont* font, std::function<void()> functor);
	PauseUIObject(const PauseUIObject& other) = delete;
	PauseUIObject(PauseUIObject&& other) noexcept = delete;
	PauseUIObject& operator=(const PauseUIObject& other) = delete;
	PauseUIObject& operator=(PauseUIObject&& other) noexcept = delete;
	~PauseUIObject() = default;

protected:
	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext&) override;
	void Update(const GameContext& gameContext) override;
private:

	bool m_ShouldSelectButton = false;
	int m_SelectedButtonIndex = 0;
	const float m_ButtonPadding = 100.f;
	const float HalfWidth = (float)OverlordGame::GetGameSettings().Window.Width / 2.f;
	const float HalfHeight = (float)OverlordGame::GetGameSettings().Window.Height / 2.f;
	
	DirectX::XMFLOAT4 m_TextColor;
	std::wstring m_TextString;
	const std::wstring m_PauseString = L" PAUSE ";
	SpriteFont* m_pFont;
	TextObject* m_Text;

	std::function<void()> m_RestartCallback;

	std::vector<SpriteComponent*> m_Sprites;
	std::vector<GameObject*> m_Buttons;

	void UpdateTriggerEvent(const GameContext & context);
	void ResetButtons();
	void UpdateText();
};