#include "stdafx.h"
#include "MaterialManager.h"
#include "Material.h"

MaterialManager::MaterialManager():
	m_pMaterials(std::map<UINT, Material*>())
{
}

MaterialManager::~MaterialManager()
{
	//Delete Materials
	for (std::pair<UINT, Material*> materialEntry : m_pMaterials)
	{
		SafeDelete(materialEntry.second);
	}

	m_pMaterials.clear();
}

Material* MaterialManager::GetMaterial(UINT materialId) const
{
	const auto it = m_pMaterials.find(materialId);
	if (it == m_pMaterials.end())
	{
		Logger::LogFormat(LogLevel::Error, L"MaterialManager::GetMaterial() > Material with ID %i not found!", materialId);
		return nullptr;
	}
	
	return it->second;
}

void MaterialManager::RemoveMaterial(UINT materialId)
{
	auto it = m_pMaterials.find(materialId);
	if (it == m_pMaterials.end())
	{
		Logger::LogFormat(LogLevel::Warning, L"MaterialManager::RemoveMaterial() > Material with ID %i not found!", materialId);
		return;
	}

	SafeDelete(it->second);
	m_pMaterials.erase(it);
}

void MaterialManager::AddMaterial(Material *pMaterial, UINT materialId)
{
#if _DEBUG
	if (pMaterial == nullptr)
	{
		Logger::LogError(L"MaterialManager::AddMaterial() > Material is NULL!");
		return;
	}

	for (std::pair<UINT, Material*> materialEntry : m_pMaterials)
	{
		if (materialEntry.first == materialId)
		{
			Logger::LogFormat(LogLevel::Warning, L"MaterialManager::AddMaterial() > MaterialID %i is already used!", materialId);
			return;
		}

		if (materialEntry.second == pMaterial)
		{
			Logger::LogFormat(LogLevel::Warning, L"MaterialManager::AddMaterial() > Material is already added to the manager (ID %i)!", materialId);
			return;
		}
	}
#endif

	m_pMaterials[materialId] = pMaterial;
}
