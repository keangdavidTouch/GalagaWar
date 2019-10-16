#pragma once

class GameObject;
class TransformComponent;
struct GameContext;

class BaseComponent
{
	friend class GameObject;
	void RootInitialize(const GameContext& gameContext);
public:
	BaseComponent(const BaseComponent& other) = delete;
	BaseComponent(BaseComponent&& other) noexcept = delete;
	BaseComponent& operator=(const BaseComponent& other) = delete;
	BaseComponent& operator=(BaseComponent&& other) noexcept = delete;
	BaseComponent();
	virtual ~BaseComponent() = default;

	GameObject* GetGameObject() const { return m_pGameObject; }
	TransformComponent* GetTransform() const;

protected:
	virtual void Initialize(const GameContext& gameContext) = 0;
	virtual void Update(const GameContext& gameContext) = 0;
	virtual void Draw(const GameContext& gameContext) = 0;
	virtual void PostDraw(const GameContext&) {}
	virtual void DrawShadowMap(const GameContext& gameContext) { UNREFERENCED_PARAMETER(gameContext); }

	GameObject* m_pGameObject;
	bool m_IsInitialized;
};


