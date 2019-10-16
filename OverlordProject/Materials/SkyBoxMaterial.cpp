#include "stdafx.h"
#include "SkyBoxMaterial.h"
#include "TextureData.h"
#include "ContentManager.h"

ID3DX11EffectShaderResourceVariable* SkyBoxMaterial::m_pSkyBoxSRVvariable = nullptr;

SkyBoxMaterial::SkyBoxMaterial() 
	: Material(L"./Resources/Effects/SkyBox.fx")
	, m_pSkyBoxTexture(ContentManager::Load<TextureData>(L"./Resources/Textures/SkyBox.dds"))
{
}

SkyBoxMaterial::~SkyBoxMaterial()
{
}

void SkyBoxMaterial::SetSkyBox(const std::wstring& assetFile) 
{
	m_pSkyBoxTexture = ContentManager::Load<TextureData>(assetFile);
}

void SkyBoxMaterial::LoadEffectVariables()
{
	m_pSkyBoxSRVvariable = GetEffect()->GetVariableByName("m_CubeMap")->AsShaderResource();
}

void SkyBoxMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	m_pSkyBoxSRVvariable->SetResource(m_pSkyBoxTexture->GetShaderResourceView());
}