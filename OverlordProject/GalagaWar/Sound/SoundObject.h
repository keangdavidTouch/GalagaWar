#pragma once

class SoundObject
{
public:
	SoundObject();
	~SoundObject() = default;
	
	FMOD::Channel* GetChannel();
	void Play(size_t index);
	void AddSound(std::string path, bool stream = false, bool loop = false);

private:

	FMOD::Channel *m_Channel;
	FMOD::System *m_System;
	std::vector<FMOD::Sound*> m_SoundEffects;
};
