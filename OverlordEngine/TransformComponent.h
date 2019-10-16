#pragma once
#include "BaseComponent.h"

class TransformComponent : public BaseComponent
{
	DirectX::XMFLOAT3 m_Position, m_WorldPosition, m_Scale, m_WorldScale, m_Forward, m_Up, m_Right;
	DirectX::XMFLOAT4 m_Rotation, m_WorldRotation;
	DirectX::XMFLOAT4X4 m_World {};
	unsigned char m_IsTransformChanged;

public:
	TransformComponent(const TransformComponent& other) = delete;
	TransformComponent(TransformComponent&& other) noexcept = delete;
	TransformComponent& operator=(const TransformComponent& other) = delete;
	TransformComponent& operator=(TransformComponent&& other) noexcept = delete;
	TransformComponent();
	virtual ~TransformComponent() = default;

	void Translate(float x, float y, float z);
	void Translate(const DirectX::XMFLOAT3& position);
	void Translate(const DirectX::XMVECTOR& position);

	void Rotate(float x, float y, float z, bool isEuler = true);
	void Rotate(const DirectX::XMFLOAT3& rotation, bool isEuler = true);
	void Rotate(const DirectX::XMVECTOR& rotation, bool isQuaternion = true);

	void ScaleUniform(float scaleFactor);
	void Scale(float x, float y, float z);
	void Scale(const DirectX::XMFLOAT3& scale);

	void LerpRotation(DirectX::XMFLOAT3 from, DirectX::XMFLOAT3 to, float lerp);
	void LerpPosition(DirectX::XMFLOAT3 from, DirectX::XMFLOAT3 to, float lerp);
	static float GetAngleBetweenVector(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& target); 

	const DirectX::XMFLOAT3& GetPosition() const { return m_Position; }
	const DirectX::XMFLOAT3& GetWorldPosition() const { return m_WorldPosition; }
	const DirectX::XMFLOAT3& GetScale() const { return m_Scale; }
	const DirectX::XMFLOAT3& GetWorldScale() const { return m_WorldScale; }
	const DirectX::XMFLOAT4& GetRotation() const { return m_Rotation; }
	const DirectX::XMFLOAT4& GetWorldRotation() const { return m_WorldRotation; }
	const DirectX::XMFLOAT4X4& GetWorld() const { return m_World; }

	const DirectX::XMFLOAT3& GetForward() const { return m_Forward; }
	const DirectX::XMFLOAT3& GetUp() const { return m_Up; }
	const DirectX::XMFLOAT3& GetRight() const { return m_Right; }

protected:

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

	void UpdateTransforms();
	bool CheckConstraints() const;
};
