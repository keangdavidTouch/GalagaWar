#pragma once
#include "Singleton.h"

class SoundManager: public Singleton<SoundManager>
{
	void Initialize();
	FMOD::System* m_pFmodSystem = nullptr;

public:
	SoundManager(const SoundManager& other) = delete;
	SoundManager(SoundManager&& other) noexcept = delete;
	SoundManager& operator=(const SoundManager& other) = delete;
	SoundManager& operator=(SoundManager&& other) noexcept = delete;
	SoundManager();
	virtual ~SoundManager();

	static bool ErrorCheck(FMOD_RESULT res);
	FMOD::System* GetSystem() const { return m_pFmodSystem; }
};

