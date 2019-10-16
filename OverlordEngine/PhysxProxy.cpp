#include "stdafx.h"
#include "PhysxProxy.h"
#include "Components.h"
#include "DebugRenderer.h"
#include "GameScene.h"
#include "GameObject.h"
#include "PhysxManager.h"

#include <characterkinematic/PxControllerManager.h>

bool PhysxProxy::m_PhysXFrameStepping = false;
float PhysxProxy::m_PhysXStepTime = 0.f;

PhysxProxy::PhysxProxy() :
	m_pPhysxScene(nullptr),
	m_pControllerManager(nullptr),
	m_DrawPhysx(false)
{
}

PhysxProxy::~PhysxProxy()
{
	if (m_pControllerManager != nullptr)
		m_pControllerManager->release();
	if (m_pPhysxScene != nullptr)
		m_pPhysxScene->release();
}

void PhysxProxy::Initialize(GameScene* parent)
{
	if (m_pPhysxScene != nullptr)
	{
		Logger::LogError(L"[PhysicsProxy] Initialize called twice!");
		return;
	}

	m_pPhysxScene = PhysxManager::GetInstance()->CreateScene(parent);
	if (m_pPhysxScene == nullptr)
	{
		Logger::LogError(L"[PhysicsProxy] Failed to create physx scene!");
		return;
	}

	m_pPhysxScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	m_pPhysxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	m_pPhysxScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	m_pPhysxScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	m_pPhysxScene->setSimulationEventCallback(this);

	m_pControllerManager = PxCreateControllerManager(*m_pPhysxScene);
	if (m_pControllerManager == nullptr)
		Logger::LogError(L"[PhysicsProxy] Failed to create controller manager!");
}

void PhysxProxy::Update(const GameContext& gameContext) const
{
	if (gameContext.pGameTime->IsRunning() && gameContext.pGameTime->GetElapsed() > 0)
	{
		if (m_PhysXFrameStepping)
		{
			if (m_PhysXStepTime > 0.f)
			{
				m_pPhysxScene->simulate(m_PhysXStepTime);
				m_pPhysxScene->fetchResults(true);
				m_PhysXStepTime = 0.f;
			}
			else if (m_PhysXStepTime < 0.f)
			{
				m_pPhysxScene->simulate(gameContext.pGameTime->GetElapsed());
				m_pPhysxScene->fetchResults(true);
			}
		}
		else
		{
			m_pPhysxScene->simulate(gameContext.pGameTime->GetElapsed());
			m_pPhysxScene->fetchResults(true);
		}
	}

	//Send Camera to PVD
	if (m_pPhysxScene->getPhysics().getPvdConnectionManager())
	{
		const auto transform = gameContext.pCamera->GetTransform();
		m_pPhysxScene->getPhysics().getPvdConnectionManager()->setCamera("SceneCam", ToPxVec3(transform->GetPosition()),
		                                                                 ToPxVec3(transform->GetUp()),
		                                                                 ToPxVec3(transform->GetForward()));
	}
}

void PhysxProxy::Draw(const GameContext& ) const
{
	if (m_DrawPhysx)
		DebugRenderer::DrawPhysX(m_pPhysxScene);
}

void PhysxProxy::onConstraintBreak(physx::PxConstraintInfo* , physx::PxU32 )
{
}

void PhysxProxy::onWake(physx::PxActor** , physx::PxU32 )
{
}

void PhysxProxy::onSleep(physx::PxActor** , physx::PxU32 )
{
}

void PhysxProxy::onContact(const physx::PxContactPairHeader& , const physx::PxContactPair* ,
                           physx::PxU32 )
{
}

void PhysxProxy::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (physx::PxTriggerPairFlag::eDELETED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::
			eDELETED_SHAPE_OTHER))
			continue;

		const auto triggerComponent = reinterpret_cast<BaseComponent*>(((pairs[i].triggerShape)->getActor())->userData);
		const auto otherComponent = reinterpret_cast<BaseComponent*>(((pairs[i].otherShape)->getActor())->userData);


		if (triggerComponent != nullptr && otherComponent != nullptr)
		{
			GameObject* trigger = triggerComponent->GetGameObject();
			GameObject* other = otherComponent->GetGameObject();

			if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
				trigger->OnTrigger(trigger, other, GameObject::TriggerAction::ENTER);
			else if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
				trigger->OnTrigger(trigger, other, GameObject::TriggerAction::LEAVE);
		}
	}
}

bool PhysxProxy::Raycast(const physx::PxVec3& origin, const physx::PxVec3& unitDir, const physx::PxReal distance,
                         physx::PxRaycastCallback& hitCall, physx::PxHitFlags hitFlags,
                         const physx::PxQueryFilterData& filterData, physx::PxQueryFilterCallback* filterCall,
                         const physx::PxQueryCache* cache) const
{
	if (m_pPhysxScene != nullptr)
	{
		return m_pPhysxScene->raycast(origin, unitDir, distance, hitCall, hitFlags, filterData, filterCall, cache);
	}
	Logger::LogError(L"[PhysicsProxy] Raycast failed. Physics scene is a null pointer");
	return false;
}
