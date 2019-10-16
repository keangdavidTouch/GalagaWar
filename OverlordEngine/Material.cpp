#include "stdafx.h"
#include "Material.h"
#include "ContentManager.h"
#include "CameraComponent.h"
#include "ModelComponent.h"
#include "TransformComponent.h"

Material::Material(std::wstring effectFile, std::wstring technique) :
	m_effectFile(std::move(effectFile)), 
	m_pEffect(nullptr),
	m_TechniqueName(std::move(technique)),
	m_pTechnique(nullptr),
	m_pWorldMatrixVariable(nullptr),
	m_pViewMatrixVariable(nullptr),
	m_pViewInverseMatrixVariable(nullptr),
	m_pWvpMatrixVariable(nullptr),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_InputLayoutID(0),
	m_IsInitialized(false)
{
}

Material::~Material()
{
	SafeRelease(m_pInputLayout);
	m_pInputLayoutDescriptions.clear();
}

void Material::Initialize(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		auto pos = m_effectFile.rfind('.', m_effectFile.length());
		if(pos != std::string::npos)
		{
			std::wstring const extension = m_effectFile.substr(pos + 1, m_effectFile.length() - pos);
			if (extension == L"fx")
			{
				LoadEffect(gameContext);
				m_IsInitialized = true;
			}
			else if(extension == L"fxc")
			{
				Logger::LogInfo(L"Loading precompiled shader");
				LoadCompiledEffect(gameContext);
				m_IsInitialized = true;
			}
		}
	}
}

bool Material::LoadEffect(const GameContext& gameContext)
{
	//Load Effect
	m_pEffect = ContentManager::Load<ID3DX11Effect>(m_effectFile);

	if (!m_TechniqueName.empty())
	{
		auto techString = std::string(m_TechniqueName.begin(), m_TechniqueName.end());
		m_pTechnique = m_pEffect->GetTechniqueByName(techString.c_str());
	}
	else
	{
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	}

	//Build InputLayout
	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, m_pInputLayoutDescriptions,
	                               m_pInputLayoutSize, m_InputLayoutID);

	auto effectVar = m_pEffect->GetVariableBySemantic("World");
	m_pWorldMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	effectVar = m_pEffect->GetVariableBySemantic("View");
	m_pViewMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	effectVar = m_pEffect->GetVariableBySemantic("ViewInverse");
	m_pViewInverseMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	effectVar = m_pEffect->GetVariableBySemantic("WorldViewProjection");
	m_pWvpMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;

	LoadEffectVariables();

	return true;
}

bool Material::LoadCompiledEffect(const GameContext& gameContext)
{
	ID3DX11Effect* pEffect;
	D3DX11CreateEffectFromFile(m_effectFile.c_str(),
		0, gameContext.pDevice, &pEffect);
	return pEffect;
}

void Material::SetEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	if (m_IsInitialized)
	{
		auto world = XMLoadFloat4x4(&pModelComponent->GetTransform()->GetWorld());
		auto view = XMLoadFloat4x4(&gameContext.pCamera->GetView());
		const auto projection = XMLoadFloat4x4(&gameContext.pCamera->GetProjection());

		if (m_pWorldMatrixVariable)
			m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<float*>(&world));

		if (m_pViewMatrixVariable)
			m_pViewMatrixVariable->SetMatrix(reinterpret_cast<float*>(&view));

		if (m_pWvpMatrixVariable)
		{
			auto wvp = world * view * projection;
			m_pWvpMatrixVariable->SetMatrix(reinterpret_cast<const float*>(&(wvp)));
		}

		if (m_pViewInverseMatrixVariable)
		{
			auto viewInv = XMLoadFloat4x4(&gameContext.pCamera->GetViewInverse());
			m_pViewInverseMatrixVariable->SetMatrix(reinterpret_cast<float*>(&viewInv));
		}

		UpdateEffectVariables(gameContext, pModelComponent);
	}
}
