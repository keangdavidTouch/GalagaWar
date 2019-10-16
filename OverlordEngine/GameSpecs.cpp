#include "stdafx.h"
#include "GameSpecs.h"
#include "GameTime.h"

int GameSpecs::m_FPS = 0;
float GameSpecs::m_FpsElapsed = 0.0f;
int GameSpecs::m_FpsFrames = 0;

void GameSpecs::Update(const GameContext& context)
{
	++m_FpsFrames;
	m_FpsElapsed += context.pGameTime->GetElapsed();
	if(m_FpsElapsed >= 1.0f)
	{
		m_FPS = m_FpsFrames;
		m_FpsFrames = 0;
		m_FpsElapsed -= 1.0f;
	}
}
