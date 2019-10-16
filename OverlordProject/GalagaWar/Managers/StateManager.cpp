#include "stdafx.h"
#include "StateManager.h"

StateManager::StateManager()
	: m_State(State::Intro)
	, m_PlayState(WinState::None)
	, m_LoseElapsedSec(0.f)
{
}

void StateManager::Update(const GameContext& gameContext)
{
	if (m_State == State::Stop)
	{
		if (m_PlayState == WinState::Lose)
		{
			m_LoseElapsedSec += gameContext.pGameTime->GetElapsed();

			if (m_LoseElapsedSec >= 0.75f)
			{
				m_State = State::Pause;
				m_LoseElapsedSec = 0;
			}
		}
		else
		{
			m_State = State::Pause;
		}
	}
}

