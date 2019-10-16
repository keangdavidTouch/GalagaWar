#include "stdafx.h"
#include "SoundObject.h"
#include "SoundManager.h"

SoundObject::SoundObject()
	: m_Channel()
	, m_SoundEffects(std::vector<FMOD::Sound*>())
{
	m_Channel->setVolume(1.f);
	m_System = SoundManager::GetInstance()->GetSystem();
}

void SoundObject::AddSound(std::string path, bool stream, bool loop)
{ 
	FMOD::Sound* sound = nullptr;
	
	if (stream)
		m_System->createStream(std::move(path).c_str(), FMOD_2D, 0, &sound);
	else 
		m_System->createSound(std::move(path).c_str(), FMOD_2D, 0, &sound);

	sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

	m_SoundEffects.push_back(sound);
}

void SoundObject::Play(size_t index)
{
	m_Channel->setPaused(true);

	if (index <= m_SoundEffects.size()) {
		m_System->playSound(m_SoundEffects[index], 0, false, &m_Channel);
	}
	else {
		Logger::LogWarning(L"Could not play sound effect");
	}
}

FMOD::Channel * SoundObject::GetChannel()
{
	return m_Channel;
}
