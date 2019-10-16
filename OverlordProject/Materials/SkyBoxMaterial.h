#pragma once
#include "Material.h"

class TextureData;

class SkyBoxMaterial : public Material
{
public:
	SkyBoxMaterial();
	~SkyBoxMaterial();

	void SetSkyBox(const std::wstring& assetFile);

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);

private:
	TextureData* m_pSkyBoxTexture;
	static ID3DX11EffectShaderResourceVariable* m_pSkyBoxSRVvariable;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SkyBoxMaterial(const SkyBoxMaterial &obj);
	SkyBoxMaterial& operator=(const SkyBoxMaterial& obj);
};

