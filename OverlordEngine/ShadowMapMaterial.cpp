//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ShadowMapMaterial.h"
#include "ContentManager.h"

ShadowMapMaterial::~ShadowMapMaterial()
{
	//TODO: make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
	SafeRelease(m_pInputLayouts[0]);
	SafeRelease(m_pInputLayouts[1]);
	m_InputLayoutDescriptions->clear();
}

void ShadowMapMaterial::Initialize(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		//TODO: initialize the effect, techniques, shader variables, input layouts (hint use EffectHelper::BuildInputLayout), etc.
		// Load Effect
		m_pShadowEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ShadowMapGenerator.fx");
		// Load techniques
		m_pShadowTechs[0] = m_pShadowEffect->GetTechniqueByIndex(0);
		m_pShadowTechs[1] = m_pShadowEffect->GetTechniqueByIndex(1);
		// Build InputLayout
		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pShadowTechs[0], &m_pInputLayouts[0], m_InputLayoutDescriptions[0], m_InputLayoutSizes[0], m_InputLayoutIds[0]);
		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pShadowTechs[1], &m_pInputLayouts[1], m_InputLayoutDescriptions[1], m_InputLayoutSizes[1], m_InputLayoutIds[1]);
		// Shader Variable
		auto effectVar = m_pShadowEffect->GetVariableByName("gWorld");
		m_pWorldMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
		effectVar = m_pShadowEffect->GetVariableByName("gLightViewProj");
		m_pLightVPMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
		effectVar = m_pShadowEffect->GetVariableByName("gBones");
		m_pBoneTransforms = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	}
}

void ShadowMapMaterial::SetLightVP(DirectX::XMFLOAT4X4 lightVP) const
{
	//TODO: set the correct shader variable
	m_pLightVPMatrixVariable->SetMatrix(reinterpret_cast<float*>(&lightVP));
}

void ShadowMapMaterial::SetWorld(DirectX::XMFLOAT4X4 world) const
{
	//TODO: set the correct shader variable
	m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<float*>(&world));
}

void ShadowMapMaterial::SetBones(const float* pData, int count) const
{
	//TODO: set the correct shader variable
	m_pBoneTransforms->SetMatrixArray(pData, 0, count);
}
