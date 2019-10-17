#include "stdafx.h"
#include "SettingsUIObject.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "OverlordGame.h"
#include "GameScene.h"
#include "TextObject.h"
#include "ContentManager.h"
#include "SpriteFont.h"

SettingUIObject::SettingUIObject(SpriteFont* font)
	: m_pFont(font)
	, m_Sprites()
	, m_Buttons()
{
	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/SF TransRobotics_64.fnt");
}

void SettingUIObject::Initialize(const GameContext & /*gameContext*/)
{
	const float width = (float)OverlordGame::GetGameSettings().Window.Width;
	const float height = (float)OverlordGame::GetGameSettings().Window.Height;

	const float halfWidth = width / 2.f;
	const float halfHeight = height / 2.f;

	std::wstring text = L"SETTINGS";
	auto textPos = DirectX::XMFLOAT2(halfWidth - (text.length() * 13), halfHeight - 175);
	auto m_Text = new TextObject(m_pFont, text, textPos);
	AddChild(m_Text);

	//Background
	auto pauseBackground = new GameObject();
	pauseBackground->AddComponent(new SpriteComponent(L"./Resources/Textures/UI/BlackScreen.png", DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT4(1, 1, 1, 0.7f)));
	//Frame
	auto windowFrame = new GameObject();
	windowFrame->AddComponent(new SpriteComponent(L"./Resources/Textures/Buttons/WindowFrame.png", DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT4(0, 1, 1, 1)));
	windowFrame->GetTransform()->Translate(halfWidth, halfHeight, 0);
	//Sprites
	auto soundSprite = new SpriteComponent(L"./Resources/Textures/Buttons/sound.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	auto musicSprite = new SpriteComponent(L"./Resources/Textures/Buttons/music.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	auto homeSprite = new SpriteComponent(L"./Resources/Textures/Buttons/home.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	auto redCrossSprite = new SpriteComponent(L"./Resources/Textures/Buttons/cross.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	redCrossSprite->SetHidden(true);
	auto redCrossSprite2 = new SpriteComponent(L"./Resources/Textures/Buttons/cross.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	redCrossSprite2->SetHidden(true);
	//Buttons
	auto soundButton = new GameObject();
	soundButton->AddComponent(soundSprite);
	soundButton->AddComponent(redCrossSprite);
	soundButton->GetTransform()->Translate(halfWidth - 100, halfHeight, 0);
	auto musicButton = new GameObject();
	musicButton->AddComponent(musicSprite);
	musicButton->AddComponent(redCrossSprite2);
	musicButton->GetTransform()->Translate(halfWidth, halfHeight, 0);
	auto homeButton = new GameObject();
	homeButton->AddComponent(homeSprite);
	homeButton->GetTransform()->Translate(halfWidth + 100, halfHeight, 0);

	AddChild(pauseBackground);
	AddChild(windowFrame);
	AddChild(soundButton);
	AddChild(musicButton);
	AddChild(homeButton);

	m_Sprites.push_back(soundSprite);
	m_Sprites.push_back(musicSprite);
	m_Sprites.push_back(homeSprite);
	m_Sprites.push_back(redCrossSprite);
	m_Sprites.push_back(redCrossSprite2);

	m_Buttons.push_back(soundButton);
	m_Buttons.push_back(musicButton);
	m_Buttons.push_back(homeButton);
}

void SettingUIObject::Update(const GameContext & gameContext)
{
	auto nrOfNumber = int(m_Buttons.size());

	if (gameContext.pInput->IsActionTriggered(14))
	{
		m_SelectedButtonIndex += 1;
		if (m_SelectedButtonIndex >= nrOfNumber)
			m_SelectedButtonIndex = 0;
	}

	if (gameContext.pInput->IsActionTriggered(15))
	{
		m_SelectedButtonIndex -= 1;
		if (m_SelectedButtonIndex < 0)
			m_SelectedButtonIndex = nrOfNumber - 1;
	}

	//Select Button
	if (gameContext.pInput->IsActionTriggered(20))
	{
		switch (m_SelectedButtonIndex)
		{
		case 0:
			//Set Sound
			m_Sprites[3]->SetHidden(!m_Sprites[3]->IsHidden());
			break;
		case 1:
			//Set Music
			m_Sprites[4]->SetHidden(!m_Sprites[4]->IsHidden());
			break;
		case 2:
			//Exit
			GetScene()->RemoveChild(this, false);
			break;
		}
	}

	//Reset Button Appearance 
	for (int i = 0; i < nrOfNumber; ++i)
		m_Sprites[i]->SetColor(DirectX::XMFLOAT4(1, 1, 1, 0.5f));

	for (int i = 0; i < nrOfNumber; ++i)
		m_Buttons[i]->GetTransform()->ScaleUniform(0.9f);

	//Set Selected Button Appearance
	m_Sprites[m_SelectedButtonIndex]->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	m_Buttons[m_SelectedButtonIndex]->GetTransform()->ScaleUniform(1);
}
