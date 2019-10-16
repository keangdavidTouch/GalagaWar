#pragma once
#include "Material.h"
#include "ContentManager.h"
#include "TextureData.h"

class UberMaterial final : public Material
{
public:
	UberMaterial();
	~UberMaterial() = default;

	//LIGHT
	//*****
	void UberMaterial::SetLightDirection(DirectX::XMFLOAT3 direction) { m_LightDirection = direction; }
	
	//DIFFUSE
	//*******
	void UberMaterial::EnableDiffuseTexture(bool enable) { m_bDiffuseTexture = enable; }
	void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile) { m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile); }
	void UberMaterial::SetDiffuseColor(DirectX::XMFLOAT4 color) { m_ColorDiffuse = color; }
	
	//SPECULAR
	//********
	void UberMaterial::SetSpecularColor(DirectX::XMFLOAT4 color) { m_ColorSpecular = color; }
	void UberMaterial::EnableSpecularLevelTexture(bool enable) { m_bSpecularLevelTexture = enable; }
	void UberMaterial::SetSpecularLevelTexture(const std::wstring& assetFile) { m_pSpecularLevelTexture = ContentManager::Load<TextureData>(assetFile); }
	void UberMaterial::SetShininess(int shininess) { m_Shininess = shininess; }
	
	//AMBIENT
	//*******
	void UberMaterial::SetAmbientColor(DirectX::XMFLOAT4 color) { m_ColorAmbient = color; }
	void UberMaterial::SetAmbientIntensity(float intensity) { m_AmbientIntensity = intensity; }


	//NORMAL MAPPING
	//**************
	void UberMaterial::FlipNormalGreenCHannel(bool flip) { m_bFlipGreenChannel = flip; } 
	void UberMaterial::EnableNormalMapping(bool enable) { m_bNormalMapping = enable; }
	void UberMaterial::SetNormalMapTexture(const std::wstring& assetFile) { m_pNormalMappingTexture = ContentManager::Load<TextureData>(assetFile); }
	
	//ENVIRONMENT MAPPING
	//*******************
	void UberMaterial::EnableEnvironmentMapping(bool enable) { m_bEnvironmentMapping = enable; }
	void UberMaterial::SetEnvironmentCube(const std::wstring& assetFile) { m_pEnvironmentCube = ContentManager::Load<TextureData>(assetFile); }
	void UberMaterial::SetReflectionStrength(float strength) { m_ReflectionStrength = strength; }
	void UberMaterial::SetRefractionStrength(float strength) { m_RefractionStrength = strength; }
	void UberMaterial::SetRefractionIndex(float index) { m_RefractionIndex = index; }

	//OPACITY
	//*******
	void UberMaterial::SetOpacity(float opacity) { m_Opacity = opacity; }
	void UberMaterial::EnableOpacityMap(bool enable) { m_bOpacityMap = enable; }
	void UberMaterial::SetOpacityTexture(const std::wstring& assetFile) { m_pOpacityMap = ContentManager::Load<TextureData>(assetFile); }

	//SPECULAR MODELS
	//***************
	void UberMaterial::EnableSpecularBlinn(bool enable) { m_bSpecularBlinn = enable; }
	void UberMaterial::EnableSpecularPhong(bool enable) { m_bSpecularPhong = enable; }

	//FRESNEL FALLOFF
	//***************
	void EnableFresnelFaloff(bool enable) { m_bFresnelFaloff = enable; }
	void SetFresnelColor(DirectX::XMFLOAT4 color) { m_ColorFresnel = color; }
	void SetFresnelPower(float power) { m_FresnelPower = power; }
	void SetFresnelMultiplier(float multiplier) { m_FresnelMultiplier = multiplier; }
	void SetFresnelHardness(float hardness) { m_FresnelHardness = hardness; }

	//ROUGHNESS
	//***************
	void EnableRougness(bool enable) { m_bRoughness = enable; }
	void SetRougnessMapTexture(const std::wstring& assetFile) { m_pRoughnessMappingTexture = ContentManager::Load<TextureData>(assetFile); };

	//METALLIC
	//***************
	void EnableMetallic(bool enable) { m_bMetallic = enable; }
	void SetMetallicMapTexture(const std::wstring& assetFile) { m_pMetallicMappingTexture = ContentManager::Load<TextureData>(assetFile); };

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);

