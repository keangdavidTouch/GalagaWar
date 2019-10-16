#pragma once
#include "Singleton.h"

enum class State
{
	Intro, Playing, Stop, Pause
};

enum class WinState
{
	None, Win, Lose 
};

class StateManager: public Singleton<StateManager>
{

public:

	StateManager(const StateManager& other) = delete;
	StateManager(StateManager&& other) noexcept = delete;
	StateManager& operator=(const StateManager& other) = delete;
	StateManager& operator=(StateManager&& other) noexcept = delete;
	StateManager();
	virtual ~StateManager() = default;

	void Update(const GameContext& context);
	State GetState() const { return m_State; };
	void SetState(State state) { m_State = state; };
	WinState GetWinState() const { return m_PlayState; };
	void SetWinState(WinState state) { m_PlayState = state; };
private:
	State m_State;
	WinState m_PlayState;
	float m_LoseElapsedSec;
};