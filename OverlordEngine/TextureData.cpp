#include "stdafx.h"
#include "TextureData.h"

TextureData::TextureData(ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureShaderResourceView):
	m_pTexture(pTexture),
	m_pTextureShaderResourceView(pTextureShaderResourceView)
{
	auto tex2D = static_cast<ID3D11Texture2D*>(pTexture);
	D3D11_TEXTURE2D_DESC text2Ddesc;
	tex2D->GetDesc(&text2Ddesc);

	m_Dimension = DirectX::XMFLOAT2((float)text2Ddesc.Width, (float)text2Ddesc.Height);
}


TextureData::~TextureData()
{
	SafeRelease(m_pTexture);
	SafeRelease(m_pTextureShaderResourceView);
}
