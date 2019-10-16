#include "stdafx.h"
#include "BaseComponent.h"
#include "GameObject.h"

BaseComponent::BaseComponent() : m_pGameObject(nullptr), m_IsInitialized(false) {}

void BaseComponent::RootInitialize(const GameContext& gameContext)
{
	assert(!m_IsInitialized);
	if(m_IsInitialized)
		return;

	Initialize(gameContext);

	m_IsInitialized = true;
}

TransformComponent* BaseComponent::GetTransform() const
{
#if _DEBUG
	if(m_pGameObject == nullptr)
	{
		Logger::LogWarning(L"BaseComponent::GetTransform() > Failed to retrieve the TransformComponent. GameObject is NULL.");
		return nullptr;
	}
#endif

	return m_pGameObject->GetTransform();
}
