#pragma once
#include <typeinfo>
#include <functional>

class BaseComponent;
class TransformComponent;
class GameScene;

class GameObject
{
public:
	enum class TriggerAction
	{
		ENTER,
		LEAVE
	};

	typedef std::function<void (GameObject* triggerobject, GameObject* otherobject, TriggerAction action)> PhysicsCallback;

	GameObject();
	virtual ~GameObject();
	GameObject(const GameObject& other) = delete;
	GameObject(GameObject&& other) noexcept = delete;
	GameObject& operator=(const GameObject& other) = delete;
	GameObject& operator=(GameObject&& other) noexcept = delete;

	void AddChild(GameObject* obj);
	void RemoveChild(GameObject* obj);

	void AddComponent(BaseComponent* pComp);
	void RemoveComponent(BaseComponent* pComp);
	void OnTrigger(GameObject* triggerobject, GameObject* otherobject, TriggerAction action);

	const std::wstring& GetTag() const { return m_Tag; }
	void SetTag(const std::wstring& tag) { m_Tag = tag; }

	TransformComponent* GetTransform() const { return m_pTransform; }

	GameScene* GetScene();
	GameObject* GetParent() const { return m_pParentObject; }

	void SetOnTriggerCallBack(PhysicsCallback callback);

#pragma region
	template <class T>
	bool HasComponent(bool searchChildren = false)
	{
		return GetComponent<T>(searchChildren) != nullptr;
	}

	template <class T>
	T* GetComponent(bool searchChildren = false)
	{
		const type_info& ti = typeid(T);
		for (auto* component : m_pComponents)
		{
			if (component && typeid(*component) == ti)
				return static_cast<T*>(component);
		}

		if (searchChildren)
		{
			for (auto* child : m_pChildren)
			{
				if (child->GetComponent<T>(searchChildren) != nullptr)
					return child->GetComponent<T>(searchChildren);
			}
		}

		return nullptr;
	}

	template <class T>
	std::vector<T*> GetComponents(bool searchChildren = false)
	{
		const type_info& ti = typeid(T);
		std::vector<T*> components;

		for (auto* component : m_pComponents)
		{
			if (component && typeid(*component) == ti)
				components.push_back(static_cast<T*>(component));
		}

		if (searchChildren)
		{
			for (auto* child : m_pChildren)
			{
				auto childComponents = child->GetComponents<T>(searchChildren);

				for (auto* childComp : childComponents)
					components.push_back(static_cast<T*>(childComp));
			}
		}

		return components;
	}

	template <class T>
	T* GetChild()
	{
		const type_info& ti = typeid(T);
		for (auto* child : m_pChildren)
		{
			if (child && typeid(*child) == ti)
				return static_cast<T*>(child);
		}
		return nullptr;
	}

	template <class T>
	std::vector<T*> GetChildren()
	{
		const type_info& ti = typeid(T);
		std::vector<T*> children;

		for (auto* child : m_pChildren)
		{
			if (child && typeid(*child) == ti)
				children.push_back((T*)child);
		}
		return children;
	}
#pragma endregion Template Methods

protected:
	virtual void Initialize(const GameContext& ) {}
	virtual void PostInitialize(const GameContext& ) {}
	virtual void Draw(const GameContext&) {}
	virtual void PostDraw(const GameContext&) {}
	virtual void Update(const GameContext&) {}

private:
	// TODO: try to get rid of this dependency
	friend class GameScene;

	void RootInitialize(const GameContext& gameContext);
	void RootUpdate(const GameContext& gameContext);
	void RootDraw(const GameContext& gameContext);
	void RootPostDraw(const GameContext& gameContext);
	void RootDrawShadowMap(const GameContext& gameContext);

	std::vector<GameObject*> m_pChildren;
	std::vector<BaseComponent*> m_pComponents;

	bool m_IsInitialized, m_IsActive;
	GameScene* m_pParentScene;
	GameObject* m_pParentObject;
	TransformComponent* m_pTransform;
	PhysicsCallback m_OnTriggerCallback;
	std::wstring m_Tag;
};
