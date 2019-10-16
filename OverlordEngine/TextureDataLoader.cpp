#include "stdafx.h"
#include "TextureDataLoader.h"

TextureData* TextureDataLoader::LoadContent(const std::wstring& assetFile)
{
	ID3D11Resource* pTexture;
	ID3D11ShaderResourceView* pShaderResourceView;

	DirectX::TexMetadata info;
	DirectX::ScratchImage *image = new DirectX::ScratchImage();

	//Find Extension
	std::wstring extension;
	if (assetFile.find_last_of(L".") != std::wstring::npos)
	{
		extension = assetFile.substr(assetFile.find_last_of(L".") + 1);
	}
	else
	{
		Logger::LogFormat(LogLevel::Error, L"TextureDataLoader::LoadContent() > Invalid File Extensions!\nPath: %s",assetFile.c_str());
		return nullptr;
	}
		
	HRESULT hr;
	if (lstrcmpiW(extension.c_str(), L"dds") == 0) //DDS Loader
	{
		hr = LoadFromDDSFile(assetFile.c_str(), DirectX::DDS_FLAGS_NONE, &info, *image);
		if (Logger::LogHResult(hr, L"TextureDataLoader::LoadContent() > LoadFromDDsFile Failed!"))
			return nullptr;
	}
	else if (lstrcmpiW(extension.c_str(), L"tga") == 0) //TGA Loader
	{
		hr = LoadFromTGAFile(assetFile.c_str(), &info, *image);
		if (Logger::LogHResult(hr, L"TextureDataLoader::LoadContent() > LoadFromTGAFile Failed!"))
			return nullptr;
	}
	else //WIC Loader
	{
		hr = LoadFromWICFile(assetFile.c_str(), DirectX::WIC_FLAGS_NONE, &info, *image);
		if (Logger::LogHResult(hr, L"TextureDataLoader::LoadContent() > LoadFromWICFile Failed!"))
			return nullptr;
	}
	

	hr = CreateTexture(m_pDevice, image->GetImages(), image->GetImageCount(),image->GetMetadata(), &pTexture);
	if (Logger::LogHResult(hr, L"TextureDataLoader::LoadContent() > CreateTexture Failed!"))
	{
		SafeDelete(image);
		return nullptr;
	}

	hr = CreateShaderResourceView(m_pDevice, image->GetImages(), image->GetImageCount(), image->GetMetadata(), &pShaderResourceView);
	if(Logger::LogHResult(hr, L"TextureDataLoader::LoadContent() > CreateShaderResourceView Failed!"))
	{
		SafeDelete(image);
		return nullptr;
	}

	
	SafeDelete(image);
	return new TextureData(pTexture, pShaderResourceView);
}

void TextureDataLoader::Destroy(TextureData* objToDestroy)
{
	SafeDelete(objToDestroy);
}
