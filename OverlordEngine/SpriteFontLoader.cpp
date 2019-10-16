#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "BinaryReader.h"
#include "ContentManager.h"
#include "TextureData.h"
#include <bits.h>

#define GET_BIT_VALUE(where, bit_number) (((where) & (1 << (bit_number))) >> bit_number)

SpriteFont* SpriteFontLoader::LoadContent(const std::wstring& assetFile)
{
	auto pBinReader = new BinaryReader();
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout
	//Parse the Identification bytes (B,M,F)
	const char id1 = pBinReader->Read<byte>();
	 const char id2 = pBinReader->Read<byte>();
	 const char id3 = pBinReader->Read<byte>();
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	//...

	 if (id1 != 'B' || id2 != 'M' || id3 != 'F') {
		 Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Not a valid.fnt font");
		 return nullptr;
	 }

	//Parse the version (version 3 required)
	char version = pBinReader->Read<byte>();
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	//...

	 if (version < 3) {
		 Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
	 }

	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	char blockId = pBinReader->Read<byte>();
	int blockSize = pBinReader->Read<int>();
	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	pSpriteFont->m_FontSize = short(pBinReader->Read<int>());
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	pBinReader->SetBufferPosition(23);
	//Retrieve the FontName [SpriteFont::m_FontName]
	pSpriteFont->m_FontName = pBinReader->ReadNullString();
	//...

	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pBinReader->Read<byte>();
	blockSize = pBinReader->Read<int>(); 
	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	//pBinReader->MoveBufferPosition(4);
	pBinReader->Read<short>();
	pBinReader->Read<short>();
	pSpriteFont->m_TextureWidth = pBinReader->Read<short>();
	pSpriteFont->m_TextureHeight = pBinReader->Read<short>();

	//Retrieve PageCount
	const UINT pageCount = pBinReader->Read<short>();
	//> if pagecount > 1
	//> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed)
	//Advance to Block2 (Move Reader)
	//...t
	if (pageCount > 1) {
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed");
	}
	//std::cout << pBinReader->GetBufferPosition() << std::endl;
	//pBinReader->SetBufferPosition(52);
	pBinReader->MoveBufferPosition(5);

	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pBinReader->Read<byte>();
	blockSize = pBinReader->Read<int>();
	//Retrieve the PageName (store Local)
	const std::wstring pageName = pBinReader->ReadNullString();
	//	> If PageName is empty
	//	> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty])
	if (pageName.empty()) {
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]");
	}
	//>Retrieve texture filepath from the assetFile path
	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	const auto pos = assetFile.rfind(pSpriteFont->m_FontName);
	const std::wstring filePath = assetFile.substr(0, pos);
	//std::wcout << filePath << std::endl;
	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)
	const auto texturePath = filePath + pageName;
	pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(texturePath);

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pBinReader->Read<byte>();
	blockSize = pBinReader->Read<int>();
	//Retrieve Character Count (see documentation)
	const UINT charCount = blockSize/20;
	//Retrieve Every Character, For every Character:
	for (UINT i = 0; i < charCount; i ++)
	{
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		wchar_t charId = wchar_t(pBinReader->Read<UINT>());

		//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
		if (!pSpriteFont->IsCharValid(charId))
		{
			Logger::LogFormat(LogLevel::Warning, L"SpriteFont::IsCharValid");
			continue;;
		}

		//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
		FontMetric* metric = &pSpriteFont->GetMetric(charId);
		//> Set IsValid to true [FontMetric::IsValid]
		metric->IsValid = true;
		//> Set Character (CharacterId) [FontMetric::Character]
		metric->Character = charId;
		//> Retrieve Xposition (store Local)
		const short xPos = pBinReader->Read<short>();
		//> Retrieve Yposition (store Local)
		const short yPos = pBinReader->Read<short>();
		/*std::cout << xPos << ", " << yPos << std::endl;
			std::cout << yPos << std::endl;*/

		//> Retrieve & Set Width [FontMetric::Width]
		metric->Width = pBinReader->Read<short>();
		//> Retrieve & Set Height [FontMetric::Height]
		metric->Height = pBinReader->Read<short>();
		//> Retrieve & Set OffsetX [FontMetric::OffsetX]
		metric->OffsetX = pBinReader->Read<short>();
		//> Retrieve & Set OffsetY [FontMetric::OffsetY]
		metric->OffsetY = pBinReader->Read<short>();
		//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
		metric->AdvanceX = pBinReader->Read<short>();
		//> Retrieve & Set Page [FontMetric::Page]
		metric->Page = pBinReader->Read<char>();
		//> Retrieve Channel (BITFIELD!!!) 
		const char channel = pBinReader->Read<char>();
		
		//std::cout << "Channel: " << channel << std::endl;
		
		if (int(channel) == 1)
		{
			metric->Channel = 2;
		}
		else if(int(channel) == 2)
		{
			metric->Channel = 1;
		}
		else if (int(channel) == 4)
		{
			metric->Channel = 0;
		}
		//	> See documentation for BitField meaning [FontMetrix::Channel]
		//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
		metric->TexCoord.x = float(xPos) / pSpriteFont->m_TextureWidth;
		metric->TexCoord.y = float(yPos) / pSpriteFont->m_TextureHeight;
	}
	
	//DONE :)

	delete pBinReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
