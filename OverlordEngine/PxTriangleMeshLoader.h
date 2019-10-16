#pragma once
#include "ContentLoader.h"

class PxTriangleMeshLoader : public ContentLoader<physx::PxTriangleMesh>
{
public:
	PxTriangleMeshLoader() = default;
	virtual ~PxTriangleMeshLoader() = default;

	PxTriangleMeshLoader(const PxTriangleMeshLoader& other) = delete;
	PxTriangleMeshLoader(PxTriangleMeshLoader&& other) noexcept = delete;
	PxTriangleMeshLoader& operator=(const PxTriangleMeshLoader& other) = delete;
	PxTriangleMeshLoader& operator=(PxTriangleMeshLoader&& other) noexcept = delete;
protected:
	physx::PxTriangleMesh* LoadContent(const std::wstring& assetFile) override;
	void Destroy(physx::PxTriangleMesh* objToDestroy) override;
};

