#include "stdafx.h"
#include "PauseUIObject.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "GameScene.h"
#include "TextObject.h"
#include "ContentManager.h"
#include "SpriteFont.h"
#include "../Managers/StateManager.h"
#define WM_Quit 0x0012

PauseUIObject::PauseUIObject(SpriteFont* font, std::function<void()> functor)
	: m_pFont(font)
	, m_RestartCallback(functor)
	, m_Sprites()
	, m_Buttons()
	, m_TextColor(DirectX::XMFLOAT4(1,1,1,1))
	, m_TextString()
{
}

void PauseUIObject::Initialize(const GameContext & /*gameContext*/)
{
	auto textPos = DirectX::XMFLOAT2(0,0);
	m_Text = new TextObject(m_pFont, m_PauseString, textPos);
	AddChild(m_Text);

	//Background, Window Frame
	auto pauseBackground = new GameObject();
	pauseBackground->AddComponent(new SpriteComponent(L"./Resources/Textures/UI/BlackScreen.png", DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT4(1, 1, 1, 0.7f)));
	auto windowFrame = new GameObject();
	windowFrame->GetTransform()->Scale(1.25, 2, 1.25);
	windowFrame->AddComponent(new SpriteComponent(L"./Resources/Textures/Buttons/WindowFrame.png", DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT4(0, 1, 1, 1)));
	windowFrame->GetTransform()->Translate(HalfWidth, HalfHeight, 0);

	//Button GameObjects
	auto resumeButton = new GameObject();
	auto resumeSprite = new SpriteComponent(L"./Resources/Textures/Buttons/resume.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	resumeButton->AddComponent(resumeSprite);
	auto restartButton = new GameObject();
	auto restartSprite = new SpriteComponent(L"./Resources/Textures/Buttons/restart.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	restartButton->AddComponent(restartSprite);
	auto exitButton = new GameObject();
	auto exitSprite = new SpriteComponent(L"./Resources/Textures/Buttons/exit.png", DirectX::XMFLOAT2(0.5f, 0.5f));
	exitButton->AddComponent(exitSprite);
	
	//Add Objects
	AddChild(pauseBackground);
	AddChild(windowFrame);
	AddChild(resumeButton);
	AddChild(restartButton);
	AddChild(exitButton);
	//Sprites
	m_Sprites.push_back(resumeSprite);
	m_Sprites.push_back(restartSprite);
	m_Sprites.push_back(exitSprite);
	//Buttons
	m_Buttons.push_back(resumeButton);
	m_Buttons.push_back(restartButton);
	m_Buttons.push_back(exitButton);

	ResetButtons();
	UpdateText();
}

void PauseUIObject::Update(const GameContext & context)
{
	// Update in Pause-State Only
	if (StateManager::GetInstance()->GetState() != State::Pause) 
		return;

	//Update Input Event
	UpdateTriggerEvent(context);
	//Reset Buttons Appearance to Default
	ResetButtons();
	//Change SELECTED button
	m_Sprites[m_SelectedButtonIndex]->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	m_Buttons[m_SelectedButtonIndex]->GetTransform()->ScaleUniform(1);

	auto winState = StateManager::GetInstance()->GetWinState();

	if (winState != WinState::None) {

		//Hide RESUME button
		m_Sprites[0]->SetColor(DirectX::XMFLOAT4(1, 1, 1, 0));
		//Change other buttons position
		auto buttonPos = m_Buttons[1]->GetTransform()->GetPosition();
		m_Buttons[1]->GetTransform()->Translate(buttonPos.x, buttonPos.y - m_ButtonPadding/2, 0);
		m_Buttons[2]->GetTransform()->Translate(buttonPos.x, buttonPos.y + m_ButtonPadding/2, 0);
		//Change Text
		switch (winState)
		{
			case WinState::Win: 
				m_TextString = L"YOU WIN";
				m_TextColor = DirectX::XMFLOAT4(1,1,0,1);
				break;
			case WinState::Lose:
				m_TextString = L"YOU LOSE";
				m_TextColor = DirectX::XMFLOAT4(1,0,0,1);
				break;
			default: break;
		}
	}
	
	//Reset TextObject Appearance
	UpdateText();
}

void PauseUIObject::UpdateTriggerEvent(const GameContext & context)
{
	auto stateManager = StateManager::GetInstance();
	auto winState = StateManager::GetInstance()->GetWinState();
	auto nrOfNumber = int(m_Buttons.size());

	//Scrolling Buttons using Thumbstick
	auto thumbstickPos = InputManager::GetThumbstickPosition();

	if(thumbstickPos.y == 0)
		m_ShouldSelectButton = true;

	if (context.pInput->IsActionTriggered(12) || thumbstickPos.y < -0.5f && m_ShouldSelectButton)
	{
		m_SelectedButtonIndex += 1;
		if (m_SelectedButtonIndex >= nrOfNumber)
			m_SelectedButtonIndex = 0;

		m_ShouldSelectButton = false;
	}

	if (context.pInput->IsActionTriggered(13) || thumbstickPos.y > 0.5f && m_ShouldSelectButton)
	{
		m_SelectedButtonIndex -= 1;
		if (m_SelectedButtonIndex < 0)
			m_SelectedButtonIndex = nrOfNumber - 1;

		m_ShouldSelectButton = false;
	}

	//Select button pressing A-Button
	if (context.pInput->IsActionTriggered(20))
	{
		switch (m_SelectedButtonIndex)
		{
		case 0:
			// Resume
			if (winState == WinState::None) {
				stateManager->SetState(State::Playing);
				GetScene()->RemoveChild(this, false);
			}
			break;
		case 1:
			// Restart
			m_RestartCallback();
			GetScene()->RemoveChild(this, false);
			break;
		case 2: 
			PostQuitMessage(WM_Quit); 
			break;
		}
	}
}

void PauseUIObject::ResetButtons()
{
	//Reset Button colors
	for (size_t i = 0; i < m_Buttons.size(); ++i)
		m_Sprites[i]->SetColor(DirectX::XMFLOAT4(1, 1, 1, 0.5f));
	for (size_t i = 0; i < m_Buttons.size(); ++i)
		m_Buttons[i]->GetTransform()->ScaleUniform(0.9f);

	m_TextString = m_PauseString;
	m_TextColor = DirectX::XMFLOAT4(1,1,1,1);
	m_Buttons[0]->GetTransform()->Translate(HalfWidth, HalfHeight - m_ButtonPadding, 0);
	m_Buttons[1]->GetTransform()->Translate(HalfWidth, HalfHeight, 0);
	m_Buttons[2]->GetTransform()->Translate(HalfWidth, HalfHeight + m_ButtonPadding, 0);
}

void PauseUIObject::UpdateText()
{
	auto textPos = DirectX::XMFLOAT2(HalfWidth - (m_TextString.length() * 13), HalfHeight - 275);
	m_Text->SetPosition(textPos);
	m_Text->SetText(m_TextString);
	m_Text->SetColor(m_TextColor);
}
