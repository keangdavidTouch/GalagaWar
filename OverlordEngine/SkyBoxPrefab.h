#pragma once
#include "GameObject.h"

class SkyBoxMaterial;

class SkyBoxPrefab final : public GameObject
{
	float m_Width, m_Height, m_Depth;
	DirectX::XMFLOAT4 m_Color;

public:
	SkyBoxPrefab(const std::wstring& assetFile);
	SkyBoxPrefab(const SkyBoxPrefab& other) = delete;
	SkyBoxPrefab(SkyBoxPrefab&& other) noexcept = delete;
	SkyBoxPrefab& operator=(const SkyBoxPrefab& other) = delete;
	SkyBoxPrefab& operator=(SkyBoxPrefab&& other) noexcept = delete;
	virtual ~SkyBoxPrefab() = default;

	void SetSkyBox(const std::wstring& assetFile);

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
private:
	std::wstring m_AssetPath;
	GameObject* m_Box;
	SkyBoxMaterial* m_SkyBoxMat;
};
