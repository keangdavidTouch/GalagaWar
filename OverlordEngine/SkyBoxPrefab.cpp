#include "stdafx.h"
#include "SkyBoxPrefab.h"
#include "../OverlordProject/Materials/SkyBoxMaterial.h"
#include "ModelComponent.h"
#include "TransformComponent.h"

SkyBoxPrefab::SkyBoxPrefab(const std::wstring& assetFile)
	: m_SkyBoxMat(nullptr)
	, m_AssetPath(assetFile)
{
}

void SkyBoxPrefab::Initialize(const GameContext & gameContext)
{
	m_SkyBoxMat = new SkyBoxMaterial();
	m_SkyBoxMat->SetSkyBox(m_AssetPath);
	gameContext.pMaterialManager->AddMaterial(m_SkyBoxMat, 5);
	
	auto model = new ModelComponent(L"./Resources/Meshes/Box.ovm");
	model->SetMaterial(5);

	m_Box = new GameObject();
	m_Box->AddComponent(model);
	AddChild(m_Box);
}

void SkyBoxPrefab::Update(const GameContext& )
{
}

void SkyBoxPrefab::SetSkyBox(const std::wstring& assetFile)
{
	m_SkyBoxMat->SetSkyBox(assetFile);
}
