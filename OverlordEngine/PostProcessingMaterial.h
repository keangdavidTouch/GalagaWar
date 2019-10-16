#pragma once
#include "EffectHelper.h"
#include "TextureData.h"

class RenderTarget;
struct GameContext;

class PostProcessingMaterial 
{
public:
	explicit PostProcessingMaterial(std::wstring effectFile, unsigned int renderIndex = 0, std::wstring  technique = L"");
	virtual ~PostProcessingMaterial();

	PostProcessingMaterial(const PostProcessingMaterial& other) = delete;
	PostProcessingMaterial(PostProcessingMaterial&& other) noexcept = delete;
	PostProcessingMaterial& operator=(const PostProcessingMaterial& other) = delete;
	PostProcessingMaterial& operator=(PostProcessingMaterial&& other) noexcept = delete;

	virtual void Initialize(const GameContext& gameContext);

	virtual void SetRenderIndex(unsigned int index) { m_RenderIndex = index; }
	virtual unsigned int GetRenderIndex() const { return m_RenderIndex; }

	virtual RenderTarget* GetRenderTarget() const { return m_pRenderTarget; }
	virtual void Draw(const GameContext& gameContext, RenderTarget* previousRendertarget);

protected:
	ID3DX11Effect* GetEffect() const { return m_pEffect; }

private:
	virtual bool LoadEffect(const GameContext& gameContext, const std::wstring& effectFile);
	virtual void LoadEffectVariables() = 0;
	virtual void UpdateEffectVariables(RenderTarget* rendertarget) = 0;

	void CreateVertexBuffer(const GameContext& gameContext);
	void CreateIndexBuffer(const GameContext& gameContext);

	bool m_IsInitialized;

	ID3D11InputLayout* m_pInputLayout;
	std::vector<ILDescription> m_pInputLayoutDescriptions;
	UINT m_pInputLayoutSize;

	std::wstring m_effectFile;
	UINT m_InputLayoutID;
	unsigned int m_RenderIndex;

	RenderTarget* m_pRenderTarget;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_NumVertices, m_NumIndices;
	unsigned int m_VertexBufferStride;

	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	/*ID3DX11EffectTechnique* m_pTechnique[2];*/
	std::wstring m_TechniqueName;

	TextureData* m_pLUTTexture;
	ID3DX11EffectShaderResourceVariable* m_pLUTTextureVariable;
};
