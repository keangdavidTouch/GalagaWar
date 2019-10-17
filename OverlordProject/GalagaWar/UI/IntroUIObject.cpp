#include "stdafx.h"
#include "IntroUIObject.h"
#include "OverlordGame.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "ContentManager.h"
#include "TextObject.h"
#include "SpriteFont.h"

IntroUIObject::IntroUIObject()
	: m_AnimatedElapsedSec(0.1f)
	, m_Speed(0.75f)
{
}

IntroUIObject::~IntroUIObject()
{
}

void IntroUIObject::Initialize(const GameContext&)
{
	const float width = (float)OverlordGame::GetGameSettings().Window.Width;
	const float height = (float)OverlordGame::GetGameSettings().Window.Height;
	const float halfWidth = width / 2.f;
	const float halfHeight = height / 2.f;

	auto font = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/SF TransRobotics_32.fnt");
	std::wstring text = L"OR PRESS ENTER TO START";
	auto textPos = DirectX::XMFLOAT2(halfWidth - (text.length() * 6.5f), height - 50.f);
	m_TextObject = new TextObject(font, text, textPos);
	AddChild(m_TextObject);

	auto button2Frame = new GameObject();
	button2Frame->AddComponent(new SpriteComponent(L"./Resources/Textures/Buttons/ButtonFrame3.png", DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT4(0, 0.8f, 0.8f, 1)));
	button2Frame->GetTransform()->Scale(0.6f, 0.6f, 0.6f);
	button2Frame->GetTransform()->Translate(width - 50.f, 50.f, 0);
	AddChild(button2Frame);

	auto settingIcon = new GameObject();
	settingIcon->AddComponent(new SpriteComponent(L"./Resources/Textures/Buttons/setting_icon3.png", DirectX::XMFLOAT2(0.5f, 0.5f)));
	settingIcon->GetTransform()->Scale(0.8f, 0.8f, 0.8f);
	settingIcon->GetTransform()->Translate(width - 50.f, 50.f, 0);
	AddChild(settingIcon);

	m_PlayButtonSprite = new SpriteComponent(L"./Resources/Textures/Buttons/start2.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	m_PlayButtonFrameSprite = new SpriteComponent(L"./Resources/Textures/Buttons/ButtonFrame2.png", DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT4(0, 0.8f, 0.8f, 1));
	m_PlayButton = new GameObject();
	m_PlayButton->AddComponent(m_PlayButtonFrameSprite);
	m_PlayButton->AddComponent(m_PlayButtonSprite);
	m_PlayButton->GetTransform()->Translate(halfWidth, height - 125.f, 0);
	AddChild(m_PlayButton);
	//auto playButton = new GameObject();
	//playButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Buttons/start.png", DirectX::XMFLOAT2(0.5f, 0.5f)));
	//playButton->GetTransform()->Scale(0.4f, 0.4f, 0.4f);
	//playButton->GetTransform()->Translate(halfWidth, height - 125.f, 0);
	//AddChild(playButton);
}

void IntroUIObject::Update(const GameContext& gameContext)
{
	m_AnimatedElapsedSec += gameContext.pGameTime->GetElapsed() * m_Speed;

	if (m_AnimatedElapsedSec > 1) {
		m_Speed *= -1;
	}
	else if (m_AnimatedElapsedSec <= 0.1f) {
		m_Speed *= -1;
	}

	/*float lerp = 0.5F * (1.0F + sin(DirectX::XM_PI * gameContext.pGameTime->GetTotal() * 0.5f));
	std::cout << lerp << std::endl;*/

	/*m_PlayButtonSprite->SetColor(DirectX::XMFLOAT4(1, 1, 1, m_AnimatedElapsedSec));
	m_PlayButtonFrameSprite->SetColor(DirectX::XMFLOAT4(0, 1, 1, m_AnimatedElapsedSec));*/
	m_TextObject->SetColor(DirectX::XMFLOAT4(1, 1, 1, m_AnimatedElapsedSec));
}
