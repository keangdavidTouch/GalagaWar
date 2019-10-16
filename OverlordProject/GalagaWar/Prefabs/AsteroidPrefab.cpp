#include "stdafx.h"
#include "AsteroidPrefab.h"
#include "ModelComponent.h"
#include "TransformComponent.h"

void AsteroidPrefab::Initialize(const GameContext&)
{
	std::wstringstream nameStream{};
	nameStream << L"./Resources/Meshes/Asteroide" << m_Type << ".ovm";

	ModelComponent* asteroidModel = new ModelComponent(nameStream.str());
	asteroidModel->SetMaterial(19);
	AddComponent(asteroidModel);
}

void AsteroidPrefab::PostInitialize(const GameContext&)
{
	GetTransform()->Scale(m_Scale, m_Scale, m_Scale);
}
