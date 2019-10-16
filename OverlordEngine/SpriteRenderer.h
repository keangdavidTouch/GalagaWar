#pragma once
#include "Singleton.h"

class TextureData;
class OverlordGame;
class GameScene;

struct SpriteVertex
{
	unsigned int TextureId;
	DirectX::XMFLOAT4 TransformData;
	DirectX::XMFLOAT4 TransformData2;
#ifdef SPRITE_EFFECT_SUPPORT
	DirectX::XMFLOAT4 SpriteData;
#endif
	DirectX::XMFLOAT4 Color;

	bool Equals(const SpriteVertex& source) const
	{
#ifdef SPRITE_EFFECT_SUPPORT
		if (!XMFloat4Equals(source.SpriteData, SpriteData))
			return false;
#endif
		if (source.TextureId != TextureId)return false;
		if (!XMFloat4Equals(source.TransformData, TransformData))return false;
		if (!XMFloat4Equals(source.TransformData2, TransformData2))return false;
		if (!XMFloat4Equals(source.Color, Color))return false;

		return true;
	}
};

class SpriteRenderer final : public Singleton<SpriteRenderer>
{
public:
	SpriteRenderer(const SpriteRenderer& other) = delete;
	SpriteRenderer(SpriteRenderer&& other) noexcept = delete;
	SpriteRenderer& operator=(const SpriteRenderer& other) = delete;
	SpriteRenderer& operator=(SpriteRenderer&& other) noexcept = delete;

	void Draw(TextureData* pTexture, SpriteVertex& v) const;
	void Draw(TextureData* pTexture, DirectX::XMFLOAT2 position,
	          DirectX::XMFLOAT4 color = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White),
	          DirectX::XMFLOAT2 pivot = DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2 scale = DirectX::XMFLOAT2(1, 1),
	          float rotation = 0.f, float depth = 0.f);
	void DrawImmediate(const GameContext& gameContext, ID3D11ShaderResourceView* pSrv, DirectX::XMFLOAT2 position,
	                   DirectX::XMFLOAT4 color = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White),
	                   DirectX::XMFLOAT2 pivot = DirectX::XMFLOAT2(0, 0),
	                   DirectX::XMFLOAT2 scale = DirectX::XMFLOAT2(1, 1), float rotation = 0.f);

private:
	// TODO get rid of these nasty dependencies
	friend class Singleton<SpriteRenderer>;
	friend class OverlordGame;
	friend class GameScene;

	SpriteRenderer();
	~SpriteRenderer();

	void InitRenderer(ID3D11Device* pDevice);
	void Draw(const GameContext& gameContext);
	void UpdateBuffer(const GameContext& gameContext);

	std::vector<SpriteVertex> m_Sprites;
	std::vector<TextureData*> m_Textures;
	unsigned int m_BufferSize, m_InputLayoutSize;
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer *m_pVertexBuffer, *m_pImmediateVertexBuffer;
	SpriteVertex m_ImmediateVertex{};

	ID3DX11EffectMatrixVariable* m_pTransfromMatrixV;
	DirectX::XMFLOAT4X4 m_Transform;
	ID3DX11EffectVectorVariable* m_pTextureSizeV;
	ID3DX11EffectShaderResourceVariable* m_pTextureSRV;

	static bool SpriteSortByTexture(const SpriteVertex& v0, const SpriteVertex& v1);
	static bool SpriteSortByDepth(const SpriteVertex& v0, const SpriteVertex& v1);
};
