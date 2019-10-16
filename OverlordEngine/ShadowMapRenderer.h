#pragma once

class MeshFilter;
class RenderTarget;
class ShadowMapMaterial;
class ShadowMapMaterial_Skinned;

class ShadowMapRenderer final
{
public:
	ShadowMapRenderer() = default;
	~ShadowMapRenderer();

	ShadowMapRenderer(const ShadowMapRenderer& other) = delete;
	ShadowMapRenderer(ShadowMapRenderer&& other) noexcept = delete;
	ShadowMapRenderer& operator=(const ShadowMapRenderer& other) = delete;
	ShadowMapRenderer& operator=(ShadowMapRenderer&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext);

	void SetLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction);
	DirectX::XMFLOAT3 GetLightDirection() const { return m_LightDirection; }
	DirectX::XMFLOAT4X4 GetLightVP() const { return m_LightVP; }

	void Begin(const GameContext& gameContext) const;
	void End(const GameContext& gameContext) const;

	void Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones = std::vector<DirectX::XMFLOAT4X4>()) const;
	void UpdateMeshFilter(const GameContext& gameContext, MeshFilter* pMeshFilter);
	ShadowMapMaterial* GetMaterial() const { return m_pShadowMat; }
	ID3D11ShaderResourceView* GetShadowMap() const;

private:
	ShadowMapMaterial* m_pShadowMat = nullptr;
	RenderTarget* m_pShadowRT = nullptr;
	bool m_IsInitialized = false;

	//LIGHT
	DirectX::XMFLOAT3 m_LightPosition = {}, m_LightDirection = {};
	DirectX::XMFLOAT4X4 m_LightVP = {};
	const float m_Size = 100.0f;
};

