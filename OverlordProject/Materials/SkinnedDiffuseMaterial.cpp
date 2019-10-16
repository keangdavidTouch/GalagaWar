#include "stdafx.h"
#include "SkinnedDiffuseMaterial.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "ModelComponent.h"
#include "ModelAnimator.h"

ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial::m_pBoneTransforms = nullptr;
ID3DX11EffectVectorVariable* SkinnedDiffuseMaterial::m_pTransparentColorVariable = nullptr;

SkinnedDiffuseMaterial::SkinnedDiffuseMaterial() : Material(L"./Resources/Effects/PosNormTex3D_Skinned.fx"),
	m_pDiffuseTexture(nullptr)
{}

void SkinnedDiffuseMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void SkinnedDiffuseMaterial::SetTransparentColor(DirectX::XMFLOAT4 color)
{
	m_TransparentColor = color;
}

void SkinnedDiffuseMaterial::LoadEffectVariables()
{
	if (!m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseSRVvariable->IsValid())
		{
			Logger::LogWarning(L"SkinnedDiffuseMaterial::LoadEffectVariables() > \'gDiffuseMap\' variable not found!");
			m_pDiffuseSRVvariable = nullptr;
		}
	}

	if (!m_pTransparentColorVariable)
	{
		m_pTransparentColorVariable = GetEffect()->GetVariableByName("gTransparentReplaceColor")->AsVector();
		if (!m_pTransparentColorVariable->IsValid())
		{
			Logger::LogWarning(L"SkinnedDiffuseMaterial::LoadEffectVariables() > \'gTransparentReplaceColor\' variable not found!");
			m_pTransparentColorVariable = nullptr;
		}
	}

	//TODO: Create a link to the gBones variable
	if (!m_pBoneTransforms)
	{
		m_pBoneTransforms = GetEffect()->GetVariableByName("gBones")->AsMatrix();
		if (!m_pBoneTransforms->IsValid())
		{
			Logger::LogWarning(L"SkinnedDiffuseMaterial::LoadEffectVariables() > \'m_pBoneTransforms\' variable not found!");
			m_pBoneTransforms = nullptr;
		}
	}
}

void SkinnedDiffuseMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);

	if (m_pTransparentColorVariable)
		m_pTransparentColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_TransparentColor));

	if (m_pDiffuseTexture && m_pDiffuseSRVvariable)
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

	//TODO: Set the matrix array (BoneTransforms of the ModelAnimator)}
	/*if (m_pBoneTransforms)
	{
		m_pBoneTransforms->SetMatrix(reinterpret_cast<float*>(&pModelComponent->GetAnimator()->GetBoneTransforms()[0]));
	}*/

	m_pBoneTransforms->SetMatrixArray((const float*)&pModelComponent->GetAnimator()->GetBoneTransforms().front(), 0,
		pModelComponent->GetAnimator()->GetBoneTransforms().size());

}