private:
	//LIGHT
	//*****
	DirectX::XMFLOAT3 m_LightDirection;
	static ID3DX11EffectVectorVariable* m_pLightDirectionVariable;

	//DIFFUSE
	//*******
	bool m_bDiffuseTexture;
	static ID3DX11EffectScalarVariable* m_pUseDiffuseTextureVariable;
	TextureData* m_pDiffuseTexture;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	DirectX::XMFLOAT4 m_ColorDiffuse;
	static ID3DX11EffectVectorVariable* m_pDiffuseColorVariable;

	//SPECULAR
	//********
	DirectX::XMFLOAT4 m_ColorSpecular;
	static ID3DX11EffectVectorVariable* m_pSpecularColorVariable;
	bool m_bSpecularLevelTexture;
	static ID3DX11EffectScalarVariable* m_pUseSpecularLevelTextureVariable;
	TextureData* m_pSpecularLevelTexture;
	static ID3DX11EffectShaderResourceVariable* m_pSpecularLevelSRVvariable;
	int m_Shininess;
	static ID3DX11EffectScalarVariable* m_pShininessVariable;

	//AMBIENT
	//*******
	DirectX::XMFLOAT4 m_ColorAmbient;
	static ID3DX11EffectVectorVariable* m_pAmbientColorVariable;
	float m_AmbientIntensity;
	static ID3DX11EffectScalarVariable* m_pAmbientIntensityVariable;

	//NORMAL MAPPING
	//**************
	bool m_bFlipGreenChannel;
	static ID3DX11EffectScalarVariable* m_pFlipGreenChannelVariable;
	bool m_bNormalMapping;
	static ID3DX11EffectScalarVariable* m_pUseNormalMappingVariable;
	TextureData* m_pNormalMappingTexture;
	static ID3DX11EffectShaderResourceVariable* m_pNormalMappingSRVvariable;
	
	//ENVIRONMENT MAPPING
	//*******************
	bool m_bEnvironmentMapping;
	static ID3DX11EffectScalarVariable* m_pUseEnvironmentMappingVariable;
	TextureData* m_pEnvironmentCube;
	static ID3DX11EffectShaderResourceVariable* m_pEnvironmentSRVvariable;
	float m_ReflectionStrength;
	static ID3DX11EffectScalarVariable* m_pReflectionStrengthVariable;
	float m_RefractionStrength;
	static ID3DX11EffectScalarVariable* m_pRefractionStrengthVariable;
	float m_RefractionIndex;
	static ID3DX11EffectScalarVariable* m_pRefractionIndexVariable;

	//OPACITY
	//***************
	float m_Opacity;
	static ID3DX11EffectScalarVariable* m_pOpacityVariable;
	bool m_bOpacityMap;
	static ID3DX11EffectScalarVariable* m_pUseOpacityMapVariable;
	TextureData* m_pOpacityMap;
	static ID3DX11EffectShaderResourceVariable* m_pOpacitySRVvariable;

	//SPECULAR MODELS
	//***************
	bool m_bSpecularBlinn;
	static ID3DX11EffectScalarVariable* m_pUseBlinnVariable;
	bool m_bSpecularPhong;
	static ID3DX11EffectScalarVariable* m_pUsePhongVariable;

	//FRESNEL FALLOFF
	//***************
	bool m_bFresnelFaloff;
	static ID3DX11EffectScalarVariable* m_pUseFresnelFalloffVariable;
	DirectX::XMFLOAT4 m_ColorFresnel;
	static ID3DX11EffectVectorVariable* m_pFresnelColorVariable;
	float m_FresnelPower;
	static ID3DX11EffectScalarVariable* m_pFresnelPowerVariable;
	float m_FresnelMultiplier;
	static ID3DX11EffectScalarVariable* m_pFresnelMultiplierVariable;
	float m_FresnelHardness;
	static ID3DX11EffectScalarVariable* m_pFresnelHardnessVariable;

	//ROUGNESS MAPPING
	//**************
	bool m_bRoughness;
	static ID3DX11EffectScalarVariable* m_pUseRoughnessVariable;
	TextureData* m_pRoughnessMappingTexture;
	static ID3DX11EffectShaderResourceVariable* m_pRoughnessMappingSRVvariable;

	//METALLIC MAPPING
	//**************
	bool m_bMetallic;
	static ID3DX11EffectScalarVariable* m_pUseMetallicVariable;
	TextureData* m_pMetallicMappingTexture;
	static ID3DX11EffectShaderResourceVariable* m_pMetallicMappingSRVvariable;
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	UberMaterial(const UberMaterial &obj);
	UberMaterial& operator=(const UberMaterial& obj);
};

