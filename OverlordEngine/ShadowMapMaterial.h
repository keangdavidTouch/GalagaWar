#pragma once
#include "Material.h"

class ShadowMapMaterial final
{
public:
	enum ShadowGenType
	{
		Static,
		Skinned
	};

	ShadowMapMaterial() = default;
	~ShadowMapMaterial();

	ShadowMapMaterial(const ShadowMapMaterial& other) = delete;
	ShadowMapMaterial(ShadowMapMaterial&& other) noexcept = delete;
	ShadowMapMaterial& operator=(const ShadowMapMaterial& other) = delete;
	ShadowMapMaterial& operator=(ShadowMapMaterial&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext);
	void SetLightVP(DirectX::XMFLOAT4X4 lightVP) const;
	void SetWorld(DirectX::XMFLOAT4X4 world) const;
	void SetBones(const float* pData, int count) const;

private:
	friend class ShadowMapRenderer;

	ID3DX11EffectMatrixVariable *m_pWorldMatrixVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pBoneTransforms = nullptr;
	ID3DX11EffectMatrixVariable *m_pLightVPMatrixVariable = nullptr;

	static const int NUM_TYPES = 2;
	ID3DX11Effect* m_pShadowEffect = nullptr;
	ID3DX11EffectTechnique* m_pShadowTechs[NUM_TYPES] = { nullptr, nullptr };
	ID3D11InputLayout *m_pInputLayouts[NUM_TYPES] = { nullptr, nullptr };
	UINT m_InputLayoutSizes[NUM_TYPES] = { 0,0 };
	UINT m_InputLayoutIds[NUM_TYPES] = { 0,0 };
	std::vector<ILDescription> m_InputLayoutDescriptions[NUM_TYPES];

	bool m_IsInitialized = false;
};

