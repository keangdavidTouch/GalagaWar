#include "stdafx.h"
#include "PickupUIObject.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "TextObject.h"
#include <iomanip>
#include "../Managers/StateManager.h"
#include "../Managers/PickupManager.h"

PickupUIObject::PickupUIObject(SpriteFont* font)
{
	auto textPos = DirectX::XMFLOAT2(80, 15);
	m_TextObject = new TextObject(font, L"0000", textPos);
}

PickupUIObject::~PickupUIObject()
{
}

void PickupUIObject::Initialize(const GameContext & )
{
	
	AddChild(m_TextObject);

	auto starIcon = new GameObject();
	starIcon->AddComponent(new SpriteComponent(L"./Resources/Textures/Buttons/star.png", DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT4(1, 1, 1, 1)));
	starIcon->GetTransform()->Translate(45, 45, 0);
	AddChild(starIcon);
}

void PickupUIObject::PostInitialize(const GameContext &)
{
}

void PickupUIObject::Update(const GameContext & )
{
	// SCORE-TEXT
	std::wstringstream stream;
	stream << std::setfill(L'0') << std::setw(4) << std::to_wstring(PickupManager::GetInstance()->GetCollectedCoin());
	m_TextObject->SetText(stream.str());

	if (StateManager::GetInstance()->GetState() == State::Pause) {
		m_TextObject->SetColor(DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1));
	}
	else {
		m_TextObject->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	}
}
