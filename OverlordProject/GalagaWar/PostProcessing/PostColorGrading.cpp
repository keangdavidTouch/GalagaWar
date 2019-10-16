#include "stdafx.h"
#include "PostColorGrading.h"
#include "RenderTarget.h"

PostColorGrading::PostColorGrading()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Lut.fx", 1),
	m_pTextureMapVariable(nullptr)
{
}

void PostColorGrading::LoadEffectVariables()
{
	auto effectVar = GetEffect()->GetVariableByName("gTexture");
	m_pTextureMapVariable = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;
}

void PostColorGrading::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	if(m_pTextureMapVariable)
		m_pTextureMapVariable->SetResource(pRendertarget->GetShaderResourceView());
}

