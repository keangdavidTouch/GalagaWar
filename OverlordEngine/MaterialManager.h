#pragma once
#include <map>

class Material;

class MaterialManager final
{
public:
	MaterialManager(const MaterialManager& other) = delete;
	MaterialManager(MaterialManager&& other) noexcept = delete;
	MaterialManager& operator=(const MaterialManager& other) = delete;
	MaterialManager& operator=(MaterialManager&& other) noexcept = delete;
	MaterialManager();
	~MaterialManager();

	void AddMaterial(Material *pMaterial, UINT materialId);
	void RemoveMaterial(UINT materialId);
	Material* GetMaterial(UINT materialId) const;

private:
	std::map<UINT, Material*> m_pMaterials;
};

