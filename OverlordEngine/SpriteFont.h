#pragma once

struct TextCache
{
	TextCache(std::wstring  text, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT4 col) :
		Text(std::move(text)), Position(pos), Color(col)
	{
	}
	~TextCache() = default;
	// needed in textrenderer
	TextCache(const TextCache& other) = default;
	// needed in textrenderer
	TextCache(TextCache&& other) noexcept = default;
	TextCache& operator=(const TextCache& other) = delete;
	TextCache& operator=(TextCache&& other) noexcept = delete;

	const std::wstring Text;
	DirectX::XMFLOAT2 Position;
	DirectX::XMFLOAT4 Color;
};

struct FontMetric
{
	bool IsValid;
	wchar_t Character;
	unsigned short Width;
	unsigned short Height;
	short OffsetX;
	short OffsetY;
	short AdvanceX;
	unsigned char Page;
	unsigned char Channel;
	DirectX::XMFLOAT2 TexCoord;
};

class TextureData;
class TextRenderer;

class SpriteFont final
{
public:
	SpriteFont();
	~SpriteFont() = default;

	SpriteFont(const SpriteFont& other) = delete;
	SpriteFont(SpriteFont&& other) noexcept = delete;
	SpriteFont& operator=(const SpriteFont& other) = delete;
	SpriteFont& operator=(SpriteFont&& other) noexcept = delete;

	static bool IsCharValid(const wchar_t& character);

	static const int MAX_CHAR_ID = 255;
	static const int MIN_CHAR_ID = 0;
	static const int CHAR_COUNT = MAX_CHAR_ID - MIN_CHAR_ID + 1;

	const FontMetric& GetMetric(const wchar_t& character) const { return m_CharTable[character - MIN_CHAR_ID]; }

	void AddToTextCache(TextCache&& cache);
	void ClearCache();

	const std::vector<TextCache>& GetTextCache() const { return m_TextCache; }
	const std::wstring& GetFontName() const { return m_FontName; }
	int GetBufferStart() const	{ return m_BufferStart; }
	void SetBufferStart(int start) { m_BufferStart = start; }
	int GetBufferSize() const { return m_BufferSize; }
	void SetBufferSize(int size) { m_BufferSize = size; }
	TextureData* GetTexture() const { return m_pTexture; }
	bool IsAddedToRenderer() const { return m_IsAddedToRenderer; }
	void SetAddedToRenderer(bool set) { m_IsAddedToRenderer = set; }
	int GetTextureWidth() const { return m_TextureWidth; }
	int GetTextureHeight() const { return m_TextureHeight; }

	bool IsValid() { return GetFontName() != L"";  }

private:
	// TODO: get rid of these nasty dependency
	friend class SpriteFontLoader;

	FontMetric& GetMetric(const wchar_t& character) { return m_CharTable[character - MIN_CHAR_ID]; };

	FontMetric m_CharTable[CHAR_COUNT]{};
	std::vector<TextCache> m_TextCache;
	std::wstring m_FontName;
	int m_CharacterCount;
	int m_CharacterSpacing;
	int m_TextureWidth;
	int m_TextureHeight;
	int m_BufferStart, m_BufferSize;
	TextureData* m_pTexture;
	short m_FontSize;
	bool m_IsAddedToRenderer;
};
