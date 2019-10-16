#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostColorGrading : public PostProcessingMaterial
{
public:
	PostColorGrading();
	PostColorGrading(const PostColorGrading& other) = delete;
	PostColorGrading(PostColorGrading&& other) noexcept = delete;
	PostColorGrading& operator=(const PostColorGrading& other) = delete;
	PostColorGrading& operator=(PostColorGrading&& other) noexcept = delete;
	virtual ~PostColorGrading() = default;

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariable;
};
