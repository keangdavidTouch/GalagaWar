#pragma once
#include "ContentLoader.h"
#include "TextureData.h"

class TextureDataLoader : public ContentLoader<TextureData>
{
public:
	TextureDataLoader() = default;
	virtual ~TextureDataLoader() = default;

	TextureDataLoader(const TextureDataLoader& other) = delete;
	TextureDataLoader(TextureDataLoader&& other) noexcept = delete;
	TextureDataLoader& operator=(const TextureDataLoader& other) = delete;
	TextureDataLoader& operator=(TextureDataLoader&& other) noexcept = delete;

protected:
	TextureData* LoadContent(const std::wstring& assetFile) override;
	void Destroy(TextureData* objToDestroy) override;

};

