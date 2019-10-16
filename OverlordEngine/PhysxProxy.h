#pragma once
struct GameContext;
class GameScene;

class PhysxProxy : public physx::PxSimulationEventCallback
{
public:
	PhysxProxy();
	~PhysxProxy();

	PhysxProxy(const PhysxProxy& other) = delete;
	PhysxProxy(PhysxProxy&& other) noexcept = delete;
	PhysxProxy& operator=(const PhysxProxy& other) = delete;
	PhysxProxy& operator=(PhysxProxy&& other) noexcept = delete;

	physx::PxScene* GetPhysxScene() const { return m_pPhysxScene; }
	physx::PxControllerManager* GetControllerManager() const { return m_pControllerManager; }

	void EnablePhysxDebugRendering(bool enable) { m_DrawPhysx = enable; }

	static void EnablePhysXFrameStepping(bool enable) { m_PhysXFrameStepping = enable; }
	static void NextPhysXFrame(float time = 0.03f) { m_PhysXStepTime = time; }

	void Update(const GameContext& gameContext) const;
	void Draw(const GameContext& gameContext) const;
	bool Raycast(const physx::PxVec3& origin, const physx::PxVec3& unitDir, physx::PxReal distance,
	             physx::PxRaycastCallback& hitCall,
	             physx::PxHitFlags hitFlags = physx::PxHitFlags(physx::PxHitFlag::eDEFAULT),
	             const physx::PxQueryFilterData& filterData = physx::PxQueryFilterData(),
	             physx::PxQueryFilterCallback* filterCall = nullptr,
	             const physx::PxQueryCache* cache = nullptr) const;

private:
	void Initialize(GameScene* parent);

	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override;
	void onWake(physx::PxActor** actors, physx::PxU32 count) override;
	void onSleep(physx::PxActor** actors, physx::PxU32 count) override;
	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs,
	               physx::PxU32 nbPairs) override;
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;

	physx::PxScene* m_pPhysxScene;
	physx::PxControllerManager* m_pControllerManager;

	bool m_DrawPhysx;

	//Static debug variables
	static bool m_PhysXFrameStepping;
	static float m_PhysXStepTime;

	friend class GameScene;
};
