#pragma once
#include "ContentLoader.h"

class PxConvexMeshLoader : public ContentLoader<physx::PxConvexMesh>
{
public:
	PxConvexMeshLoader() = default;
	virtual ~PxConvexMeshLoader() = default;

	PxConvexMeshLoader(const PxConvexMeshLoader& other) = delete;
	PxConvexMeshLoader(PxConvexMeshLoader&& other) noexcept = delete;
	PxConvexMeshLoader& operator=(const PxConvexMeshLoader& other) = delete;
	PxConvexMeshLoader& operator=(PxConvexMeshLoader&& other) noexcept = delete;
protected:

	physx::PxConvexMesh* LoadContent(const std::wstring& assetFile) override;
	void Destroy(physx::PxConvexMesh* objToDestroy) override;
};

