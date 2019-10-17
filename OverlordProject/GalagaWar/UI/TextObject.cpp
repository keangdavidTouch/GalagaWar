#include "stdafx.h"
#include "TextObject.h"
#include "SpriteFont.h"
#include "TextRenderer.h"

TextObject::TextObject(SpriteFont* pFont, std::wstring text, DirectX::XMFLOAT2& pos) 
	: m_pFont(pFont), 
	m_Text(std::move(text)),
	m_Pos(pos),
	m_Color(DirectX::XMFLOAT4(DirectX::Colors::White))
{
}

void TextObject::Draw(const GameContext & )
{
	if (m_pFont->IsValid())
		TextRenderer::GetInstance()->DrawText(m_pFont, m_Text, m_Pos, m_Color);
}
