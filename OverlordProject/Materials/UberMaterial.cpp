#include "stdafx.h"
#include "UberMaterial.h"

ID3DX11EffectVectorVariable* UberMaterial::m_pLightDirectionVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseDiffuseTextureVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectVectorVariable* UberMaterial::m_pDiffuseColorVariable = nullptr;

ID3DX11EffectVectorVariable* UberMaterial::m_pSpecularColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseSpecularLevelTextureVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial:: m_pSpecularLevelSRVvariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pShininessVariable = nullptr;
ID3DX11EffectVectorVariable* UberMaterial::m_pAmbientColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pAmbientIntensityVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pFlipGreenChannelVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseNormalMappingVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pNormalMappingSRVvariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial:: m_pUseEnvironmentMappingVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pEnvironmentSRVvariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pReflectionStrengthVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pRefractionStrengthVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pRefractionIndexVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pOpacityVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseOpacityMapVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pOpacitySRVvariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseBlinnVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUsePhongVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseFresnelFalloffVariable = nullptr;
ID3DX11EffectVectorVariable* UberMaterial::m_pFresnelColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelPowerVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelMultiplierVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelHardnessVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseRoughnessVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pRoughnessMappingSRVvariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseMetallicVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pMetallicMappingSRVvariable = nullptr;

UberMaterial::UberMaterial() : Material(L"./Resources/Effects/UberShader.fx")
	, m_LightDirection{ DirectX::XMFLOAT3(0.577f, 0.577f, 0.577f) }
	, m_bDiffuseTexture{ true }
	, m_pDiffuseTexture{ ContentManager::Load<TextureData>(L"./Resources/Textures/CobbleStone_DiffuseMap.dds") }
	, m_ColorDiffuse{ DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f) }
	, m_ColorSpecular{ DirectX::XMFLOAT4(1, 1.f, 1.f, 1.f) }
	, m_bSpecularLevelTexture{ false }
	, m_pSpecularLevelTexture{ nullptr }
	, m_Shininess{ 15 }
	, m_ColorAmbient{ DirectX::XMFLOAT4(0.f , 0.f, 0.f, 1.f) }
	, m_AmbientIntensity{ 0.f }
	, m_bFlipGreenChannel{ false }
	, m_bNormalMapping{ false }
	, m_pNormalMappingTexture{ nullptr }
	, m_bEnvironmentMapping{ false }
	, m_pEnvironmentCube{ nullptr }
	, m_ReflectionStrength{ 0.f }
	, m_RefractionStrength{ 0.f }
	, m_RefractionIndex{ 0.3f }
	, m_Opacity{ 1.f }
	, m_bOpacityMap{ false }
	, m_pOpacityMap{ nullptr }
	, m_bSpecularBlinn{ false }
	, m_bSpecularPhong{ true }
	, m_bFresnelFaloff{ true }
	, m_ColorFresnel{ DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f) }
	, m_FresnelPower{ 1.f }
	, m_FresnelMultiplier{ 1.f }
	, m_FresnelHardness{ 0 }
	, m_bMetallic(false)
	, m_bRoughness(false)
	, m_pRoughnessMappingTexture(nullptr)
	, m_pMetallicMappingTexture(nullptr)
	
{ }


void UberMaterial::LoadEffectVariables()
{
	//LIGHT
	//*****
	m_pLightDirectionVariable = GetEffect()->GetVariableByName("gLightDirection")->AsVector();
	//DIFFUSE
	//*******
	m_pUseDiffuseTextureVariable = GetEffect()->GetVariableByName("gUseTextureDiffuse")->AsScalar();
	m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gTextureDiffuse")->AsShaderResource();
	m_pDiffuseColorVariable = GetEffect()->GetVariableByName("gColorDiffuse")->AsVector();
	////SPECULAR
	////********
	m_pSpecularColorVariable = GetEffect()->GetVariableByName("gColorSpecular")->AsVector();
	m_pSpecularLevelSRVvariable = GetEffect()->GetVariableByName("gTextureSpecularIntensity")->AsShaderResource();
	m_pUseSpecularLevelTextureVariable = GetEffect()->GetVariableByName("gUseTextureSpecularIntensity")->AsScalar();
	m_pShininessVariable = GetEffect()->GetVariableByName("gShininess")->AsScalar();
	////AMBIENT
	////*******
	m_pAmbientColorVariable = GetEffect()->GetVariableByName("gColorAmbient")->AsVector();
	m_pAmbientIntensityVariable = GetEffect()->GetVariableByName("gAmbientIntensity")->AsScalar();
	////NORMAL MAPPING
	////**************
	m_pFlipGreenChannelVariable = GetEffect()->GetVariableByName("gFlipGreenChannel")->AsScalar();
	m_pUseNormalMappingVariable = GetEffect()->GetVariableByName("gUseTextureNormal")->AsScalar();
	m_pNormalMappingSRVvariable = GetEffect()->GetVariableByName("gTextureNormal")->AsShaderResource();
	////ENVIRONMENT MAPPING
	////*******************
	m_pUseEnvironmentMappingVariable = GetEffect()->GetVariableByName("gUseTextureEnvironment")->AsScalar();
	m_pReflectionStrengthVariable = GetEffect()->GetVariableByName("gReflectionStrength")->AsScalar();
	m_pRefractionStrengthVariable = GetEffect()->GetVariableByName("gRefractionStrength")->AsScalar();
	m_pRefractionIndexVariable = GetEffect()->GetVariableByName("gRefractionIndex")->AsScalar();
	m_pEnvironmentSRVvariable = GetEffect()->GetVariableByName("gCubeEnvironment")->AsShaderResource();
	////OPACITY
	////***************
	m_pOpacityVariable = GetEffect()->GetVariableByName("gOpacityIntensity")->AsScalar();
	m_pUseOpacityMapVariable = GetEffect()->GetVariableByName("gUseTextureOpacity")->AsScalar();
	m_pOpacitySRVvariable = GetEffect()->GetVariableByName("gTextureOpacity")->AsShaderResource();
	////SPECULAR MODELS
	////***************
	m_pUseBlinnVariable = GetEffect()->GetVariableByName("gUseSpecularBlinn")->AsScalar();
	m_pUsePhongVariable = GetEffect()->GetVariableByName("gUseSpecularPhong")->AsScalar();
	////FRESNEL FALLOFF
	////***************
	m_pUseFresnelFalloffVariable = GetEffect()->GetVariableByName("gUseFresnelFalloff")->AsScalar();
	m_pFresnelColorVariable = GetEffect()->GetVariableByName("gColorFresnel")->AsVector();
	m_pFresnelPowerVariable = GetEffect()->GetVariableByName("gFresnelPower")->AsScalar();
	m_pFresnelMultiplierVariable = GetEffect()->GetVariableByName("gFresnelMultiplier")->AsScalar();
	m_pFresnelHardnessVariable = GetEffect()->GetVariableByName("gFresnelHardness")->AsScalar();
	////ROUGNESS 
	////***************
	m_pUseRoughnessVariable = GetEffect()->GetVariableByName("gUseRoughness")->AsScalar();
	m_pRoughnessMappingSRVvariable = GetEffect()->GetVariableByName("gTextureRoughness")->AsShaderResource();
	////METALLIC 
	////***************
	m_pUseMetallicVariable = GetEffect()->GetVariableByName("gUseMetallic")->AsScalar();
	m_pMetallicMappingSRVvariable = GetEffect()->GetVariableByName("gTextureMetallic")->AsShaderResource();

}

void UberMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	//LIGHT
	//*****
	m_pUseDiffuseTextureVariable->SetBool(m_bDiffuseTexture);
	if (m_pDiffuseTexture) m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	m_pDiffuseColorVariable->SetRawValue(&m_ColorDiffuse, 0, sizeof(m_ColorDiffuse));
	//DIFFUSE
	//*******
	m_pSpecularColorVariable->SetRawValue(&m_ColorSpecular, 0, sizeof(m_ColorSpecular));
	if (m_pSpecularLevelTexture) m_pSpecularLevelSRVvariable->SetResource(m_pSpecularLevelTexture->GetShaderResourceView());
	m_pUseSpecularLevelTextureVariable->SetBool(m_pUseSpecularLevelTextureVariable);
	m_pShininessVariable->SetInt(m_Shininess);
	////AMBIENT
	////*******
	m_pAmbientColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_ColorAmbient));
	m_pAmbientIntensityVariable->SetFloat(m_AmbientIntensity); 
	////NORMAL MAPPING
	////**************
	m_pFlipGreenChannelVariable->SetBool(m_bFlipGreenChannel);
	m_pUseNormalMappingVariable->SetFloat(m_bNormalMapping);
	if (m_pNormalMappingTexture) m_pNormalMappingSRVvariable->SetResource(m_pNormalMappingTexture->GetShaderResourceView());
	////ENVIRONMENT MAPPING
	////*******************
	m_pUseEnvironmentMappingVariable->SetFloat(m_bEnvironmentMapping);
	m_pReflectionStrengthVariable->SetFloat(m_ReflectionStrength);
	m_pRefractionStrengthVariable->SetFloat(m_RefractionStrength);
	m_pRefractionIndexVariable->SetFloat(m_RefractionIndex);
	if (m_pEnvironmentCube) m_pEnvironmentSRVvariable->SetResource(m_pEnvironmentCube->GetShaderResourceView());
	////OPACITY
	////***************
	m_pOpacityVariable->SetFloat(m_Opacity);
	m_pUseOpacityMapVariable->SetBool(m_bOpacityMap);
	if (m_pOpacityMap) m_pOpacitySRVvariable->SetResource(m_pOpacityMap->GetShaderResourceView());
	////SPECULAR MODELS
	////***************
	m_pUseBlinnVariable->SetFloat(m_bSpecularBlinn);
	m_pUsePhongVariable->SetFloat(m_bSpecularPhong);
	////FRESNEL FALLOFF
	////***************
	m_pUseFresnelFalloffVariable->SetBool(m_bFresnelFaloff);
	m_pFresnelColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_ColorFresnel)); 
	m_pFresnelPowerVariable->SetFloat(m_FresnelPower); 
	m_pFresnelMultiplierVariable->SetFloat(m_FresnelMultiplier); 
	m_pFresnelHardnessVariable->SetFloat(m_FresnelHardness);
	////ROUGNESS 
	////***************
	m_pUseRoughnessVariable->SetBool(m_bRoughness);
	if (m_pRoughnessMappingTexture) m_pRoughnessMappingSRVvariable->SetResource(m_pRoughnessMappingTexture->GetShaderResourceView()); 
	////METALLIC 
	////***************
	m_pUseMetallicVariable->SetBool(m_bMetallic);
	if (m_pMetallicMappingTexture) m_pMetallicMappingSRVvariable->SetResource(m_pMetallicMappingTexture->GetShaderResourceView());

}
