#pragma once
#include "ContentLoader.h"

class EffectLoader : public ContentLoader<ID3DX11Effect>
{
public:
	EffectLoader(const EffectLoader& other) = delete;
	EffectLoader(EffectLoader&& other) noexcept = delete;
	EffectLoader& operator=(const EffectLoader& other) = delete;
	EffectLoader& operator=(EffectLoader&& other) noexcept = delete;
	EffectLoader() = default;
	virtual ~EffectLoader() = default;
protected:
	ID3DX11Effect* LoadContent(const std::wstring& assetFile) override;
	void Destroy(ID3DX11Effect* objToDestroy) override;
};
