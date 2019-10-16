#pragma once
#include "EffectHelper.h"

class MeshFilter;
class ModelComponent;
struct GameContext;

class Material
{
public:
	Material(std::wstring  effectFile, std::wstring  technique = L"");
	virtual ~Material();

	Material(const Material& other) = delete;
	Material(Material&& other) noexcept = delete;
	Material& operator=(const Material& other) = delete;
	Material& operator=(Material&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext);
	void SetEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);
	ID3DX11EffectTechnique* GetTechnique() const { return m_pTechnique; }
	ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout; }

protected:

	virtual void LoadEffectVariables() = 0;
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) = 0;

	ID3DX11Effect* GetEffect() const { return m_pEffect; }
	
private:
	// TODO: get rid of this nasty dependency
	friend class MeshFilter;

	bool LoadEffect(const GameContext& gameContext);
	bool LoadCompiledEffect(const GameContext& gameContext);

	std::wstring m_effectFile;
	ID3DX11Effect* m_pEffect;

	std::wstring m_TechniqueName;
	ID3DX11EffectTechnique* m_pTechnique;

	ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable;
	ID3DX11EffectMatrixVariable* m_pViewMatrixVariable;
	ID3DX11EffectMatrixVariable* m_pViewInverseMatrixVariable;
	ID3DX11EffectMatrixVariable* m_pWvpMatrixVariable;

	ID3D11InputLayout* m_pInputLayout;
	std::vector<ILDescription> m_pInputLayoutDescriptions;
	unsigned int m_pInputLayoutSize;
	unsigned int m_InputLayoutID;

	bool m_IsInitialized;
};
