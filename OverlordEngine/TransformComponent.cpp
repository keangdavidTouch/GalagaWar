#include "stdafx.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "ControllerComponent.h"
#include "GameObject.h"

enum TransformChanged
{
	NONE = 0x00,
	TRANSLATION = 0x01,
	ROTATION = 0x02,
	SCALE = 0x04,
};

TransformComponent::TransformComponent():
	m_Position(DirectX::XMFLOAT3(0, 0, 0)),
	m_WorldPosition(DirectX::XMFLOAT3(0, 0, 0)),
	m_Scale(DirectX::XMFLOAT3(1, 1, 1)),
	m_WorldScale(DirectX::XMFLOAT3(1, 1, 1)),
	m_Forward(DirectX::XMFLOAT3(0, 0, 1)),
	m_Up(DirectX::XMFLOAT3(0, 1, 0)),
	m_Right(DirectX::XMFLOAT3(1, 0, 0)),
	m_Rotation(DirectX::XMFLOAT4(0, 0, 0, 1)),
	m_WorldRotation(DirectX::XMFLOAT4(0, 0, 0, 1))
{
	m_IsTransformChanged = TransformChanged::NONE;
}

void TransformComponent::Initialize(const GameContext& )
{
	UpdateTransforms();
}

void TransformComponent::Update(const GameContext& )
{
	// TODO: matrices are updated EVERY FRAME! Fix this
	UpdateTransforms();
}

void TransformComponent::UpdateTransforms()
{
#pragma region code that should go
	// TODO: Remove this butt-ugly dependency on other components
	const auto rigidbody = m_pGameObject->GetComponent<RigidBodyComponent>();
	const auto controller = m_pGameObject->GetComponent<ControllerComponent>();

	if (rigidbody && controller)
	{
		Logger::LogError(L"GameObject cannot have RigidBody and Controller!)");
	}

	if (rigidbody != nullptr && m_IsInitialized)
	{
		if (m_IsTransformChanged & TransformChanged::TRANSLATION)
			rigidbody->Translate(m_Position);
		else
			m_Position = rigidbody->GetPosition();

		if (m_IsTransformChanged & TransformChanged::ROTATION)
			rigidbody->Rotate(m_Rotation);
		else
			m_Rotation = rigidbody->GetRotation();
	}
	else if (controller != nullptr && m_IsInitialized)
	{
		if (m_IsTransformChanged & TransformChanged::TRANSLATION)
			controller->Translate(m_Position);
		else
			m_Position = controller->GetPosition();
	}
#pragma endregion

	//Calculate World Matrix
	//**********************
	auto rot = XMLoadFloat4(&m_Rotation);
	auto world = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) *
		DirectX::XMMatrixRotationQuaternion(rot) *
		DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

	const auto parent = m_pGameObject->GetParent();
	if (parent)
	{
		const auto parentWorld = XMLoadFloat4x4(&parent->GetTransform()->m_World);
		world *= parentWorld;
	}

	XMStoreFloat4x4(&m_World, world);

	//Get World Transform
	DirectX::XMVECTOR pos, scale;
	if (XMMatrixDecompose(&scale, &rot, &pos, world))
	{
		XMStoreFloat3(&m_WorldPosition, pos);
		XMStoreFloat3(&m_WorldScale, scale);
		XMStoreFloat4(&m_WorldRotation, rot);
	}

	const auto rotMat = DirectX::XMMatrixRotationQuaternion(rot);
	const auto forward = XMVector3TransformCoord(DirectX::XMVectorSet(0, 0, 1, 0), rotMat);
	const auto right = XMVector3TransformCoord(DirectX::XMVectorSet(1, 0, 0, 0), rotMat);
	const auto up = DirectX::XMVector3Cross(forward, right);

	XMStoreFloat3(&m_Forward, forward);
	XMStoreFloat3(&m_Right, right);
	XMStoreFloat3(&m_Up, up);

	m_IsTransformChanged = TransformChanged::NONE;
}

void TransformComponent::Draw(const GameContext& )
{
}

