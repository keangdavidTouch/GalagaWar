#pragma once
#include "GameObject.h"

class SpriteFont;

class TextObject : public GameObject
{
public:
	TextObject(SpriteFont* pFont, std::wstring text, DirectX::XMFLOAT2& pos);
	TextObject(const TextObject& other) = delete;
	TextObject(TextObject&& other) noexcept = delete;
	TextObject& operator=(const TextObject& other) = delete;
	TextObject& operator=(TextObject&& other) noexcept = delete;
	~TextObject() = default;

	void SetColor(DirectX::XMFLOAT4 color) { m_Color = color; }
	void SetPosition(DirectX::XMFLOAT2& pos) { m_Pos = pos; };
	void SetText(std::wstring text) { m_Text = std::move(text); };

	std::wstring& GetText() { return m_Text; };

protected:
	//void Initialize(const GameContext& gameContext) override;
	//void Update(const GameContext& gameContext) override;
	void Draw(const GameContext&) override;

private:
	SpriteFont* m_pFont;
	std::wstring m_Text;
	DirectX::XMFLOAT2 m_Pos;
	DirectX::XMFLOAT4 m_Color;

};