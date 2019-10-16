#pragma once
class TextureData
{
public:
	TextureData(ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureShaderResourceView);
	~TextureData(void);

	ID3D11Resource* GetResource() const { return m_pTexture; }
	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pTextureShaderResourceView; }
 DirectX::XMFLOAT2 GetDimension() const { return m_Dimension; }

private:

	ID3D11Resource *m_pTexture;
	ID3D11ShaderResourceView *m_pTextureShaderResourceView;
	DirectX::XMFLOAT2 m_Dimension;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	TextureData(const TextureData& t);
	TextureData& operator=(const TextureData& t);
};