void TransformComponent::Translate(float x, float y, float z)
{
	if (!CheckConstraints())
		return;

	m_IsTransformChanged |= TransformChanged::TRANSLATION;
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void TransformComponent::Translate(const DirectX::XMFLOAT3& position)
{
	Translate(position.x, position.y, position.z);
}

void TransformComponent::Translate(const DirectX::XMVECTOR& position)
{
	if (!CheckConstraints())
		return;

	m_IsTransformChanged |= TransformChanged::TRANSLATION;
	XMStoreFloat3(&m_Position, position);
}

void TransformComponent::Rotate(float x, float y, float z, bool isEuler)
{
	if (!CheckConstraints())
		return;

	m_IsTransformChanged |= TransformChanged::ROTATION;
	if (isEuler)
	{
		XMStoreFloat4(&m_Rotation,
		              DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(x),
		                                                        DirectX::XMConvertToRadians(y),
		                                                        DirectX::XMConvertToRadians(z)));
	}
	else
	{
		XMStoreFloat4(&m_Rotation, DirectX::XMQuaternionRotationRollPitchYaw(x, y, z));
	}
}

void TransformComponent::Rotate(const DirectX::XMFLOAT3& rotation, bool isEuler)
{
	Rotate(rotation.x, rotation.y, rotation.z, isEuler);
}

void TransformComponent::Rotate(const DirectX::XMVECTOR& rotation, bool isQuaternion)
{
	if (!CheckConstraints())
		return;

	m_IsTransformChanged |= TransformChanged::ROTATION;
	if (isQuaternion)
	{
		XMStoreFloat4(&m_Rotation, rotation);
	}
	else
	{
		Logger::LogFixMe(L"TransformComponent::Rotate(XMVECTOR, non-Quad) > Not Implemented Yet!");
	}
}

void TransformComponent::ScaleUniform(float scaleFactor)
{
	Scale(scaleFactor, scaleFactor, scaleFactor);
}

void TransformComponent::Scale(float x, float y, float z)
{
	if (!CheckConstraints())
		return;

	m_IsTransformChanged |= TransformChanged::SCALE;
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
}

void TransformComponent::Scale(const DirectX::XMFLOAT3& scale)
{
	Scale(scale.x, scale.y, scale.z);
}

void TransformComponent::LerpRotation(DirectX::XMFLOAT3 from, DirectX::XMFLOAT3 to, float lerp)
{
	auto fromAngle = DirectX::XMLoadFloat3(&from);
	auto toAngle = DirectX::XMLoadFloat3(&to);

	auto rotationVector = DirectX::XMVectorLerp(fromAngle, toAngle, lerp);
	auto rotationFloat = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&rotationFloat, rotationVector);
	Rotate(rotationFloat, false);
}

void TransformComponent::LerpPosition(DirectX::XMFLOAT3 from, DirectX::XMFLOAT3 to, float lerp)
{
	auto fromPos = DirectX::XMLoadFloat3(&from);
	auto toPos = DirectX::XMLoadFloat3(&to);

	auto posVector = DirectX::XMVectorLerp(fromPos, toPos, lerp);
	auto posFloat = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&posFloat, posVector);
	Translate(posFloat);
}

float TransformComponent::GetAngleBetweenVector(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& target)
{
	auto forward = DirectX::XMFLOAT3(0,0,1);
	auto up = DirectX::XMFLOAT3(0,1,0);
	auto referenceForward = XMLoadFloat3(&forward);
	auto upVec = XMLoadFloat3(&up);
	auto referenceRight = DirectX::XMVector3AngleBetweenVectors(upVec, referenceForward);

	auto fromVector = DirectX::XMLoadFloat3(&origin);
	auto toVector = DirectX::XMLoadFloat3(&target);
	auto newDirection = DirectX::XMVectorSubtract(toVector, fromVector);

	auto angle = DirectX::XMVector3AngleBetweenVectors(newDirection, referenceForward);

	/*auto dotProduct = XMVector3Dot(newDirection, referenceRight);
	auto dot = XMFLOAT3();
	XMStoreFloat3(&dot, dotProduct); 
	float angleSign = physx::intrinsics::sign(dot.y);*/

	auto rot = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&rot, angle);

	if(target.x < origin.x)
	{
		rot.y *= -1;
	}

	rot.y += DirectX::XM_PI;
	return rot.y;
}

bool TransformComponent::CheckConstraints() const
{
	if (!m_IsInitialized)
		return true;

	const auto rigidBody = GetGameObject()->GetComponent<RigidBodyComponent>();
	if (rigidBody != nullptr && rigidBody->IsStatic())
	{
		Logger::LogWarning(L"[TransformComponent] Constraint Broken: GameObject with a static rigid body cannot move!");
		return false;
	}

	return true;
}
