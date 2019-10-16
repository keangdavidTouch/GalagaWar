#pragma once
#include "BaseComponent.h"
#include "Enumerations.h"

class CameraComponent: public BaseComponent
{
	friend class GameScene; // TODO: remove this ugly dependency!

	DirectX::XMFLOAT4X4 m_View, m_Projection, m_ViewInverse, m_ViewProjection, m_ViewProjectionInverse;
	float m_FarPlane, m_NearPlane, m_FOV, m_Size;
	bool m_IsActive, m_PerspectiveProjection;

public:
	CameraComponent(const CameraComponent& other) = delete;
	CameraComponent(CameraComponent&& other) noexcept = delete;
	CameraComponent& operator=(const CameraComponent& other) = delete;
	CameraComponent& operator=(CameraComponent&& other) noexcept = delete;
	CameraComponent();
	virtual ~CameraComponent() = default;

	void SetFieldOfView(float fov) { m_FOV = fov; }
	void SetOrthoSize(float size) { m_Size = size; }
	void SetNearClippingPlane(float nearPlane) { m_NearPlane = nearPlane; }
	void SetFarClippingPlane(float farPlane) { m_FarPlane = farPlane; }

	void UsePerspectiveProjection() { m_PerspectiveProjection = true; }
	void UseOrthographicProjection() { m_PerspectiveProjection = false; }

	void SetActive();
	bool IsActive() const {return m_IsActive;}

	const DirectX::XMFLOAT4X4& GetView() const {return m_View;}
	const DirectX::XMFLOAT4X4& GetProjection() const {return m_Projection;}
	const DirectX::XMFLOAT4X4& GetViewProjection() const {return m_ViewProjection;}
	const DirectX::XMFLOAT4X4& GetViewInverse() const {return m_ViewInverse;}
	const DirectX::XMFLOAT4X4& GetViewProjectionInverse() const {return m_ViewProjectionInverse;}

	GameObject* Pick(const GameContext& gameContext, CollisionGroupFlag ignoreGroups = static_cast<CollisionGroupFlag>(0)) const;
	void ShakeCamera();

protected:
	
	void Update(const GameContext& context) override;
	void Draw(const GameContext& context) override;
	void Initialize(const GameContext& gameContext) override;

	//Shaking-Effect
	const float m_ShakeTime = 2.f;
	float m_ShakeElapsedSec = 0.f;

	void DoShakingAnimation(const GameContext& context);
};

