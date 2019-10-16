#include "stdafx.h"
#include "CameraComponent.h"
#include "OverlordGame.h"
#include "TransformComponent.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "GameScene.h"
#include "RigidBodyComponent.h"


CameraComponent::CameraComponent():
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(DirectX::XM_PIDIV4),
	m_Size(25.0f),
	m_IsActive(true),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, DirectX::XMMatrixIdentity());
}

void CameraComponent::Initialize(const GameContext&) {}

void CameraComponent::Update(const GameContext&)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	const auto windowSettings = OverlordGame::GetGameSettings().Window;
	DirectX::XMMATRIX projection;

	if(m_PerspectiveProjection)
	{
		projection = DirectX::XMMatrixPerspectiveFovLH(m_FOV, windowSettings.AspectRatio ,m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size>0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
		const float viewHeight = (m_Size>0) ? m_Size : windowSettings.Height;
		projection = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const DirectX::XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const DirectX::XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const DirectX::XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const DirectX::XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const DirectX::XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::Draw(const GameContext&) {}

void CameraComponent::SetActive()
{
	auto gameObject = GetGameObject();
	if(gameObject == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game object is null");
		return;
	}

	auto gameScene = gameObject->GetScene();
	if(gameScene == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game scene is null");
		return;
	}

	gameScene->SetActiveCamera(this);
}

GameObject* CameraComponent::Pick(const GameContext& gameContext, CollisionGroupFlag ignoreGroups) const
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(ignoreGroups);
		
	// Convert mouse coordinates to NDC
	const float halfWidth = OverlordGame::GetGameSettings().Window.Width / 2.f;
	const float halfHeight = OverlordGame::GetGameSettings().Window.Height / 2.f;
	auto mousePos = gameContext.pInput->GetMousePosition();
	DirectX::XMFLOAT2 mousePosNDC{};
	mousePosNDC.x = (mousePos.x - halfWidth) / halfWidth;
	mousePosNDC.y = (halfHeight - mousePos.y) / halfHeight;

	const auto nearPlaneFloat4 = DirectX::XMFLOAT4(mousePosNDC.x, mousePosNDC.y, 0, 0);
	const auto farPlaneFloat4 = DirectX::XMFLOAT4(mousePosNDC.x, mousePosNDC.y, 1, 0);

	const DirectX::XMVECTOR mouseVecNearPlane = DirectX::XMLoadFloat4(&nearPlaneFloat4);
	const DirectX::XMVECTOR mouseVecFarPlane = DirectX::XMLoadFloat4(&farPlaneFloat4);

	/*DirectX::XMMATRIX inverseMatrix = DirectX::XMLoadFloat4x4(&GetViewProjectionInverse());
	DirectX::XMVECTOR nearPoint = DirectX::XMVector4Transform(mouseVecNearPlane, inverseMatrix);
	DirectX::XMVECTOR farPoint = DirectX::XMVector4Transform(mouseVecFarPlane, inverseMatrix);*/

	DirectX::XMMATRIX inverseMatrix = DirectX::XMLoadFloat4x4(&GetViewProjectionInverse());
	DirectX::XMVECTOR nearPoint = DirectX::XMVector3TransformCoord(mouseVecNearPlane, inverseMatrix); //TODO:: what is the different between XMVector3TransformCoord and XMVector4Transform
	DirectX::XMVECTOR farPoint = DirectX::XMVector3TransformCoord(mouseVecFarPlane, inverseMatrix);

	DirectX::XMFLOAT3 rayStartFLOAT3{};
	DirectX::XMStoreFloat3(&rayStartFLOAT3, nearPoint);

	DirectX::XMFLOAT3 rayDirFLOAT3{};
	DirectX::XMStoreFloat3(&rayDirFLOAT3,  DirectX::XMVectorSubtract(farPoint, nearPoint));

	physx::PxVec3 rayStart = ToPxVec3(rayStartFLOAT3);
	physx::PxVec3 rayDir = ToPxVec3(rayDirFLOAT3);
	//rayStart.normalize();
	rayDir.normalize();

	physx::PxQueryFilterData filterData;
	filterData.data.word0 = ~ignoreGroups; //TODO: what is ~ for?

	physx::PxRaycastBuffer hit;

	auto physxProxy = GetGameObject()->GetScene()->GetPhysxProxy()->GetPhysxScene();
	
	if (physxProxy->raycast(rayStart, rayDir, PX_MAX_F32, hit, physx::PxHitFlag::eDEFAULT, filterData))
	{
		return ((RigidBodyComponent*)hit.block.actor->userData)->GetGameObject();
	}

	//TODO implement
	return nullptr;
}

void CameraComponent::ShakeCamera()
{
	m_ShakeElapsedSec += 0.0001f;
}

void CameraComponent::DoShakingAnimation(const GameContext& gameContext)
{
	using namespace DirectX;

	if(m_ShakeElapsedSec > 0.f && m_ShakeElapsedSec < m_ShakeTime)
	{
		m_ShakeElapsedSec += gameContext.pGameTime->GetElapsed();
		float lerp = 0.5F * (1.0F + sin(XM_PI * gameContext.pGameTime->GetTotal() * 15));
		GetTransform()->LerpPosition( XMFLOAT3(-1, 200, 1), XMFLOAT3(1, 200, -1), lerp);
	}
	else
	{
		m_ShakeElapsedSec = 0;
	}
}