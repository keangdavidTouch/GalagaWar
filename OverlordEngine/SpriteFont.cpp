#include "stdafx.h"
#include "SpriteFont.h"

SpriteFont::SpriteFont():
	m_TextCache(std::vector<TextCache>()),
	m_FontName(L""),
	m_CharacterCount(0),
	m_CharacterSpacing(1),
	m_TextureWidth(0),
	m_TextureHeight(0),
	m_BufferStart(0),
	m_BufferSize(0),
	m_pTexture(nullptr),
	m_FontSize(0),
	m_IsAddedToRenderer(false)
{
	for (auto & i : m_CharTable)
	{
		ZeroMemory(static_cast<void*>(&i), sizeof(FontMetric));
		i.IsValid = false;
	}
}

bool SpriteFont::IsCharValid(const wchar_t& character)
{
	if (character >= MIN_CHAR_ID && character <= MAX_CHAR_ID)
		return true;

	return false;
}

void SpriteFont::AddToTextCache(TextCache&& cache)
{
	m_TextCache.emplace_back(cache);
}

void SpriteFont::ClearCache()
{
	m_TextCache.clear();
}
