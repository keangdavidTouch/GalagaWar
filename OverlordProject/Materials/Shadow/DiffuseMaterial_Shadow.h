#pragma once
#include "Material.h"

class TextureData;

class DiffuseMaterial_Shadow final : public Material
{
public:
	DiffuseMaterial_Shadow();
	~DiffuseMaterial_Shadow() = default;

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetLightDirection(DirectX::XMFLOAT3 dir);

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	TextureData* m_pDiffuseTexture;
	DirectX::XMFLOAT3 m_LightDirection = { -0.577f, -0.577f, 0.577f };

	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	static ID3DX11EffectShaderResourceVariable* m_pShadowSRVvariable;
	static ID3DX11EffectVectorVariable* m_pLightDirectionVariable;
	static ID3DX11EffectMatrixVariable* m_pLightWVPvariable;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	DiffuseMaterial_Shadow(const DiffuseMaterial_Shadow &obj);
	DiffuseMaterial_Shadow& operator=(const DiffuseMaterial_Shadow& obj);
};

