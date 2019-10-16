#include "stdafx.h"
#include "GalagaCamera.h"
#include "TransformComponent.h"
#include "Managers/StateManager.h"

void GalagaCamera::Initialize(const GameContext&)
{
	AddComponent(new CameraComponent());
	GetTransform()->Rotate(DirectX::XM_PIDIV2, 0, 0, false);
	GetTransform()->Translate(0, 200, 0);
}

void GalagaCamera::Update(const GameContext & gameContext)
{
	if (StateManager::GetInstance()->GetState() == State::Stop)
		Shake(gameContext);

	if (m_ShouldAnimated) {

		using namespace DirectX;

		m_CameraLerp += gameContext.pGameTime->GetElapsed();
		
		if (m_CameraLerp <= 1)
		{
			GetTransform()->LerpRotation(XMFLOAT3(0.5f, XM_PI + (XM_PI / 3), 0), XMFLOAT3(XM_PIDIV2, XM_2PI, 0), m_CameraLerp);
			GetTransform()->LerpPosition(XMFLOAT3(35, 18, -45), XMFLOAT3(0, 50, -75), m_CameraLerp);
		}
		else if (m_CameraLerp <= 2)
		{
			GetTransform()->LerpPosition(XMFLOAT3(0, 50, -75), XMFLOAT3(0, 200, 0), m_CameraLerp - 1);
		}
		else {
			m_CameraLerp = 0;
			m_ShouldAnimated = false;
			m_AnimationCallback();
		}
	}
	else {

		if (StateManager::GetInstance()->GetState() == State::Intro) {

			//Intro Animation
			using namespace DirectX;
			float lerp = 0.5F * (1.0F + sin(XM_PI * gameContext.pGameTime->GetTotal() * 0.75f));

			m_Target->GetTransform()->LerpRotation(XMFLOAT3(0, XM_PI, 0.3f), XMFLOAT3(0, XM_PI, -0.3f), lerp);

			const auto camerafromAngleFloat = XMFLOAT3(0.5f, XM_PI + (XM_PI / 3), 0);
			GetTransform()->Rotate(camerafromAngleFloat, false);
			GetTransform()->LerpPosition(XMFLOAT3(35, 18, -52), XMFLOAT3(35, 18, -45), lerp);
		}
	}
}

void GalagaCamera::Shake(const GameContext& gameContext)
{
	float lerp = 0.5F * (1.0F + sin(DirectX::XM_PI * gameContext.pGameTime->GetTotal() * 20));
	GetTransform()->LerpPosition(DirectX::XMFLOAT3(-1.1f, 200, -1), DirectX::XMFLOAT3(1.1f, 200, 1), lerp);
}

void GalagaCamera::Animate(std::function<void()> callback)
{
	m_ShouldAnimated = true;
	m_AnimationCallback = callback;
}

void GalagaCamera::SetTarget(GameObject * target)
{
	m_Target = target;
}
