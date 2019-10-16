#include "stdafx.h"
#include "PxConvexMeshLoader.h"
#include "PhysxManager.h"

physx::PxConvexMesh* PxConvexMeshLoader::LoadContent(const std::wstring& assetFile)
{
	std::string buffer = std::string(assetFile.begin(), assetFile.end());
	auto inputStream  = physx::PxDefaultFileInputData(buffer.c_str());
	return PhysxManager::GetInstance()->GetPhysics()->createConvexMesh(inputStream);
}

void PxConvexMeshLoader::Destroy(physx::PxConvexMesh* ) {}
