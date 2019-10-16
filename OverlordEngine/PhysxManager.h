#pragma once
#include "Singleton.h"

class PhysxAllocator;
class PhysxErrorCallback;
class GameScene;
class OverlordGame;

class PhysxManager final : public Singleton<PhysxManager>, public physx::PxVisualDebuggerConnectionHandler
{
public:
	friend class Singleton<PhysxManager>;

	void Init(void* pDevice);

	physx::PxPhysics* GetPhysics() const { return m_pPhysics; }
	physx::PxScene* CreateScene(GameScene* pScene) const;

	void onPvdSendClassDescriptions(physx::PxVisualDebuggerConnection& connection) override;
	void onPvdConnected(physx::PxVisualDebuggerConnection& connection) override;
	void onPvdDisconnected(physx::PxVisualDebuggerConnection& connection) override;

	bool ToggleVisualDebuggerConnection() const;

	PhysxManager(const PhysxManager& other) = delete;
	PhysxManager(PhysxManager&& other) noexcept = delete;
	PhysxManager& operator=(const PhysxManager& other) = delete;
	PhysxManager& operator=(PhysxManager&& other) noexcept = delete;

private:
	PhysxManager();
	virtual ~PhysxManager();

	void CleanUp();

	PhysxAllocator* m_pDefaultAllocator;
	PhysxErrorCallback* m_pDefaultErrorCallback;
	physx::PxFoundation* m_pFoundation;
	physx::PxProfileZoneManager* m_pProfileZoneManager;
	physx::PxPhysics* m_pPhysics;
	physx::PxDefaultCpuDispatcher* m_pDefaultCpuDispatcher;
	physx::PxCudaContextManager* m_pCudaContextManager;
};
