#include "stdafx.h"
#include "PxTriangleMeshLoader.h"
#include "PhysxManager.h"

physx::PxTriangleMesh* PxTriangleMeshLoader::LoadContent(const std::wstring& assetFile)
{
	std::string buffer =std::string(assetFile.begin(), assetFile.end());
	auto inputStream  = physx::PxDefaultFileInputData(buffer.c_str());
	return PhysxManager::GetInstance()->GetPhysics()->createTriangleMesh(inputStream);
}

void PxTriangleMeshLoader::Destroy(physx::PxTriangleMesh*) {}
